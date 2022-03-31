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

#    include <cstdint>
#    include <string.h> // for memmove

//constexpr uint8_t END_CHAR      = '#'; //= 0300;   // End of packet character;
//constexpr uint8_t ESC_CHAR      = '^'; //= 0333;   // Escape character
//constexpr uint8_t ESC_END_CHAR  = 'D'; //= 0334;   // Escaped end character
//constexpr uint8_t ESC_ESC_CHAR  = '['; //= 0335;   // Escaped escape character
//constexpr uint8_t ESC_NULL_CHAR = 'C'; //* 0336;	// Escaped null char

#    if __cplusplus >= 201103L
#        define _CONSTEXPR constexpr
#        define _USE_CONSTEXPR constexpr
#    else
#        define _CONSTEXPR
#        define _USE_CONSTEXPR const
#    endif

namespace slip {

    template <typename CharT, CharT END = CharT(0300), CharT ESC = CharT(0333), 
        CharT ESCEND = CharT(0334), CharT ESCESC = CharT(0335)>
    struct slip_base {
        static _USE_CONSTEXPR CharT end_char     = END;
        static _USE_CONSTEXPR CharT esc_char     = ESC;
        static _USE_CONSTEXPR CharT esc_end_char = ESCEND;
        static _USE_CONSTEXPR CharT esc_esc_char = ESCESC;
    };

    template <typename CharT, CharT END = CharT(0300), CharT ESC = CharT(0333),
              CharT ESCEND = CharT(0334), CharT ESCESC = CharT(0335)>
    struct encoder_base : public slip_base<CharT, END, ESC, ESCEND, ESCESC> {
        static inline size_t encoded_size(const CharT* buf, size_t buflen) {
            static _USE_CONSTEXPR int _n_special    = 2;
            static _USE_CONSTEXPR CharT _specials[] = {END, ESC};
            const CharT* buf_end = buf + buflen;
            size_t nspecial      = 0;
            int isp;
            for (; buf < buf_end; buf++) {
                for (isp = 0; isp < _n_special; isp++) {
                    if (buf[0] == _specials[isp]) {
                        nspecial++;
                        break;
                    }
                }
            }
            return buflen + nspecial + 1;
        }
        static inline size_t encode(CharT* dbuf, size_t dsize, const CharT* sbuf,
                                    size_t ssize) {
            _USE_CONSTEXPR size_t BAD_DECODE = 0;
            static _USE_CONSTEXPR int _n_specials    = 2;
            static _USE_CONSTEXPR CharT _specials[] = {END, ESC};
            static _USE_CONSTEXPR CharT _escapes[] = {ESCEND, ESCESC};
            const CharT* send                = sbuf + ssize;
            CharT* dstart                    = dbuf;
            CharT* dend                      = dbuf + dsize;
            if (!dbuf || !sbuf || ssize == 0 || dsize < ssize + 1)
                return BAD_DECODE;
            // copy source to end of the dest_buf. memmove copies overlaps in reverse. Use
            // std::copy_backward if converting this function to iterators
            sbuf = (CharT*)memmove(dbuf + dsize - ssize, sbuf, ssize);
            send = sbuf + ssize;
            int isp;

            while (sbuf < send) {
                isp = 0;
                for (; isp < _n_specials; isp++) { // encode this char?
                    if (sbuf[0] == _specials[isp]) break;
                }
                if (isp >= _n_specials) { // regular character
                    if (dbuf >= dend) return BAD_DECODE;
                    *(dbuf++) = *(sbuf++); // copy it
                } else {
                    if (dbuf + 1 >= dend) return BAD_DECODE;
                    *(dbuf++) = esc_char;
                    *(dbuf++) = _escapes[isp];
                    sbuf++;
                }
            }

            if (dbuf >= dend) {
                return BAD_DECODE;
            }
            *(dbuf++) = END;
            return dbuf - dstart;
        }
    };

    using byte_encoder = encoder_base<unsigned char>;
    using encoder      = encoder_base<char>;
    using encoder_hr   = encoder_base<char,'#','^','D','['>;

    template <typename CharT, CharT END = CharT(0300), CharT ESC = CharT(0333),
              CharT ESCEND = CharT(0334), CharT ESCESC = CharT(0335)>
    struct decoder_base : public slip_base<CharT, END, ESC, ESCEND, ESCESC> {

        static inline size_t decoded_size(const CharT* buf, size_t buflen) {
            const CharT* bufend = buf + buflen;
            size_t nescapes     = 0;
            for (; buf < bufend; buf++) {
                if (buf[0] == ESC) {
                    nescapes++;
                    buf++;
                } else if (buf[0] == END) {
                    buflen--;
                    buf = bufend;
                }
            }
            return buflen - nescapes;
        }

        static inline size_t decode(CharT* dbuf, size_t dsize, const CharT* sbuf,
                                    size_t ssize) {
            _USE_CONSTEXPR size_t BAD_DECODE = 0;
            static _USE_CONSTEXPR int _n_special    = 2;
            static _USE_CONSTEXPR CharT _specials[] = {END, ESC};
            static _USE_CONSTEXPR CharT _escapes[]  = {ESCEND, ESCESC};
            const CharT* send                = sbuf + ssize;
            CharT* dstart                    = dbuf;
            CharT* dend                      = dbuf + dsize;
            if (!dbuf || !sbuf || ssize < 1 || dsize < ssize - 1) return BAD_DECODE;
            int isp;

            while (sbuf < send) {
                if (sbuf[0] == END) return dbuf - dstart;
                if (sbuf[0] != ESC) { // regular character
                    if (dbuf >= dend) return BAD_DECODE;
                    *(dbuf++) = *(sbuf++);
                } else {
                    // check char after escape
                    if (sbuf++ >= send) return BAD_DECODE;
                    for (isp = 0; isp < _n_special; isp++) {
                        if (sbuf[0] == _escapes[isp]) break;
                    }
                    if (isp >= _n_special) return BAD_DECODE;
                    *(dbuf++) = _specials[isp];
                    sbuf++;
                }
            }
            return dbuf - dstart;
        }
    };

    using byte_decoder = decoder_base<unsigned char>;
    using decoder      = decoder_base<char>;
    using decoder_hr   = decoder_base<char, '#', '^', 'D', '['>;
}

#endif // __SLIPINPLACE_H__