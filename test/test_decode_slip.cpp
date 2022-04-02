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

TEST_CASE("decoder_slip out-of-place large buffer", "[decoder_slip-oop-01]") {
    using namespace hrslip;
    using test_decoder = slip::decoder<char>;
    size_t dc_size, pre_size;

    WHEN("empty input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("");
        REQUIRE(src.length() == 0);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 0);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }

    WHEN("single end at input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("#");
        REQUIRE(src.length() == 1);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 0);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "");
    }

    WHEN("no specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus#");
        REQUIRE(src.length() == 6);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 5);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 5);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus");
        std::string dslip(buf, dc_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus");
    }

    WHEN("bad encoding") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lo^_rus#");
        REQUIRE(src.length() == 8);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        // decoded_size does not detect bad encoding!
        REQUIRE(pre_size == 6);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }    

    WHEN("consecutive specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lo^[^Drus#");
        REQUIRE(src.length() == 10);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 7);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 7);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lo^#rus");
        std::string dslip(buf, dc_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lo\333\300rus");
    }

    WHEN("ESC at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^[#");
        REQUIRE(src.length() == 8);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 6);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 6);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus^");
        std::string dslip(buf, dc_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\333");
    }

    WHEN("END at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^D#");
        REQUIRE(src.length() == 8);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 6);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 6);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus#");
        std::string dslip(buf, dc_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\300");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^[^D^D#");
        REQUIRE(src.length() == 12);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 8);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus^##");
        std::string dslip(buf, dc_size);
        /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
        REQUIRE(dslip == "Lorus\333\300\300");
    }
}
