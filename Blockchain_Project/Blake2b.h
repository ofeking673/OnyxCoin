#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace Crypto
{
    class Blake2b 
    {
    public:
        static constexpr size_t HASH_SIZE = 64;       // 512-bit hash
        static constexpr size_t BLOCK_SIZE = 128;     // 1024-bit blocks

        /// <summary>
        /// Constructs a Blake2b instance with an optional key for authentication.
        /// </summary>
        /// <param name="key">Optional secret key for keyed hashing (authentication).</param>
        explicit Blake2b(const std::vector<uint8_t>& key = {});

        /// <summary>
        /// Updates the hash with additional data.
        /// </summary>
        /// <param name="data">Pointer to the data to hash.</param>
        /// <param name="len">Length of the data in bytes.</param>
        void update(const uint8_t* data, size_t len);

        /// <summary>
        /// Finalizes the hash and produces the output.
        /// </summary>
        /// <param name="hash">Output buffer to store the hash. Must be at least HASH_SIZE bytes.</param>
        void final(uint8_t* hash);

        /**
         * @brief 
         *
         * @param data 
         * @param len 
         * @param hash 
         */

        /// <summary>
        /// Computes the Blake2b hash of the given data in one step.
        /// </summary>
        /// <param name="data">Pointer to the data to hash.</param>
        /// <param name="len">Length of the data in bytes.</param>
        /// <param name="hash">Output buffer to store the hash. Must be at least HASH_SIZE bytes.</param>
        static void hash(const uint8_t* data, size_t len, uint8_t* hash);

    private:
        // Internal state
        std::array<uint64_t, 8> state_;
        uint64_t total_length_ = 0;
        std::vector<uint8_t> buffer_;

        // Blake2b constants
        static constexpr std::array<std::array<size_t, 16>, 12> sigma_ = { {
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 },
            {14,10, 4, 8, 9,15,13, 6, 1,12, 0, 2,11, 7, 5, 3 },
            {11, 8,12, 0, 5, 2,15,13,10,14, 3, 6, 7, 1, 9, 4 },
            {7, 9, 3, 1,13,12,11,14, 2, 6, 5,10, 4, 0,15, 8 },
            {9, 0, 5, 7, 2, 4,10,15,14, 1,11,12, 6, 8, 3,13 },
            {2,12, 6,10, 0,11, 8, 3, 4,13, 7, 5,15,14, 1, 9 },
            {12, 5, 1,15,14,13, 4,10, 0, 7, 6, 3, 9, 2, 8,11 },
            {13,11, 7,14,12, 1, 3, 9, 5, 0,15, 4, 8, 6, 2,10 },
            {6,15,14, 9,11, 3, 0, 8,12, 2,13, 7, 1, 4,10, 5 },
            {10, 2, 8, 4, 7, 6, 1, 5,15,11, 9,14, 3,12,13, 0 },
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 },
            {14,10, 4, 8, 9,15,13, 6, 1,12, 0, 2,11, 7, 5, 3 }
        } };

        /// <summary>
        /// Compress function as defined in the Blake2b specification.
        /// </summary>
        /// <param name="block">Pointer to the current block of data.</param>
        /// <param name="block_len">Length of the block in bytes.</param>
        /// <param name="is_last">Indicates if this is the final block.</param>
        void compress(const uint8_t* block, size_t block_len, bool is_last);

        /**
         * @brief 
         *
         * @param v 
         * @param a 
         * @param b 
         * @param c 
         * @param d 
         * @param x 
         * @param y 
         */

        /// <summary>
        /// G mixing function as defined in the Blake2b specification.
        /// </summary>
        /// <param name="v">Work variables.</param>
        /// <param name="a">Index of the first variable.</param>
        /// <param name="b">Index of the second variable.</param>
        /// <param name="c">Index of the third variable.</param>
        /// <param name="d">Index of the fourth variable.</param>
        /// <param name="x">Message word x.</param>
        /// <param name="y">Message word y.</param>
        void G(std::array<uint64_t, 16>& v, size_t a, size_t b, size_t c, size_t d, uint64_t x, uint64_t y);

        /// <summary>
        /// Performs a right rotation on a 64-bit integer.
        /// </summary>
        /// <param name="x">The integer to rotate.</param>
        /// <param name="n">Number of bits to rotate.</param>
        /// <returns>uint64_t The rotated integer.</returns>
        static uint64_t rotr64(uint64_t x, size_t n);

        /// <summary>
        /// Converts 8 bytes to a 64-bit little-endian integer.
        /// </summary>
        /// <param name="bytes">Pointer to the 8 bytes.</param>
        /// <returns>uint64_t The resulting 64-bit integer.</returns>
        static uint64_t from_bytes(const uint8_t* bytes);

        /// <summary>
        /// Converts a 64-bit integer to 8 bytes in little-endian order.
        /// </summary>
        /// <param name="value">The 64-bit integer.</param>
        /// <param name="bytes">Pointer to the output buffer (must have at least 8 bytes).</param>
        static void to_bytes(uint64_t value, uint8_t* bytes);
    };

} // namespace Crypto


