/*
 * Copyright (c) 2022 MIT.  All rights reserved.
 */

#include "hrslip.h"
#include <catch.hpp>
#include <slipinplace.h>
#include <string>

/**************************************************************************************
 * INCLUDE/MAIN
 **************************************************************************************/

#include <catch.hpp>

TEST_CASE("encoder_null out-of-place large buffer", "[encoder_null-oop-01]") {
    using namespace hrslip;
    using test_encoder = slip::encoder_null<char>;
    size_t ec_size, pre_size;

    WHEN("empty input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("");
        REQUIRE(src.length() == 0);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 1);
        ec_size          = test_encoder::encode(buf, bsize, "", 0);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "\300");
    }

    WHEN("null char input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("0");
        REQUIRE(src.length() == 1);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 3);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 3);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "^@#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "\333\336\300");
    }

    WHEN("no specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("Lorus");
        REQUIRE(src.length() == 5);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 6);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 6);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "Lorus#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\300");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("Lo^#0rus");
        REQUIRE(src.length() == 8);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 12);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 12);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "Lo^[^D^@rus#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lo\333\335\333\334\333\336rus\300");
    }
    WHEN("ESC at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("Lorus^");
        REQUIRE(src.length() == 6);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "Lorus^[#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\333\335\300");
    }

    WHEN("END at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("Lorus#");
        REQUIRE(src.length() == 6);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "Lorus^D#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\333\334\300");
    }

    WHEN("NULL at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("Lorus0");
        REQUIRE(src.length() == 6);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "Lorus^@#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\333\336\300");
    }
    
    WHEN("consecutive specials at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<encoder_hrnull, test_encoder>("Lorus0^##");
        REQUIRE(src.length() == 9);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 14);
        ec_size = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 14);
        std::string dest = recode<test_encoder, encoder_hrnull>(buf, ec_size);
        REQUIRE(dest == "Lorus^@^[^D^D#");
        std::string dslip(buf, ec_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\333\336\333\335\333\334\333\334\300");
    }
}