#pragma once
#include <cassert>
#include <algorithm> 
#include "Blake2b.h"

class Argon2
{
public:
    // Argon2 Type Enumeration
    enum Type
    {
        Argon2d = 0,
        Argon2i = 1,
        Argon2id = 2
    };

    /**
     * @brief Constructor with custom parameters for key derivation.
     *        Initializes parameters suitable for AES-256-CBC when no arguments are provided.
     * @param type Argon2 variant (Argon2d, Argon2i, Argon2id)
     * @param timeCost Number of iterations (default: 3)
     * @param memoryCost Memory usage in kibibytes (default: 8192)
     * @param parallelism Degree of parallelism (number of threads) (default: 1)
     * @param hashLength Length of the derived key in bytes (default: 32 for AES-256)
     */
    Argon2(Type type = Argon2id, uint32_t timeCost = 3, uint32_t memoryCost = 8192, uint32_t parallelism = 1, uint32_t hashLength = 32);

    /**
     * @brief Derives a cryptographic key from the given password and salt.
     * @param password The input password.
     * @param salt The cryptographic salt.
     * @return A vector containing the derived key.
     */
    std::vector<uint8_t> deriveKey(const std::string& password, const std::vector<uint8_t>& salt);

private:
    // Argon2 Parameters
    Type type_;
    uint32_t timeCost_;      // Number of iterations
    uint32_t memoryCost_;    // Memory usage in kibibytes
    uint32_t parallelism_;   // Degree of parallelism
    uint32_t hashLength_;    // Length of the derived key in bytes

    // Memory Block Structure
    struct MemoryBlock
    {
        uint64_t v[128];
    };

    // Memory Segments
    std::vector<MemoryBlock> memory_;

    // Instance Variables
    uint32_t lanes_;
    uint32_t segmentLength_;
    uint32_t laneLength_;

    // Pre-hashing digest
    uint8_t initialHash_[Blake2b::HASH_SIZE];

    // Internal Functions
    void initialize(const std::string& password, const std::vector<uint8_t>& salt);
    void fillMemoryBlocks();
    void finalize(std::vector<uint8_t>& output);



    // FillMemoryBlocks helper functions


    MemoryBlock zeroBlock_;
    MemoryBlock inputBlock_;
    MemoryBlock addressBlock_;
    size_t addressBlockIndex_; // Index within addressBlock_

    void fillSegment(uint32_t pass, uint32_t lane, uint32_t segment);
    uint64_t computePseudoRandom(uint32_t pass, uint32_t lane, uint32_t index, uint32_t positionInSegment, uint32_t segment);
    void computeRefBlockIndices(uint64_t pseudoRandom, uint32_t pass, uint32_t lane, uint32_t index, uint32_t segment, uint32_t& refLane, uint32_t& refIndex);
    void compressionFunction(const MemoryBlock& block1, const MemoryBlock& block2, MemoryBlock& result);

    // G function as per Argon2 specification
    void Gfunction(const MemoryBlock& blockX, const MemoryBlock& blockY, MemoryBlock& blockZ); // Says error here

    // Permutation function P
    void P(MemoryBlock& R);

    // Blake2b G function used in Argon2
    void G_Blake(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d);

    // Rotation function
    static uint64_t rotr64(uint64_t x, uint64_t n);

    void initializeAddressBlocks();
    uint64_t generateAddress();
    void initializeInputBlock(uint32_t pass, uint32_t lane, uint32_t segment);


};
