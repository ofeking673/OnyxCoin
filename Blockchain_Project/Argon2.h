#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Enum to specify the type of Argon2 algorithm
enum class Argon2Type {
    Argon2d,   // Data-dependent memory access
    Argon2i,   // Data-independent memory access
    Argon2id   // Hybrid of Argon2i and Argon2d
};

class Argon2
{
public:
    /// <summary>
    /// Constructor to initialize Argon2 with default or custom parameters.
    /// </summary>
    /// <param name="timeCost">Number of iterations (time cost).</param>
    /// <param name="memoryCost">Memory usage in kilobytes (memory cost).</param>
    /// <param name="parallelism">Degree of parallelism (number of threads).</param>
    /// <param name="hashLength">Desired length of the output hash in bytes.</param>
    /// <param name="type">Type of Argon2 algorithm to use.</param>
    Argon2(
        uint32_t timeCost = 2,
        uint32_t memoryCost = 65536, // Default: 64 MB
        uint32_t parallelism = 1,
        uint32_t hashLength = 32,
        Argon2Type type = Argon2Type::Argon2id
    );

    // Destructor
    ~Argon2();

    // Delete copy constructor and assignment operator to prevent copying
    Argon2(const Argon2&) = delete;
    Argon2& operator=(const Argon2&) = delete;

    // Sets the time cost parameter.
    void setTimeCost(uint32_t timeCost);

    // Sets the memory cost parameter (Memory usage in kilobytes).
    void setMemoryCost(uint32_t memoryCost);

    // Sets the degree of parallelism (Number of threads).
    void setParallelism(uint32_t parallelism);

    // Sets the desired hash length.
    void setHashLength(uint32_t hashLength);

    // Sets the Argon2 algorithm type (Argon2d, Argon2i, Argon2id).
    void setType(Argon2Type type);

    /// <summary>
    /// Hashes the provided password with the given salt.
    /// </summary>
    /// <param name="password">The input password to hash</param>
    /// <param name="salt">A unique salt value.</param>
    /// <param name="outHash">A vector to store the resulting hash.</param>
    /// <returns>true if hashing is successful, false otherwise.</returns>
    bool hashPassword(
        const std::string& password,
        const std::string& salt,
        std::vector<uint8_t>& outHash
    );

    /// <summary>
    /// Verifies a password against a given hash and salt.
    /// </summary>
    /// <param name="password">The input password to verify.</param>
    /// <param name="salt">The salt used during hashing.</param>
    /// <param name="hash">The hash to compare against.</param>
    /// <returns>true if the password is correct, false otherwise.</returns>
    bool verifyHash(
        const std::string& password,
        const std::string& salt,
        const std::vector<uint8_t>& hash
    );

private:
    // Argon2 configuration parameters
    uint32_t m_timeCost;
    uint32_t m_memoryCost;
    uint32_t m_parallelism;
    uint32_t m_hashLength;
    Argon2Type m_type;


    /// <summary>
    /// Initializes internal structures required for Argon2.
    /// </summary>
    /// <returns>true if initialization is successful, false otherwise.</returns>
    bool initialize();

    /// <summary>
    /// Allocates and initializes the memory blocks used by Argon2.
    /// </summary>
    /// <returns>true if memory is successfully allocated and initialized, false otherwise.</returns>
    bool allocateMemory();

    /// <summary>
    /// Executes the core Argon2 hashing algorithm.
    /// </summary>
    /// <param name="password">The input password.</param>
    /// <param name="salt">The salt value.</param>
    /// <param name="outHash">The output hash.</param>
    /// <returns>true if hashing is successful, false otherwise.</returns>
    bool executeHash(
        const std::string& password,
        const std::string& salt,
        std::vector<uint8_t>& outHash
    );
};
