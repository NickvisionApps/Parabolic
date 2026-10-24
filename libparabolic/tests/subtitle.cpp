#include <gtest/gtest.h>
#include "subtitle.h"

using namespace parabolic;

TEST(SubtitleTest, compare)
{
    SubtitleLanguage lang1{ "en", false };
    ASSERT_EQ(lang1.language(), "en");
    ASSERT_EQ(lang1.is_auto_generated(), false);
    SubtitleLanguage lang2{ "en", true };
    ASSERT_EQ(lang2.language(), "en");
    ASSERT_EQ(lang2.is_auto_generated(), true);
    SubtitleLanguage lang3{ "fr", false };
    ASSERT_EQ(lang1, lang2);
    ASSERT_NE(lang1, lang3);
    ASSERT_TRUE(lang1 < lang3);
}