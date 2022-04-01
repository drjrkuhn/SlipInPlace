/** Human readable SLIP escapes for development */

#pragma once

#include <slipinplace.h>
#include <string>

#ifndef __HRSLIP_H__
#    define __HRSLIP_H__

namespace hrslip {
    using encoder_hr = slip::encoder_base<char, '#', '^', 'D', '['>;
    using decoder_hr = slip::decoder_base<char, '#', '^', 'D', '['>;

    template <class BASE>
    std::string hr_to_base(std::string src) {
        std::string dest = src;
        std::replace(dest.begin(), dest.end(), encoder_hr::end_char, BASE::end_char);
        std::replace(dest.begin(), dest.end(), encoder_hr::esc_char, BASE::esc_char);
        std::replace(dest.begin(), dest.end(), encoder_hr::esc_end_char, BASE::esc_end_char);
        std::replace(dest.begin(), dest.end(), encoder_hr::esc_esc_char, BASE::esc_esc_char);
        return dest;
    }

    template <class BASE>
    std::string hr_to_base(const char* src, size_t size) {
        return hr_to_base<BASE>(std::string(src, size));
    };

    template <class BASE>
    std::string base_to_hr(std::string src) {
        std::string dest = src;
        std::replace(dest.begin(), dest.end(), BASE::end_char, encoder_hr::end_char);
        std::replace(dest.begin(), dest.end(), BASE::esc_char, encoder_hr::esc_char);
        std::replace(dest.begin(), dest.end(), BASE::esc_end_char, encoder_hr::esc_end_char);
        std::replace(dest.begin(), dest.end(), BASE::esc_esc_char, encoder_hr::esc_esc_char);
        return dest;
    }

    template <class BASE>
    std::string base_to_hr(const char* src, size_t size) {
        return base_to_hr<BASE>(std::string(src, size));
    }
};
#endif __HRSLIP_H__
