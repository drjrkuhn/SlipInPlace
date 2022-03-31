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

TEST_CASE("encode_hr out-of-place large buffer", "[encoder_hr-01]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    const size_t bsize = 20;
    char buf[bsize];
    size_t ec_size;

    WHEN("empty input") {
        ec_size = test_encoder::encode(buf, bsize, "", 0);
        REQUIRE(ec_size == 1);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "#");
    }

    WHEN("no specials") {
        std::string src = hr_to_base<test_encoder>("Lorus");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 6);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus#");
    }

    WHEN("consecutive specials") {
        std::string src = hr_to_base<test_encoder>("Lo^#rus");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 10);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lo^[^Drus#");
    }
    WHEN("ESC at end") {
        std::string src = hr_to_base<test_encoder>("Lorus^");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[#");
    }
    WHEN("END at end") {
        std::string src = hr_to_base<test_encoder>("Lorus#");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^D#");
    }
    WHEN("consecutive specials at end") {
        std::string src = hr_to_base<test_encoder>("Lorus^##");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 12);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[^D^D#");
    }
}

TEST_CASE("encode_hr out-of-place exact buffer", "[encoder_hr-02]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    size_t ec_size;

    WHEN("no specials") {
        const size_t bsize = 6;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 6);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus#");
    }

    WHEN("consecutive specials") {
        const size_t bsize = 10;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lo^#rus");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 10);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lo^[^Drus#");
    }
    WHEN("ESC at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[#");
    }
    WHEN("END at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus#");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^D#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 12;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^##");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 12);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[^D^D#");
    }
}

TEST_CASE("encode_hr out-of-place buffer overrun", "[encoder_hr-03]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    size_t ec_size;

    WHEN("no specials") {
        const size_t bsize = 5;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 0);
    }

    WHEN("consecutive specials") {
        const size_t bsize = 9;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lo^#rus");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("ESC at end") {
        const size_t bsize = 7;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("END at end") {
        const size_t bsize = 8;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus#");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 8);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^D#");
    }
    WHEN("consecutive specials at end") {
        const size_t bsize = 12;
        char buf[bsize];
        std::string src = hr_to_base<test_encoder>("Lorus^##");
        ec_size         = test_encoder::encode(buf, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 12);
        std::string dest = base_to_hr<test_encoder>(buf, ec_size);
        REQUIRE(dest == "Lorus^[^D^D#");
    }
}

TEST_CASE("encode_hr bad inputs", "[encoder_hr-04]") {
    using namespace hrslip;
    using test_encoder = encoder_hr;
    const size_t bsize = 20;
    char buf[bsize];
    size_t ec_size;
    std::string src = hr_to_base<test_encoder>("Lorus");

    WHEN("NULL buffer") {
        ec_size = test_encoder::encode(NULL, bsize, src.c_str(), src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("zero buffer size") {
        ec_size = test_encoder::encode(buf, 0, src.c_str(), src.length());
        REQUIRE(ec_size == 0);
    }
    WHEN("NULL input") {
        ec_size = test_encoder::encode(buf, bsize, NULL, src.length());
        REQUIRE(ec_size == 0);
    }
}