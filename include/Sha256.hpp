#ifndef ALETHEIA_SHA256_HPP
#define ALETHEIA_SHA256_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
class Sha256 {
public:
    Sha256();
    void update(const std::uint8_t *data, std::size_t len);
    std::string hexDigest();

private:
    void transform(const std::uint8_t *block);
    std::array<std::uint32_t, 8> state_;
    std::array<std::uint8_t, 64> buffer_{};
    std::uint64_t bitLength_ = 0;
    std::size_t bufferLength_ = 0;
    bool finalized_ = false;
};

std::string sha256File(const std::filesystem::path &file);
#endif
