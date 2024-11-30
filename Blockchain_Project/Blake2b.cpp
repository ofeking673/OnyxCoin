#include "Blake2b.h"

Blake2b::Blake2b(size_t outlen, const std::vector<uint8_t>&key) 
    : outlen_(outlen)
{
    if (key.size() > 64)
    {
        throw std::invalid_argument("Key length exceeds maximum of 64 bytes");
    }

    // Initialize state with IV
    state_ = IV_;

    // Parameter block: digest length, key length, fanout, depth, etc.
    uint64_t param = 0;
    param |= (uint64_t)HASH_SIZE;      // digest length
    param |= ((uint64_t)key.size() << 8);  // key length
    param |= ((uint64_t)1 << 16);          // fanout
    param |= ((uint64_t)1 << 24);          // depth
    // The rest are zeros

    state_[0] ^= param;



    // If a key is provided, process it as the first block
    if (!key.empty())
    {
        std::vector<uint8_t> block(BLOCK_SIZE, 0);
        std::copy(key.begin(), key.end(), block.begin());
        update(block.data(), BLOCK_SIZE);
    }
}

void Blake2b::update(const uint8_t* data, size_t len)
{
    size_t offset = 0;

    while (len > 0)
    {
        size_t buffer_space = BLOCK_SIZE - buffer_.size();
        size_t to_copy = std::min(len, buffer_space);

        buffer_.insert(buffer_.end(), data + offset, data + offset + to_copy);
        offset += to_copy;
        len -= to_copy;

        if (buffer_.size() == BLOCK_SIZE)
        {
            total_bytes_ += BLOCK_SIZE;
            compress(buffer_.data(), BLOCK_SIZE, false);
            buffer_.clear();
        }
    }
}

void Blake2b::final(uint8_t* hash)
{
    // Pad the remaining data
    if (!buffer_.empty())
    {
        total_bytes_ += buffer_.size();
        buffer_.resize(BLOCK_SIZE, 0);
        compress(buffer_.data(), buffer_.size(), true);
    }
    else
    {
        // If buffer is empty, process an empty block with the finalization flag
        total_bytes_ += 0;
        std::vector<uint8_t> block(BLOCK_SIZE, 0);
        compress(block.data(), block.size(), true);
    }

    // Serialize the state to produce the final hash
    for (size_t i = 0; i < 8; ++i)
    {
        to_bytes(state_[i], hash + i * 8);
    }
}

void Blake2b::hash(const uint8_t* data, size_t len, uint8_t* hash, size_t outlen)
{
    Blake2b blake2b(outlen);
    blake2b.update(data, len);
    blake2b.final(hash);
}

std::string Blake2b::bytesToHex(const uint8_t* bytes, size_t length)
{
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    return oss.str();
}

void Blake2b::compress(const uint8_t* block, size_t block_len, bool is_last)
{
    // Convert block to 16 little-endian 64-bit words
    std::array<uint64_t, 16> m;
    for (size_t i = 0; i < 16; ++i)
    {
        m[i] = from_bytes(block + i * 8);
    }

    // Initialize work variables with state
    std::array<uint64_t, 16> v;
    for (size_t i = 0; i < 8; ++i)
    {
        v[i] = state_[i];
        v[i + 8] = IV_[i];
    }

    // XOR in the total length and finalization flags
    uint64_t t0 = total_bytes_;
    uint64_t t1 = 0; // Assuming total_bytes_ fits in 64 bits
    v[12] ^= t0;
    v[13] ^= t1;

    uint64_t f0 = is_last ? ~0ULL : 0;
    v[14] ^= f0;
    v[15] ^= 0;

    // 12 rounds of mixing
    for (size_t round = 0; round < 12; ++round)
    {
        const auto& s = sigma_[round];
        GblakeFunction(v, 0, 4, 8, 12, m[s[0]], m[s[1]]);
        GblakeFunction(v, 1, 5, 9, 13, m[s[2]], m[s[3]]);
        GblakeFunction(v, 2, 6, 10, 14, m[s[4]], m[s[5]]);
        GblakeFunction(v, 3, 7, 11, 15, m[s[6]], m[s[7]]);
        GblakeFunction(v, 0, 5, 10, 15, m[s[8]], m[s[9]]);
        GblakeFunction(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        GblakeFunction(v, 2, 7, 8, 13, m[s[12]], m[s[13]]);
        GblakeFunction(v, 3, 4, 9, 14, m[s[14]], m[s[15]]);
    }

    // Update the state
    for (size_t i = 0; i < 8; ++i)
    {
        state_[i] ^= v[i] ^ v[i + 8];
    }
}

void Blake2b::GblakeFunction(std::array<uint64_t, 16>& v, size_t a, size_t b, size_t c, size_t d, uint64_t x, uint64_t y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = rotr64(v[d] ^ v[a], 32);
    v[c] = v[c] + v[d];
    v[b] = rotr64(v[b] ^ v[c], 24);
    v[a] = v[a] + v[b] + y;
    v[d] = rotr64(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];
    v[b] = rotr64(v[b] ^ v[c], 63);
}

uint64_t Blake2b::rotr64(uint64_t x, size_t n)
{
    return (x >> n) | (x << (64 - n));
}

uint64_t Blake2b::from_bytes(const uint8_t* bytes)
{
    uint64_t value = 0;
    for (size_t i = 0; i < 8; ++i)
    {
        value |= static_cast<uint64_t>(bytes[i]) << (8 * i);
    }
    return value;
}

void Blake2b::to_bytes(uint64_t value, uint8_t* bytes)
{
    for (size_t i = 0; i < 8; ++i)
    {
        bytes[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
    }
}
