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

#if 1
    template <class BASE>
    std::string hr_to_base(std::string src) {
        std::string dest = src;
        std::replace(dest.begin(), dest.end(), encoder_hr::end_code(), BASE::end_code());
        std::replace(dest.begin(), dest.end(), encoder_hr::esc_code(), BASE::esc_code());
        std::replace(dest.begin(), dest.end(), encoder_hr::escend_code(), BASE::escend_code());
        std::replace(dest.begin(), dest.end(), encoder_hr::escesc_code(), BASE::escesc_code());
        return dest;
    }

    template <class BASE>
    std::string hr_to_base(const char* src, size_t size) {
        return hr_to_base<BASE>(std::string(src, size));
    };

    template <class BASE>
    std::string base_to_hr(std::string src) {
        std::string dest = src;
        std::replace(dest.begin(), dest.end(), BASE::end_code(), encoder_hr::end_code());
        std::replace(dest.begin(), dest.end(), BASE::esc_code(), encoder_hr::esc_code());
        std::replace(dest.begin(), dest.end(), BASE::escend_code(), encoder_hr::escend_code());
        std::replace(dest.begin(), dest.end(), BASE::escesc_code(), encoder_hr::escesc_code());
        return dest;
    }

    template <class BASE>
    std::string base_to_hr(const char* src, size_t size) {
        return base_to_hr<BASE>(std::string(src, size));
    }
#endif
};

#endif // __HRSLIP_H__
