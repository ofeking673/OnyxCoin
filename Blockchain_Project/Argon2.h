#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Argon2
{
public:
    // Constructor to initialize Argon2 parameters
    Argon2(size_t memory_size, size_t iterations, size_t parallelism, size_t hash_length,
        const std::vector<uint8_t>& salt, const std::vector<uint8_t>& secret = {},
        const std::vector<uint8_t>& associated_data = {});

    // Function to compute the Argon2 hash
    std::vector<uint8_t> hash();

    // Setter functions to modify parameters if needed
    void setMemorySize(size_t memory_size);
    void setIterations(size_t iterations);
    void setParallelism(size_t parallelism);
    void setHashLength(size_t hash_length);
    void setSalt(const std::vector<uint8_t>& salt);
    void setSecret(const std::vector<uint8_t>& secret);
    void setAssociatedData(const std::vector<uint8_t>& associated_data);

private:
    // Internal functions for Argon2 algorithm steps
    void initialize();
    void fillMemory();
    void finalizeHash();

    // Compression function (e.g., Blake2b)
    std::vector<uint8_t> compressionFunction(const std::vector<uint8_t>& block1,
        const std::vector<uint8_t>& block2);

    // Utility functions
    std::vector<uint8_t> generatePseudoRandomBytes(size_t length);
    void mixBlock(size_t current_block, size_t reference_block);
    void optimizeMemory();

    // Member variables for parameters
    size_t memory_size_;         // Memory size in kilobytes
    size_t iterations_;          // Number of iterations (time cost)
    size_t parallelism_;         // Degree of parallelism (threads)
    size_t hash_length_;         // Desired length of the hash output

    // Inputs
    std::vector<uint8_t> password_;           // User's password
    std::vector<uint8_t> salt_;               // Salt
    std::vector<uint8_t> secret_;             // Optional secret
    std::vector<uint8_t> associated_data_;    // Optional associated data

    // Memory blocks
    std::vector<std::vector<uint8_t>> memory_; // Memory matrix

    // Final hash
    std::vector<uint8_t> final_hash_;         // Output hash
};