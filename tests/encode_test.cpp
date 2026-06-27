#include "StdAfx.h"
#include "encode.h"
#include <gtest/gtest.h>
#include <cstring>

TEST(EncodeTest, HexEncode_AsciiCharacters)
{
    EXPECT_STREQ("414243", hex_encode("ABC"));
    EXPECT_STREQ("",       hex_encode(""));
}

TEST(EncodeTest, HexDecode_ReversesEncode)
{
    EXPECT_STREQ("ABC", hex_decode("414243"));
    EXPECT_STREQ("",    hex_decode(""));
}

TEST(EncodeTest, HexEncode_RoundTrip)
{
    const char * original = "Crimson Editor 3.80";
    const char * encoded = hex_encode(original);
    EXPECT_STRNE("", encoded);
    EXPECT_STREQ(original, hex_decode(encoded));
}

TEST(EncodeTest, MapEncode_RoundTrip)
{
    const char * original = "round-trip test 123";
    const char * encoded = map_encode(original);
    EXPECT_STREQ(original, map_decode(encoded));
}

TEST(EncodeTest, MapEncode_ChangesContent)
{
    // map_encode is meant to obfuscate (not identity)
    const char * original = "password";
    const char * encoded = map_encode(original);
    EXPECT_STRNE(original, encoded);
}
