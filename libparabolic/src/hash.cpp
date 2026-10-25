#include "hash.h"
#include <array>
#include <format>
#include <fstream>
#include <memory>
#include <openssl/evp.h>

namespace
{
    std::string digest(const EVP_MD* algorithm, const std::filesystem::path& file_path)
    {
        std::ifstream file{ file_path, std::ios::binary };
        if (!file)
        {
            return {};
        }
        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> context{ EVP_MD_CTX_new(), EVP_MD_CTX_free };
        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
        std::array<char, 8192> buffer{};
        unsigned int length{ 0 };
        if (EVP_DigestInit_ex(context.get(), algorithm, nullptr) == 0)
        {
            return {};
        }
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0)
        {
            if (EVP_DigestUpdate(context.get(), buffer.data(), file.gcount()) == 0)
            {
                return {};
            }
        }
        if (!file.eof() || EVP_DigestFinal_ex(context.get(), hash.data(), &length) == 0)
        {
            return {};
        }
        std::string res;
        for (unsigned int i = 0; i < length; i++)
        {
            res += std::format("{:02x}", hash.at(i));
        }
        return res;
    }

    std::string digest(const EVP_MD* algorithm, std::string_view data)
    {
        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> context{ EVP_MD_CTX_new(), EVP_MD_CTX_free };
        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
        unsigned int length{ 0 };
        if (EVP_DigestInit_ex(context.get(), algorithm, nullptr) == 0 ||
            EVP_DigestUpdate(context.get(), data.data(), data.size()) == 0 ||
            EVP_DigestFinal_ex(context.get(), hash.data(), &length) == 0)
        {
            return {};
        }
        std::string res;
        for (unsigned int i = 0; i < length; i++)
        {
            res += std::format("{:02x}", hash.at(i));
        }
        return res;
    }
}

namespace parabolic::hash
{
    std::string sha256_from_file(const std::filesystem::path& file_path)
    {
        return digest(EVP_sha256(), file_path);
    }

    std::string sha256_from_string(std::string_view data)
    {
        return digest(EVP_sha256(), data);
    }

    std::string sha512_from_file(const std::filesystem::path& file_path)
    {
        return digest(EVP_sha512(), file_path);
    }

    std::string sha512_from_string(std::string_view data)
    {
        return digest(EVP_sha512(), data);
    }
}
