#include <gtest/gtest.h>

#include <core/AsciiToLower.h>

namespace core {

TEST(TestCore, TestAsciiToLowerFoldsAsciiUppercase)
{
    EXPECT_EQ(L'a', AsciiToLower(L'A'));
    EXPECT_EQ(L'z', AsciiToLower(L'Z'));
    EXPECT_EQ(L'm', AsciiToLower(L'M'));
}

TEST(TestCore, TestAsciiToLowerLeavesEverythingElseAlone)
{
    EXPECT_EQ(L'a', AsciiToLower(L'a'));
    EXPECT_EQ(L'/', AsciiToLower(L'/'));
    EXPECT_EQ(L'7', AsciiToLower(L'7'));
    // Just outside the A-Z range on either side.
    EXPECT_EQ(L'@', AsciiToLower(L'@'));
    EXPECT_EQ(L'[', AsciiToLower(L'['));
}

// The reason this exists instead of std::towlower: folding must not
// depend on the active locale. U+0130 (LATIN CAPITAL LETTER I WITH DOT
// ABOVE) is the classic case where a locale-aware fold diverges; here it
// is left untouched because it is not ASCII. Written as numeric code
// points so the test does not depend on the source file's encoding or
// the compiler's active code page.
TEST(TestCore, TestAsciiToLowerIgnoresNonAsciiRegardlessOfLocale)
{
    constexpr wchar_t kCapitalIWithDotAbove = static_cast<wchar_t>(0x0130);
    constexpr wchar_t kCapitalAWithDiaeresis = static_cast<wchar_t>(0x00C4);

    EXPECT_EQ(kCapitalIWithDotAbove, AsciiToLower(kCapitalIWithDotAbove));
    EXPECT_EQ(kCapitalAWithDiaeresis, AsciiToLower(kCapitalAWithDiaeresis));
}

TEST(TestCore, TestAsciiToLowerIsUsableAtCompileTime)
{
    static_assert(AsciiToLower(L'A') == L'a');
    static_assert(AsciiToLower(L'/') == L'/');
    SUCCEED();
}

}
