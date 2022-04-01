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

TEST_CASE("encode_hr in-place large buffer", "[encoder_hr-ip-01]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    size_t ec_size, pre_size;

    WHEN("empty input") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("");
        REQUIRE(src.length() == 0);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 1);
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 1);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "#");
    }

    WHEN("no specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus");
        REQUIRE(src.length() == 5);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 6);
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 6);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus#");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lo^#rus");
        REQUIRE(src.length() == 7);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 10);
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 10);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lo^[^Drus#");
    }
    WHEN("ESC at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^");
        REQUIRE(src.length() == 6);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[#");
    }
    WHEN("END at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus#");
        REQUIRE(src.length() == 6);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 8);
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^D#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 20;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^##");
        REQUIRE(src.length() == 8);
        pre_size = test_encoder::encoded_size(src.c_str(), src.length());
        REQUIRE(pre_size == 12);
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 12);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[^D^D#");
    }
}

TEST_CASE("encode_hr in-place exact buffer", "[encoder_hr-ip-02]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    size_t ec_size;

    WHEN("empty input") {
        const size_t bsize = 1;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 1);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "#");
    }

    WHEN("no specials") {
        const size_t bsize = 6;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 6);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus#");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 10;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lo^#rus");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 10);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lo^[^Drus#");
    }
    WHEN("ESC at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[#");
    }
    WHEN("END at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus#");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^D#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 12;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^##");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 12);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[^D^D#");
    }
}

TEST_CASE("encode_hr in-place buffer overrun", "[encoder_hr-ip-03]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    size_t ec_size;

    WHEN("no specials") {
        const size_t bsize = 5;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 0);
    }

    WHEN("consecutive specials") {
        const size_t bsize = 9;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lo^#rus");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("ESC at end") {
        const size_t bsize = 7;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("END at end") {
        const size_t bsize = 7;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus#");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 11;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^##");
        std::memcpy(buf, src.c_str(), src.length());
        ec_size = test_encoder::encode(buf, bsize, buf, src.length());
        REQUIRE(ec_size == 0);
    }
}