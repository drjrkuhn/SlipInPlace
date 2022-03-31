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

TEST_CASE("decoder_hr in-place large buffer", "[decoder_hr-ip01]") {
    using namespace hrslip;
    using test_decoder = decoder_hr;
    size_t dc_size, pre_size;

    WHEN("empty input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("");
        REQUIRE(src.length() == 0);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 0);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 0);
    }

    WHEN("single end at input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("#");
        REQUIRE(src.length() == 1);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 0);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 0);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "");
    }

    WHEN("no specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus#");
        REQUIRE(src.length() == 6);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 5);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 5);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lo^[^Drus#");
        REQUIRE(src.length() == 10);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 7);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 7);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lo^#rus");
    }
    WHEN("ESC at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus^[#");
        REQUIRE(src.length() == 8);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 6);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 6);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus^");
    }
    WHEN("END at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus^D#");
        REQUIRE(src.length() == 8);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 6);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 6);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus^[^D^D#");
        REQUIRE(src.length() == 12);
        pre_size = test_decoder::decoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 8);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus^##");
    }
}

TEST_CASE("decoder_hr in-place exact buffer", "[decoder_hr-ip02]") {
    using namespace hrslip;
    using test_decoder = decoder_hr;
    size_t dc_size;

    WHEN("single end at input") {
        const size_t bsize = 1;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("#");
        REQUIRE(src.length() == 1);
        REQUIRE(bsize == src.length());
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 0);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "");
    }

    WHEN("no specials") {
        const size_t bsize = 6;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus#");
        REQUIRE(src.length() == 6);
        REQUIRE(bsize == src.length());
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 5);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 10;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lo^[^Drus#");
        REQUIRE(src.length() == 10);
        REQUIRE(bsize == src.length());
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 7);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lo^#rus");
    }
    WHEN("ESC at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus^[#");
        REQUIRE(src.length() == 8);
        REQUIRE(bsize == src.length());
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 6);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus^");
    }
    WHEN("END at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus^D#");
        REQUIRE(src.length() == 8);
        REQUIRE(bsize == src.length());
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 6);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 12;
        char buf[bsize];
        std::string src = hr_to_base<test_decoder>("Lorus^[^D^D#");
        REQUIRE(src.length() == 12);
        REQUIRE(bsize == src.length());
        std::memcpy(buf, src.c_str(), src.length());
        dc_size = test_decoder::decode(buf, bsize, buf, src.length());
        REQUIRE(dc_size == 8);
        std::string dest = base_to_hr<test_decoder>(buf, dc_size);
        REQUIRE(dest == "Lorus^##");
    }
}
