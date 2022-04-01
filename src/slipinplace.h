/*!
 *  @file slipinplace.h
 *
 *  @mainpage SlipInPlace
 *
 *  @section intro_sec Introduction
 *
 *  Library for in-place SLIP encoding and decoding
 *
 *  @section author Author
 *
 *  Written by Jeffrey Kuhn <jrkuhn@mit.edu>.
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */

#pragma once

#ifndef __SLIPINPLACE_H
#    define __SLIPINPLACE_H__

#    include <string.h> // for memmove

#    if __cplusplus >= 201103L
#        define _CONSTEXPR constexpr
#        define _USE_CONSTEXPR constexpr
#    else
#        define _CONSTEXPR
#        define _USE_CONSTEXPR const
#    endif

#define _NOEXCEPT noexcept

namespace slip {

    /* Standard SLIP: END =\300 ESC =\333 ESCEND =\334 ESCESC =\335 */
    struct stdcodes {
        static _USE_CONSTEXPR unsigned char SLIP_END    = 0300; // 0xC0
        static _USE_CONSTEXPR unsigned char SLIP_ESC    = 0333; // 0xDB
        static _USE_CONSTEXPR unsigned char SLIP_ESCEND = 0334; // 0xDC
        static _USE_CONSTEXPR unsigned char SLIP_ESCESC = 0335; // 0xDD
    };

    
    template <typename CharT, unsigned char END_C, unsigned char ESC_C, unsigned char ESCEND_C, unsigned char ESCESC_C>
    struct slip_base {
        // target C++11 - no variable or static data member at class scope 
        // [(since C++14)](https://en.cppreference.com/w/cpp/language/variable_template)
        static _USE_CONSTEXPR CharT end_code() _NOEXCEPT { return (CharT)END_C; }
        static _USE_CONSTEXPR CharT esc_code() _NOEXCEPT  { return (CharT) ESC_C; }
        static _USE_CONSTEXPR CharT escend_code() _NOEXCEPT { return (CharT)ESCEND_C; }
        static _USE_CONSTEXPR CharT escesc_code() _NOEXCEPT { return (CharT)ESCESC_C; }
    };

    template <typename CharT, unsigned char END_C, unsigned char ESC_C, unsigned char ESCEND_C, unsigned char ESCESC_C>
    struct encoder_base : public slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C> {
        typedef slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C> base;
        using base::end_code;
        using base::esc_code;
        using base::escend_code;
        using base::escesc_code;
        
        static inline size_t encoded_size(const CharT* buf, size_t buflen) _NOEXCEPT {
            // C++11 statics allowed inside functions but not classes
            static _USE_CONSTEXPR int n_special    = 2;
            static _USE_CONSTEXPR CharT specials[] = {end_code(), esc_code()};
            const CharT* buf_end                    = buf + buflen;
            size_t nspecial                         = 0;
            int isp;
            for (; buf < buf_end; buf++) {
                for (isp = 0; isp < n_special; isp++) {
                    if (buf[0] == specials[isp]) {
                        nspecial++;
                        break;
                    }
                }
            }
            return buflen + nspecial + 1;
        }
        static inline size_t encode(CharT* dbuf, size_t dsize, const CharT* sbuf,
                                    size_t ssize) _NOEXCEPT {
            // C++11 statics allowed inside functions but not classes
            static _USE_CONSTEXPR size_t BAD_DECODE        = 0;
            static _USE_CONSTEXPR int n_specials   = 2;
            static _USE_CONSTEXPR CharT specials[] = {end_code(), esc_code()};
            static _USE_CONSTEXPR CharT escapes[]  = {escend_code(), escesc_code()};
            const CharT* send                       = sbuf + ssize;
            CharT* dstart                           = dbuf;
            CharT* dend                             = dbuf + dsize;
            if (!dbuf || !sbuf || dsize < ssize + 1)
                return BAD_DECODE;
            if (dbuf <= sbuf && sbuf <= dend) { // sbuf somewhere in dbuf. So in-place
                // copy source to end of the dest_buf. memmove copies overlaps in reverse. Use
                // std::copy_backward if converting this function to iterators
                sbuf = (CharT*)memmove(dbuf + dsize - ssize, sbuf, ssize);
                send = sbuf + ssize;
            }
            int isp;

            while (sbuf < send) {
                isp = 0;
                for (; isp < n_specials; isp++) { // encode this char?
                    if (sbuf[0] == specials[isp]) break;
                }
                if (isp >= n_specials) { // regular character
                    if (dbuf >= dend) return BAD_DECODE;
                    *(dbuf++) = *(sbuf++); // copy it
                } else {
                    if (dbuf + 1 >= dend) return BAD_DECODE;
                    *(dbuf++) = esc_code();
                    *(dbuf++) = escapes[isp];
                    sbuf++;
                }
            }

            if (dbuf >= dend) {
                return BAD_DECODE;
            }
            *(dbuf++) = end_code();
            return dbuf - dstart;
        }
    };

