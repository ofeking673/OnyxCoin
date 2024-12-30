#include "Argon2.h"

Argon2::Argon2(Type type, uint32_t timeCost, uint32_t memoryCost, uint32_t parallelism, uint32_t hashLength)
    : type_(type), timeCost_(timeCost), parallelism_(parallelism), hashLength_(hashLength) {

    lanes_ = parallelism_;

    // Adjust memoryCost_ to meet Argon2 requirements
    uint32_t minMemory = 8 * lanes_;
    uint32_t memoryBlocks = memoryCost_;

    if (memoryBlocks < minMemory) {
        memoryBlocks = minMemory;
    }

    uint32_t blockMultiple = 4 * lanes_;

    if (memoryBlocks % blockMultiple != 0) {
        memoryBlocks = ((memoryBlocks + blockMultiple - 1) / blockMultiple) * blockMultiple;
    }

    memoryCost_ = memoryBlocks;

    laneLength_ = memoryCost_ / lanes_;
    segmentLength_ = laneLength_ / 4;

    // Initialize memory blocks
    memory_.resize(memoryCost_);
}

std::vector<uint8_t> Argon2::deriveKey(const std::string password, const std::vector<uint8_t> salt)
{
    // Initialize internal state with the password and salt
    initialize(password, salt);

    // Fill memory blocks according to the Argon2 algorithm
    fillMemoryBlocks();

    // Prepare the output buffer for the derived key
    std::vector<uint8_t> output;

    // Finalize and produce the derived key
    finalize(output);

    // Return the derived key
    return output;
}

