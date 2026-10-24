#include <clocale>
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>
#include "string_manip.h"

using namespace parabolic::string_manip;

namespace
{
    class Token
    {
    public:
        Token(std::string value) :
            m_value{ std::move(value) }
        {
        }

        const std::string& value() const
        {
            return m_value;
        }

    private:
        std::string m_value;
    };
}

TEST(StringManipTest, lower_mixed)
{
    ASSERT_EQ("hello, world! 123", lower("Hello, World! 123"));
}

TEST(StringManipTest, lower_upper)
{
    ASSERT_EQ("abcxyz", lower("ABCXYZ"));
}

TEST(StringManipTest, lower_already_lower)
{
    ASSERT_EQ("abc", lower("abc"));
}

TEST(StringManipTest, lower_empty)
{
    ASSERT_EQ("", lower(""));
}

TEST(StringManipTest, lower_high_bytes_unchanged)
{
    ASSERT_EQ("\xC3\x84", lower("\xC3\x84"));
}

TEST(StringManipTest, trim_both_sides)
{
    ASSERT_EQ("abc", trim("  abc  "));
}

TEST(StringManipTest, trim_mixed_whitespace)
{
    ASSERT_EQ("abc", trim("\t\n abc \r\n"));
}

TEST(StringManipTest, trim_left_only)
{
    ASSERT_EQ("abc", trim("   abc"));
}

TEST(StringManipTest, trim_right_only)
{
    ASSERT_EQ("abc", trim("abc   "));
}

TEST(StringManipTest, trim_inner_whitespace_kept)
{
    ASSERT_EQ("a  b", trim("  a  b  "));
}

TEST(StringManipTest, trim_nothing_to_trim)
{
    ASSERT_EQ("abc", trim("abc"));
}

TEST(StringManipTest, trim_all_whitespace)
{
    ASSERT_EQ("", trim(" \t\r\n "));
}

TEST(StringManipTest, trim_empty)
{
    ASSERT_EQ("", trim(""));
}

TEST(StringManipTest, split_char)
{
    std::vector<std::string> expected{ "a", "b", "c" };
    ASSERT_EQ(expected, split("a,b,c", ','));
}

TEST(StringManipTest, split_string)
{
    std::vector<std::string> expected{ "a", "b", "c" };
    ASSERT_EQ(expected, split("a::b::c", "::"));
}

TEST(StringManipTest, split_partial_delimiter)
{
    std::vector<std::string> expected{ "a:b", "c" };
    ASSERT_EQ(expected, split("a:b::c", "::"));
}

TEST(StringManipTest, split_no_delimiter)
{
    std::vector<std::string> expected{ "abc" };
    ASSERT_EQ(expected, split("abc", ','));
}

TEST(StringManipTest, split_empty_string)
{
    std::vector<std::string> expected{ "" };
    ASSERT_EQ(expected, split("", ','));
    ASSERT_TRUE(split("", ',', false).empty());
}

TEST(StringManipTest, split_include_empty)
{
    std::vector<std::string> expected{ "", "a", "", "b", "" };
    ASSERT_EQ(expected, split(",a,,b,", ','));
}

TEST(StringManipTest, split_exclude_empty)
{
    std::vector<std::string> expected{ "a", "b" };
    ASSERT_EQ(expected, split(",a,,b,", ',', false));
}

TEST(StringManipTest, split_exclude_whitespace_only)
{
    std::vector<std::string> expected{ "a", "b" };
    ASSERT_EQ(expected, split("a, \t,b", ',', false));
}

TEST(StringManipTest, split_tokens_not_trimmed)
{
    std::vector<std::string> expected{ " a ", " b " };
    ASSERT_EQ(expected, split(" a , b ", ',', false));
}

TEST(StringManipTest, split_custom_type)
{
    std::vector<Token> tokens{ split<Token>("a,b", ',') };
    ASSERT_EQ(2u, tokens.size());
    ASSERT_EQ("a", tokens[0].value());
    ASSERT_EQ("b", tokens[1].value());
}

TEST(StringManipTest, wstr_ascii)
{
    ASSERT_EQ(L"Hello, World!", wstr("Hello, World!"));
}

TEST(StringManipTest, wstr_empty)
{
    ASSERT_EQ(L"", wstr(""));
}

TEST(StringManipTest, str_ascii)
{
    ASSERT_EQ("Hello, World!", str(L"Hello, World!"));
}

TEST(StringManipTest, str_empty)
{
    ASSERT_EQ("", str(L""));
}

TEST(StringManipTest, utf8_conversion)
{
#ifndef _WIN32
    if (!std::setlocale(LC_ALL, "C.UTF-8") && !std::setlocale(LC_ALL, "en_US.UTF-8"))
    {
        GTEST_SKIP() << "No UTF-8 locale available";
    }
#endif
    std::wstring wide{ wstr("\xC3\xA9") };
    std::string narrow{ str(L"\u00E9") };
#ifndef _WIN32
    std::setlocale(LC_ALL, "C");
#endif
    ASSERT_EQ(L"\u00E9", wide);
    ASSERT_EQ("\xC3\xA9", narrow);
}