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

TEST_CASE("decoder_hr out-of-place large buffer", "[decoder_hr-oop-01]") {
    using namespace hrslip;
    using test_decoder = decoder_hr;
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
    }
}


TEST_CASE("decoder_hr out-of-place exact buffer", "[decoder_hr-oop-02]") {
    using namespace hrslip;
    using test_decoder = decoder_hr;
    size_t dc_size;

    WHEN("single end at input") {
        const size_t bsize = 1;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("#");
        REQUIRE(src.length() == 1);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "");
    }

    WHEN("no specials") {
        const size_t bsize = 5;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus#");
        REQUIRE(src.length() == 6);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 5);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 7;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lo^[^Drus#");
        REQUIRE(src.length() == 10);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 7);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lo^#rus");
    }
    WHEN("ESC at end") {
        const size_t bsize = 6;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^[#");
        REQUIRE(src.length() == 8);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 6);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus^");
    }
    WHEN("END at end") {
        const size_t bsize = 6;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^D#");
        REQUIRE(src.length() == 8);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 6);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^[^D^D#");
        REQUIRE(src.length() == 12);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 8);
        std::string dest = recode<test_decoder,decoder_hr>(buf, dc_size);
        REQUIRE(dest == "Lorus^##");
    }
}

TEST_CASE("decoder_hr out-of-place buffer overrun", "[decoder_hr-oop-02]") {
    using namespace hrslip;
    using test_decoder = decoder_hr;
    size_t dc_size;

    WHEN("no specials") {
        const size_t bsize = 4;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus#");
        REQUIRE(src.length() == 6);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }

    WHEN("consecutive specials") {
        const size_t bsize = 6;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lo^[^Drus#");
        REQUIRE(src.length() == 10);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }
    WHEN("ESC at end") {
        const size_t bsize = 5;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^[#");
        REQUIRE(src.length() == 8);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }
    WHEN("END at end") {
        const size_t bsize = 5;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^D#");
        REQUIRE(src.length() == 8);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 7;
        char buf[bsize];
        std::string src = recode<decoder_hr,test_decoder>("Lorus^[^D^D#");
        REQUIRE(src.length() == 12);
        dc_size = test_decoder::decode(buf, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }
}

TEST_CASE("decoder_hr bad inputs", "[decoder_hr-oop-04]") {
    using namespace hrslip;
    using test_decoder = encoder_hr;
    const size_t bsize = 20;
    char buf[bsize];
    size_t dc_size;
    std::string src = recode<decoder_hr,test_decoder>("Lorus");

    WHEN("NULL buffer") {
        dc_size = test_decoder::encode(NULL, bsize, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }
    WHEN("zero buffer size") {
        dc_size = test_decoder::encode(buf, 0, src.c_str(), src.length());
        REQUIRE(dc_size == 0);
    }
    WHEN("NULL input") {
        dc_size = test_decoder::encode(buf, bsize, NULL, src.length());
        REQUIRE(dc_size == 0);
    }
}