cpp_int Argon2::castVectorToCPPInt(std::vector<uint8_t>& vector)
{
    std::ostringstream oss;
    for (size_t i = 0; i < vector.size(); ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(vector[i]);
    return cpp_int("0x" + oss.str());
}

void Argon2::initialize(const std::string& password, const std::vector<uint8_t>& salt)
{
    // Argon2 version number
    const uint32_t version = 0x13;

    // Convert lengths to uint32_t
    uint32_t pwdLength = static_cast<uint32_t>(password.size());
    uint32_t saltLength = static_cast<uint32_t>(salt.size());

    // Ensure password and salt are provided
    if (pwdLength == 0) {
        throw std::invalid_argument("Password must not be empty.");
    }
    if (saltLength == 0) {
        throw std::invalid_argument("Salt must not be empty.");
    }

    // Prepare the pre-hashing input buffer
    std::vector<uint8_t> prehash;

    // Helper lambda to append a uint32_t in little-endian order
    auto append_uint32_le = [&](uint32_t value) {
        prehash.push_back(static_cast<uint8_t>(value & 0xFF));
        prehash.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        prehash.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        prehash.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    };

    // Append core parameters
    append_uint32_le(lanes_);           // p: Degree of parallelism
    append_uint32_le(hashLength_);      // T: Hash output length
    append_uint32_le(memoryCost_);      // m: Memory size in kibibytes
    append_uint32_le(timeCost_);        // t: Number of iterations
    append_uint32_le(version);          // Version number (0x13)
    append_uint32_le(static_cast<uint32_t>(type_)); // y: Argon2 type

    // Append password
    append_uint32_le(pwdLength);        // Password length
    prehash.insert(prehash.end(), password.begin(), password.end()); // Password bytes

    // Append salt
    append_uint32_le(saltLength);       // Salt length
    prehash.insert(prehash.end(), salt.begin(), salt.end()); // Salt bytes

    // Append empty secret and associated data
    append_uint32_le(0); // Secret length (zero)
    // No secret data
    append_uint32_le(0); // Associated data length (zero)
    // No associated data

    // Compute H0 using Blake2b hash function
    // Ensure initialHash_ has sufficient size
    static_assert(sizeof(initialHash_) >= Blake2b::HASH_SIZE, "initialHash_ buffer is too small.");

    Blake2b::hash(prehash.data(), prehash.size(), initialHash_);

    // Initialize memory blocks to zero securely
    memory_.resize(memoryCost_);

    for (auto& block : memory_) {
        std::fill(std::begin(block.v), std::end(block.v), 0);
    }

    // Initialize the first two blocks in each lane
    initializeFirstBlocks();

    // Additional initialization steps (if any) can be added here
}

void Argon2::fillMemoryBlocks()
{
    // Number of passes (iterations)
    const uint32_t passes = timeCost_;

    // Initialize the pseudo-random values generator for Argon2i and Argon2id
    if (type_ == Argon2i || type_ == Argon2id) {
        initializeAddressBlocks();
    }

    // Loop over each pass
    for (uint32_t pass = 0; pass < passes; ++pass) {
        // Loop over each segment
        for (uint32_t segment = 0; segment < 4; ++segment) {
            // Process each lane in parallelism degree
            for (uint32_t lane = 0; lane < lanes_; ++lane) {
                fillSegment(pass, lane, segment);
            }
        }
    }
}

void Argon2::finalize(std::vector<uint8_t>& output)
{
    // Ensure the output vector has the correct size
    output.resize(hashLength_);

    // Initialize a block to zeros
    MemoryBlock blockhash;
    for (size_t i = 0; i < 128; ++i)
    {
        blockhash.v[i] = 0;
    }

    // XOR the last blocks of each lane into blockhash
    for (uint32_t l = 0; l < lanes_; ++l)
    {
        // Get the last block in the lane
        uint32_t lastBlockIndex = l * laneLength_ + (laneLength_ - 1);
        const MemoryBlock& lastBlock = memory_[lastBlockIndex];

        // XOR into blockhash
        for (size_t i = 0; i < 128; ++i)
        {
            blockhash.v[i] ^= lastBlock.v[i];
        }
    }

    // Serialize blockhash to bytes (little-endian)
    std::vector<uint8_t> blockhashBytes(1024); // 128 * 8 bytes
    for (size_t i = 0; i < 128; ++i)
    {
        uint64_t word = blockhash.v[i];
        for (size_t b = 0; b < 8; ++b)
        {
            blockhashBytes[i * 8 + b] = static_cast<uint8_t>(word & 0xFF);
            word >>= 8;
        }
    }

    // Compute the final hash using Blake2b with the desired output length
    Blake2b::hash(blockhashBytes.data(), blockhashBytes.size(), output.data(), hashLength_);
}

void Argon2::initializeFirstBlocks()
{
    // For each lane
    for (uint32_t l = 0; l < lanes_; ++l) {
        // For i in {0,1}
        for (uint32_t i = 0; i < 2; ++i) {
            // Create input buffer: initialHash_ || LE32(0) || LE32(lane) || LE32(i)
            std::vector<uint8_t> input(Blake2b::HASH_SIZE + 12);

            // Copy initialHash_ into input
            std::copy(initialHash_, initialHash_ + Blake2b::HASH_SIZE, input.begin());

            // Append LE32(0)
            input[Blake2b::HASH_SIZE + 0] = 0;
            input[Blake2b::HASH_SIZE + 1] = 0;
            input[Blake2b::HASH_SIZE + 2] = 0;
            input[Blake2b::HASH_SIZE + 3] = 0;

            // Append LE32(lane)
            input[Blake2b::HASH_SIZE + 4] = static_cast<uint8_t>(l & 0xFF);
            input[Blake2b::HASH_SIZE + 5] = static_cast<uint8_t>((l >> 8) & 0xFF);
            input[Blake2b::HASH_SIZE + 6] = static_cast<uint8_t>((l >> 16) & 0xFF);
            input[Blake2b::HASH_SIZE + 7] = static_cast<uint8_t>((l >> 24) & 0xFF);

            // Append LE32(i)
            input[Blake2b::HASH_SIZE + 8] = static_cast<uint8_t>(i & 0xFF);
            input[Blake2b::HASH_SIZE + 9] = static_cast<uint8_t>((i >> 8) & 0xFF);
            input[Blake2b::HASH_SIZE + 10] = static_cast<uint8_t>((i >> 16) & 0xFF);
            input[Blake2b::HASH_SIZE + 11] = static_cast<uint8_t>((i >> 24) & 0xFF);

            // Compute H' function to get a 1024-byte block
            std::vector<uint8_t> blockBytes(1024);
            uint32_t blocksNeeded = 16; // 1024 bytes / 64 bytes per Blake2b hash

            for (uint32_t j = 0; j < blocksNeeded; ++j) {
                // Input for Blake2b: input || LE32(j)
                std::vector<uint8_t> blkInput = input;
                blkInput.push_back(static_cast<uint8_t>(j & 0xFF));
                blkInput.push_back(static_cast<uint8_t>((j >> 8) & 0xFF));
                blkInput.push_back(static_cast<uint8_t>((j >> 16) & 0xFF));
                blkInput.push_back(static_cast<uint8_t>((j >> 24) & 0xFF));

                // Compute Blake2b hash
                uint8_t hash[Blake2b::HASH_SIZE];
                Blake2b::hash(blkInput.data(), blkInput.size(), hash);

                // Copy hash into blockBytes
                std::copy(hash, hash + Blake2b::HASH_SIZE, blockBytes.begin() + j * Blake2b::HASH_SIZE);
            }

            // Copy blockBytes into the memory block
            MemoryBlock& block = memory_[l * laneLength_ + i];
            for (size_t k = 0; k < 128; ++k) {
                uint64_t value = 0;
                for (size_t b = 0; b < 8; ++b) {
                    value |= static_cast<uint64_t>(blockBytes[k * 8 + b]) << (8 * b);
                }
                block.v[k] = value;
            }
        }
    }
}


void Argon2::fillSegment(uint32_t pass, uint32_t lane, uint32_t segment)
{
    uint32_t startingIndex;
    if (pass == 0 && segment == 0) {
        startingIndex = 2; // The first two blocks of each lane are already initialized
    }
    else {
        startingIndex = 0;
    }

    uint32_t segmentLength = this->segmentLength_;
    uint32_t laneLength = this->laneLength_;
    uint32_t index;

    // Initialize address generation if needed
    bool isDataIndependent = (type_ == Argon2i) || (type_ == Argon2id && pass == 0);

    if (isDataIndependent) {
        // Reset inputBlock_ and addressBlock_ for each segment
        initializeInputBlock(pass, lane, segment);
        addressBlockIndex_ = 0;
    }

    // For each index within the segment
    for (uint32_t i = startingIndex; i < segmentLength; ++i) {
        index = (segment * segmentLength_ + i) % laneLength_; // **Added modulo operation**
        uint32_t globalIndex = lane * laneLength_ + index;


        // **Corrected assertions**
        assert(globalIndex < memory_.size());
        assert(index < laneLength_);

        // Compute the pseudo-random value and reference block index
        uint64_t pseudoRandom = computePseudoRandom(pass, lane, index, i, segment);
        uint32_t refLane, refIndex;
        computeRefBlockIndices(pseudoRandom, pass, lane, index, segment, refLane, refIndex);

        // Retrieve the reference block and previous block
        uint32_t prevIndex = (globalIndex == 0) ? memory_.size() - 1 : globalIndex - 1;
        const MemoryBlock& prevBlock = memory_[prevIndex];

        uint32_t refGlobalIndex = refLane * laneLength_ + refIndex;
        // **Added assertion to ensure refGlobalIndex is within bounds**
        assert(refGlobalIndex < memory_.size());
        const MemoryBlock& refBlock = memory_[refGlobalIndex];

        // Compute the new block
        MemoryBlock& currentBlock = memory_[globalIndex];
        compressionFunction(prevBlock, refBlock, currentBlock);
    }
}


uint64_t Argon2::computePseudoRandom(uint32_t pass, uint32_t lane, uint32_t index, uint32_t positionInSegment, uint32_t segment)
{
    if (type_ == Argon2d || (type_ == Argon2id && pass != 0)) {
        // Use the previous block's first 64 bits
        uint32_t laneLength = this->laneLength_;
        uint32_t globalIndex = (lane * laneLength + index) % memoryCost_;
        const MemoryBlock& prevBlock = memory_[(globalIndex - 1 + memoryCost_) % memoryCost_];
        return prevBlock.v[0];
    }
    else {
        // Use the address generation function
        return generateAddress();
    }
}

void Argon2::computeRefBlockIndices(uint64_t pseudoRandom, uint32_t pass, uint32_t lane, uint32_t index, uint32_t segment, uint32_t& refLane, uint32_t& refIndex)
{
    uint32_t lanes = lanes_;
    uint32_t segmentLength = segmentLength_;
    uint32_t laneLength = laneLength_;
    uint32_t position = index % segmentLength_;

    uint32_t pseudoRandomLane = static_cast<uint32_t>((pseudoRandom >> 32)) % lanes;
    refLane = (pass == 0 && type_ != Argon2d) ? lane : pseudoRandomLane;

    uint64_t referenceAreaSize;
    if (pass == 0) {
        if (segment == 0) {
            // In the first segment of the first pass, reference within the same segment
            referenceAreaSize = position;
        }
        else {
            // In other segments, reference all blocks in previous segments plus the current position
            referenceAreaSize = segment * segmentLength + position;
        }
    }
    else {
        // In subsequent passes, reference within the entire lane except the current segment
        referenceAreaSize = laneLength_ - segmentLength + position;
    }

    // **Start of Change: Constrain referenceAreaSize to laneLength_**
    referenceAreaSize = std::min(referenceAreaSize, static_cast<uint64_t>(laneLength_));
    // **End of Change**

    // Ensure referenceAreaSize is not zero to prevent division by zero
    if (referenceAreaSize == 0) {
        referenceAreaSize = 1;
    }

    uint64_t relativePosition = pseudoRandom & 0xFFFFFFFF;
    refIndex = static_cast<uint32_t>((relativePosition * referenceAreaSize) >> 32);
    refIndex = static_cast<uint32_t>((referenceAreaSize - 1) - refIndex); /////// This line causes same output

    // Adjust refIndex if necessary
    if (pass == 0 && segment == 0) {
        if (refIndex == 0) {
            refIndex = 1;
        }
    }
}


void Argon2::compressionFunction(const MemoryBlock& block1, const MemoryBlock& block2, MemoryBlock& result)
{
    // Apply the G function directly to block1 and block2
    Gfunction(block1, block2, result);
}

void Argon2::Gfunction(const MemoryBlock& X, const MemoryBlock& Y, MemoryBlock& Z)
{
    MemoryBlock R;
    MemoryBlock R_original;

    // Step 1: Compute R = X XOR Y
    for (size_t i = 0; i < 128; ++i) {
        R.v[i] = X.v[i] ^ Y.v[i];
    }

    // Keep a copy of R before permutation
    for (size_t i = 0; i < 128; ++i) {
        R_original.v[i] = R.v[i];
    }

    // Step 2: Apply the P permutation function to R
    P(R);

    // Step 3: Compute Z = R XOR R_original
    for (size_t i = 0; i < 128; ++i) {
        Z.v[i] = R.v[i] ^ R_original.v[i];
    }
}

void Argon2::P(MemoryBlock& R)
{
    // Treat R as an 8x16 matrix Q
    uint64_t Q[8][16];

    // Copy R into the 8x16 matrix Q
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 16; ++j) {
            Q[i][j] = R.v[i * 16 + j];
        }
    }

    // Perform 8 rounds
    for (size_t round = 0; round < 8; ++round) {
       // Column step
        for (size_t i = 0; i < 8; ++i) {
            // Apply G_Blake to columns
            G_Blake(Q[i][0], Q[i][1], Q[i][2], Q[i][3]);
            G_Blake(Q[i][4], Q[i][5], Q[i][6], Q[i][7]);
            G_Blake(Q[i][8], Q[i][9], Q[i][10], Q[i][11]);
            G_Blake(Q[i][12], Q[i][13], Q[i][14], Q[i][15]);
        }

        // Row step
        for (size_t j = 0; j < 16; ++j) {
            // Apply G_Blake to rows
            G_Blake(Q[0][j], Q[1][j], Q[2][j], Q[3][j]);
            G_Blake(Q[4][j], Q[5][j], Q[6][j], Q[7][j]);
        }
    }

    // Copy the result back into R
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 16; ++j) {
            R.v[i * 16 + j] = Q[i][j];
        }
    }
}


