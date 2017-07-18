#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-MEME");
    BOOST_CHECK(GetBoolArg("-MEME"));
    BOOST_CHECK(GetBoolArg("-MEME", false));
    BOOST_CHECK(GetBoolArg("-MEME", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-MEMEo"));
    BOOST_CHECK(!GetBoolArg("-MEMEo", false));
    BOOST_CHECK(GetBoolArg("-MEMEo", true));

    ResetArgs("-MEME=0");
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", false));
    BOOST_CHECK(!GetBoolArg("-MEME", true));

    ResetArgs("-MEME=1");
    BOOST_CHECK(GetBoolArg("-MEME"));
    BOOST_CHECK(GetBoolArg("-MEME", false));
    BOOST_CHECK(GetBoolArg("-MEME", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noMEME");
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", false));
    BOOST_CHECK(!GetBoolArg("-MEME", true));

    ResetArgs("-noMEME=1");
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", false));
    BOOST_CHECK(!GetBoolArg("-MEME", true));

    ResetArgs("-MEME -noMEME");  // -MEME should win
    BOOST_CHECK(GetBoolArg("-MEME"));
    BOOST_CHECK(GetBoolArg("-MEME", false));
    BOOST_CHECK(GetBoolArg("-MEME", true));

    ResetArgs("-MEME=1 -noMEME=1");  // -MEME should win
    BOOST_CHECK(GetBoolArg("-MEME"));
    BOOST_CHECK(GetBoolArg("-MEME", false));
    BOOST_CHECK(GetBoolArg("-MEME", true));

    ResetArgs("-MEME=0 -noMEME=0");  // -MEME should win
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", false));
    BOOST_CHECK(!GetBoolArg("-MEME", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--MEME=1");
    BOOST_CHECK(GetBoolArg("-MEME"));
    BOOST_CHECK(GetBoolArg("-MEME", false));
    BOOST_CHECK(GetBoolArg("-MEME", true));

    ResetArgs("--noMEME=1");
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", false));
    BOOST_CHECK(!GetBoolArg("-MEME", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MEME", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MEME", "eleven"), "eleven");

    ResetArgs("-MEME -bar");
    BOOST_CHECK_EQUAL(GetArg("-MEME", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MEME", "eleven"), "");

    ResetArgs("-MEME=");
    BOOST_CHECK_EQUAL(GetArg("-MEME", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MEME", "eleven"), "");

    ResetArgs("-MEME=11");
    BOOST_CHECK_EQUAL(GetArg("-MEME", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-MEME", "eleven"), "11");

    ResetArgs("-MEME=eleven");
    BOOST_CHECK_EQUAL(GetArg("-MEME", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-MEME", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MEME", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-MEME", 0), 0);

    ResetArgs("-MEME -bar");
    BOOST_CHECK_EQUAL(GetArg("-MEME", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-MEME=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-MEME", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-MEME=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-MEME", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--MEME");
    BOOST_CHECK_EQUAL(GetBoolArg("-MEME"), true);

    ResetArgs("--MEME=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-MEME", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noMEME");
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", true));
    BOOST_CHECK(!GetBoolArg("-MEME", false));

    ResetArgs("-noMEME=1");
    BOOST_CHECK(!GetBoolArg("-MEME"));
    BOOST_CHECK(!GetBoolArg("-MEME", true));
    BOOST_CHECK(!GetBoolArg("-MEME", false));

    ResetArgs("-noMEME=0");
    BOOST_CHECK(GetBoolArg("-MEME"));
    BOOST_CHECK(GetBoolArg("-MEME", true));
    BOOST_CHECK(GetBoolArg("-MEME", false));

    ResetArgs("-MEME --noMEME");
    BOOST_CHECK(GetBoolArg("-MEME"));

    ResetArgs("-noMEME -MEME"); // MEME always wins:
    BOOST_CHECK(GetBoolArg("-MEME"));
}

BOOST_AUTO_TEST_SUITE_END()
