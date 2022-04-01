/** Human readable SLIP escapes for development */

#pragma once

#include <slipinplace.h>
#include <string>

#ifndef __HRSLIP_H__
#    define __HRSLIP_H__

namespace hrslip {
    typedef slip::encoder_base<char, '#', '^', 'D', '['> encoder_hr;
    typedef slip::decoder_base<char, '#', '^', 'D', '['> decoder_hr;

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
};

#endif // __HRSLIP_H__