void Argon2::G_Blake(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d)
{
    a = a + b;
    d ^= a;
    d = rotr64(d, 32);

    c = c + d;
    b ^= c;
    b = rotr64(b, 24);

    a = a + b;
    d ^= a;
    d = rotr64(d, 16);

    c = c + d;
    b ^= c;
    b = rotr64(b, 63);
}

uint64_t Argon2::rotr64(uint64_t x, uint64_t n)
{
    return (x >> n) | (x << (64 - n));
}

void Argon2::initializeAddressBlocks()
{
    // Initialize zeroBlock_ to zeros
    zeroBlock_ = MemoryBlock(); // Default constructor initializes all elements to zero

    // Initialize inputBlock_ to zeros
    inputBlock_ = MemoryBlock();

    // inputBlock_ initialization as per Argon2 specification
    // The initial values will be set during address generation
    // For now, we can leave inputBlock_ as zeros
}

uint64_t Argon2::generateAddress()
{
    if (addressBlockIndex_ >= 128) {
        // Generate a new address block
        // G(inputBlock_, zeroBlock_, addressBlock_)
        Gfunction(inputBlock_, zeroBlock_, addressBlock_);

        // Increment the counter in inputBlock_
        inputBlock_.v[6]++;

        // Reset the address block index
        addressBlockIndex_ = 0;
    }

    // Get the next pseudo-random value from the address block
    uint64_t pseudoRandom = addressBlock_.v[addressBlockIndex_];
    addressBlockIndex_++;

    return pseudoRandom;
}

void Argon2::initializeInputBlock(uint32_t pass, uint32_t lane, uint32_t segment)
{
    // Zero the inputBlock_
    inputBlock_ = MemoryBlock();

    // Set the parameters as per the Argon2 specification
    inputBlock_.v[0] = pass;
    inputBlock_.v[1] = lane;
    inputBlock_.v[2] = segment;
    inputBlock_.v[3] = memoryCost_;
    inputBlock_.v[4] = timeCost_;
    inputBlock_.v[5] = type_;
    inputBlock_.v[6] = 0; // Counter starting from zero
    inputBlock_.v[7] = 0; // Reserved
    // inputBlock_.v[8..127] remain zero
}
