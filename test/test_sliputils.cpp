#include "sliputils.h"

/**************************************************************************************
 * TESTS
 **************************************************************************************/

#include <catch.hpp>

TEST_CASE("escaped", "[slip_utils-01]") {
    using namespace slip;
    using namespace std;
    string res, src("\'\"\?\\\a\b\f\n\r\t\vABCabc\300\301");
    //string src("abc\t");
    res = escaped(src.c_str(), src.length(), "[]");
    REQUIRE("[\\'\\\"\\?\\\\\\a\\b\\f\\n\\r\\t\\vABCabc\\300\\301]" == res);
    // bracket pairs
    res = escaped(src.c_str(), src.length(), "\"\"");
    REQUIRE("\"\\'\\\"\\?\\\\\\a\\b\\f\\n\\r\\t\\vABCabc\\300\\301\"" == res);
    // single bracket
    res = escaped(src.c_str(), src.length(), "\"");
    REQUIRE("\"\\'\\\"\\?\\\\\\a\\b\\f\\n\\r\\t\\vABCabc\\300\\301\"" == res);
    // single bracket
    res = escaped(src.c_str(), src.length(), "'");
    REQUIRE("\'\\'\\\"\\?\\\\\\a\\b\\f\\n\\r\\t\\vABCabc\\300\\301\'" == res);
    // no brackets
    res = escaped(src.c_str(), src.length(), "");
    REQUIRE("\\'\\\"\\?\\\\\\a\\b\\f\\n\\r\\t\\vABCabc\\300\\301" == res);
    // NULL brackets
    res = escaped(src.c_str(), src.length(), NULL);
    REQUIRE("\\'\\\"\\?\\\\\\a\\b\\f\\n\\r\\t\\vABCabc\\300\\301" == res);
}
