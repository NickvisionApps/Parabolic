#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <string>
#include "hash.h"

#define SHA256_EMPTY "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
#define SHA256_ABC "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
#define SHA256_HELLO "dffd6021bb2bd5b0af676290809ec3a53191dd81c7f70a4b28688a362182986f"
#define SHA256_FOX "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"
#define SHA256_MILLION_A "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"
#define SHA512_EMPTY "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"
#define SHA512_ABC "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"
#define SHA512_HELLO "374d794a95cdcfd8b35993185fef9ba368f160d8daf432d08ba9f1ed1e5abe6cc69291e0fa2fe0006a52570ef18c19def4e617c33ce52ef0a6e5fbe318cb0387"
#define SHA512_MILLION_A "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b"

using namespace parabolic::hash;

namespace
{
    class HashFile
    {
    public:
        HashFile(const std::string& content)
        {
            static constexpr std::string_view charset{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
            thread_local std::mt19937_64 random_device{ std::random_device{}() };
            std::uniform_int_distribution<std::size_t> random{ 0, charset.size() - 1 };
            std::string filename;
            filename.reserve(16);
            for (std::size_t i{ 0 }; i < 16; i++)
            {
                filename.push_back(charset[random(random_device)]);
            }
            m_path = std::filesystem::temp_directory_path() / (filename + ".bin");
            std::ofstream file{ m_path, std::ios::binary };
            file << content;
        }

        ~HashFile()
        {
            std::error_code ec;
            std::filesystem::remove(m_path, ec);
        }

        const std::filesystem::path& path() const
        {
            return m_path;
        }

    private:
        std::filesystem::path m_path;
    };
}

TEST(HashTest, sha256_string_empty)
{
    ASSERT_EQ(SHA256_EMPTY, sha256_from_string(""));
}

TEST(HashTest, sha256_string_abc)
{
    ASSERT_EQ(SHA256_ABC, sha256_from_string("abc"));
}

TEST(HashTest, sha256_string_1)
{
    ASSERT_EQ(SHA256_HELLO, sha256_from_string("Hello, World!"));
}

TEST(HashTest, sha256_string_pangram)
{
    ASSERT_EQ(SHA256_FOX, sha256_from_string("The quick brown fox jumps over the lazy dog"));
}

TEST(HashTest, sha256_string_million_a)
{
    ASSERT_EQ(SHA256_MILLION_A, sha256_from_string(std::string(1'000'000, 'a')));
}

TEST(HashTest, sha256_null)
{
    std::string null("a\0b", 3);
    ASSERT_NE(sha256_from_string("a"), sha256_from_string(null));
    ASSERT_NE(sha256_from_string("ab"), sha256_from_string(null));
    ASSERT_EQ(sha256_from_string(std::string_view{ "a\0b", 3 }), sha256_from_string(null));
}

TEST(HashTest, sha256_deterministic)
{
    ASSERT_EQ(sha256_from_string("same"), sha256_from_string("same"));
    ASSERT_NE(sha256_from_string("Same"), sha256_from_string("same"));
}

TEST(HashTest, sha256_file_hello)
{
    HashFile file{ "Hello, World!" };
    ASSERT_EQ(SHA256_HELLO, sha256_from_file(file.path()));
}

TEST(HashTest, sha512_string_empty)
{
    ASSERT_EQ(SHA512_EMPTY, sha512_from_string(""));
}

TEST(HashTest, sha512_string_abc)
{
    ASSERT_EQ(SHA512_ABC, sha512_from_string("abc"));
}

TEST(HashTest, sha512_string_1)
{
    ASSERT_EQ(SHA512_HELLO, sha512_from_string("Hello, World!"));
}

TEST(HashTest, sha512_string_million_a)
{
    ASSERT_EQ(SHA512_MILLION_A, sha512_from_string(std::string(1'000'000, 'a')));
}

TEST(HashTest, sha512_null)
{
    std::string null("a\0b", 3);
    ASSERT_NE(sha512_from_string("a"), sha512_from_string(null));
    ASSERT_NE(sha512_from_string("ab"), sha512_from_string(null));
    ASSERT_EQ(sha512_from_string(std::string_view{ "a\0b", 3 }), sha512_from_string(null));
}

TEST(HashTest, sha512_deterministic)
{
    ASSERT_EQ(sha512_from_string("same"), sha512_from_string("same"));
    ASSERT_NE(sha512_from_string("Same"), sha512_from_string("same"));
}

TEST(HashTest, sha512_file_abc)
{
    HashFile file{ "abc" };
    ASSERT_EQ(SHA512_ABC, sha512_from_file(file.path()));
}
