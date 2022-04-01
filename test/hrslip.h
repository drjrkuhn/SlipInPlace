/** Human readable SLIP escapes for development */

#pragma once

#include <slipinplace.h>
#include <string>
#include <cassert>

#ifndef __HRSLIP_H__
#    define __HRSLIP_H__

namespace hrslip {
    typedef slip::encoder_base<char, '#', '^', 'D', '['> encoder_hr;
    typedef slip::decoder_base<char, '#', '^', 'D', '['> decoder_hr;

    template <class FROM, class TO>
    std::string recode(const std::string& src) {
        std::string dest = src;
        assert(FROM::n_specials == TO::n_specials);
        for (int i=0; i<FROM::n_specials; i++) {
            std::replace(dest.begin(), dest.end(), FROM::special_codes()[i], TO::special_codes()[i]);
            std::replace(dest.begin(), dest.end(), FROM::escaped_codes()[i], TO::escaped_codes()[i]);
        }
        return dest;
    }
    template <class FROM, class TO>
    std::string recode(const char* src, size_t size) {
        return recode<FROM,TO>(std::string(src,size));
    }
};

#endif // __HRSLIP_H__
