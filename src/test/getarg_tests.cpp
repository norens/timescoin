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
    ResetArgs("-time");
    BOOST_CHECK(GetBoolArg("-time"));
    BOOST_CHECK(GetBoolArg("-time", false));
    BOOST_CHECK(GetBoolArg("-time", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-timeo"));
    BOOST_CHECK(!GetBoolArg("-timeo", false));
    BOOST_CHECK(GetBoolArg("-timeo", true));

    ResetArgs("-time=0");
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", false));
    BOOST_CHECK(!GetBoolArg("-time", true));

    ResetArgs("-time=1");
    BOOST_CHECK(GetBoolArg("-time"));
    BOOST_CHECK(GetBoolArg("-time", false));
    BOOST_CHECK(GetBoolArg("-time", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-notime");
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", false));
    BOOST_CHECK(!GetBoolArg("-time", true));

    ResetArgs("-notime=1");
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", false));
    BOOST_CHECK(!GetBoolArg("-time", true));

    ResetArgs("-time -notime");  // -time should win
    BOOST_CHECK(GetBoolArg("-time"));
    BOOST_CHECK(GetBoolArg("-time", false));
    BOOST_CHECK(GetBoolArg("-time", true));

    ResetArgs("-time=1 -notime=1");  // -time should win
    BOOST_CHECK(GetBoolArg("-time"));
    BOOST_CHECK(GetBoolArg("-time", false));
    BOOST_CHECK(GetBoolArg("-time", true));

    ResetArgs("-time=0 -notime=0");  // -time should win
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", false));
    BOOST_CHECK(!GetBoolArg("-time", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--time=1");
    BOOST_CHECK(GetBoolArg("-time"));
    BOOST_CHECK(GetBoolArg("-time", false));
    BOOST_CHECK(GetBoolArg("-time", true));

    ResetArgs("--notime=1");
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", false));
    BOOST_CHECK(!GetBoolArg("-time", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-time", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-time", "eleven"), "eleven");

    ResetArgs("-time -bar");
    BOOST_CHECK_EQUAL(GetArg("-time", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-time", "eleven"), "");

    ResetArgs("-time=");
    BOOST_CHECK_EQUAL(GetArg("-time", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-time", "eleven"), "");

    ResetArgs("-time=11");
    BOOST_CHECK_EQUAL(GetArg("-time", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-time", "eleven"), "11");

    ResetArgs("-time=eleven");
    BOOST_CHECK_EQUAL(GetArg("-time", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-time", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-time", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-time", 0), 0);

    ResetArgs("-time -bar");
    BOOST_CHECK_EQUAL(GetArg("-time", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-time=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-time", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-time=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-time", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--time");
    BOOST_CHECK_EQUAL(GetBoolArg("-time"), true);

    ResetArgs("--time=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-time", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-notime");
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", true));
    BOOST_CHECK(!GetBoolArg("-time", false));

    ResetArgs("-notime=1");
    BOOST_CHECK(!GetBoolArg("-time"));
    BOOST_CHECK(!GetBoolArg("-time", true));
    BOOST_CHECK(!GetBoolArg("-time", false));

    ResetArgs("-notime=0");
    BOOST_CHECK(GetBoolArg("-time"));
    BOOST_CHECK(GetBoolArg("-time", true));
    BOOST_CHECK(GetBoolArg("-time", false));

    ResetArgs("-time --notime");
    BOOST_CHECK(GetBoolArg("-time"));

    ResetArgs("-notime -time"); // time always wins:
    BOOST_CHECK(GetBoolArg("-time"));
}

BOOST_AUTO_TEST_SUITE_END()
