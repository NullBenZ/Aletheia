#ifndef ALETHEIA_SHA256_HPP
#define ALETHEIA_SHA256_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
// Computes SHA-256 incrementally using the FIPS 180-4 compression steps.
class Sha256 {
public:
    Sha256(); // Initializes the standard SHA-256 state.
    void update(const std::uint8_t* data, std::size_t len); // Adds bytes to the running digest.
    std::string hexDigest(); // Finalizes and returns lowercase hexadecimal output.
private:
    void transform(const std::uint8_t* block); // Processes one 64-byte block.
    std::array<std::uint32_t, 8> state_;
    std::array<std::uint8_t, 64> buffer_{};
    std::uint64_t bitLength_ = 0;
    std::size_t bufferLength_ = 0;
    bool finalized_ = false;
    std::string digest_;
};
#endif