    template <typename CharT, unsigned char END_C, unsigned char ESC_C, unsigned char ESCEND_C, unsigned char ESCESC_C>
    struct decoder_base : public slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C> {
        typedef slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C> base;
        using base::end_code;
        using base::esc_code;
        using base::escend_code;
        using base::escesc_code;

        static inline size_t decoded_size(const CharT* buf, size_t buflen) _NOEXCEPT {
            const CharT* bufend = buf + buflen;
            size_t nescapes     = 0;
            for (; buf < bufend; buf++) {
                if (buf[0] == esc_code()) {
                    nescapes++;
                    buf++;
                } else if (buf[0] == end_code()) {
                    buflen--;
                    buf = bufend;
                }
            }
            return buflen - nescapes;
        }

        static inline size_t decode(CharT* dbuf, size_t dsize, const CharT* sbuf,
                                    size_t ssize) _NOEXCEPT {
            // C++11 statics allowed inside functions but not classes
            static _USE_CONSTEXPR size_t BAD_DECODE        = 0;
            static _USE_CONSTEXPR int n_special    = 2;
            static _USE_CONSTEXPR CharT specials[] = {end_code(), esc_code()};
            static _USE_CONSTEXPR CharT escapes[]  = {escend_code(), escesc_code()};
            const CharT* send                       = sbuf + ssize;
            CharT* dstart                           = dbuf;
            CharT* dend                             = dbuf + dsize;
            if (!dbuf || !sbuf || ssize < 1 || dsize < 1) return BAD_DECODE;
            int isp;

            while (sbuf < send) {
                if (sbuf[0] == end_code()) return dbuf - dstart;
                if (sbuf[0] != esc_code()) { // regular character
                    if (dbuf >= dend) return BAD_DECODE;
                    *(dbuf++) = *(sbuf++);
                } else {
                    // check char after escape
                    sbuf++;
                    if (sbuf >= send || dbuf >= dend) return BAD_DECODE;
                    for (isp = 0; isp < n_special; isp++) {
                        if (sbuf[0] == escapes[isp]) break;
                    }
                    if (isp >= n_special) return BAD_DECODE;
                    *(dbuf++) = specials[isp];
                    sbuf++;
                }
            }
            return dbuf - dstart;
        }
    };

    template <typename CharT>
    struct encoder
        : public encoder_base<CharT,
                              stdcodes::SLIP_END,
                              stdcodes::SLIP_ESC,
                              stdcodes::SLIP_ESCEND,
                              stdcodes::SLIP_ESCESC> {
    };

    template <typename CharT>
    struct decoder
        : public decoder_base<CharT,
                              stdcodes::SLIP_END,
                              stdcodes::SLIP_ESC,
                              stdcodes::SLIP_ESCEND,
                              stdcodes::SLIP_ESCESC> {
    };

    //using byte_encoder = encoder<unsigned char>;
    //using char_encoder = encoder<char>;
    //using byte_decoder = decoder<unsigned char>;
    //using char_decoder = decoder<char>;
}

#endif // __SLIPINPLACE_H__