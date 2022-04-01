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

#    if __cplusplus >= 201103L
#        define _CONSTEXPR constexpr
#        define _USE_CONSTEXPR constexpr
#    else
#        define _CONSTEXPR
#        define _USE_CONSTEXPR const
#    endif

namespace slip {

    /* Standard SLIP: END_V =\300 ESC_V =\333 ESCEND_V =\334 ESCESC_V =\335 */
    template <typename CharT>
    struct stdcodes {
        static _USE_CONSTEXPR CharT SLIP_END    = static_cast<CharT>(0300);
        static _USE_CONSTEXPR CharT SLIP_ESC    = static_cast<CharT>(0333);
        static _USE_CONSTEXPR CharT SLIP_ESCEND = static_cast<CharT>(0334);
        static _USE_CONSTEXPR CharT SLIP_ESCESC = static_cast<CharT>(0335);
    };

    template <typename CharT, CharT END_V, CharT ESC_V, CharT ESCEND_V, CharT ESCESC_V>
    struct slip_base {
        static _USE_CONSTEXPR CharT end_char     = END_V;
        static _USE_CONSTEXPR CharT esc_char     = ESC_V;
        static _USE_CONSTEXPR CharT esc_end_char = ESCEND_V;
        static _USE_CONSTEXPR CharT esc_esc_char = ESCESC_V;
    };

    template <typename CharT, CharT END_V, CharT ESC_V, CharT ESCEND_V, CharT ESCESC_V>
    struct encoder_base : public slip_base<CharT, END_V, ESC_V, ESCEND_V, ESCESC_V> {
        static inline size_t encoded_size(const CharT* buf, size_t buflen) {
            static _USE_CONSTEXPR int _n_special    = 2;
            static _USE_CONSTEXPR CharT _specials[] = {END_V, ESC_V};
            const CharT* buf_end                    = buf + buflen;
            size_t nspecial                         = 0;
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
            _USE_CONSTEXPR size_t BAD_DECODE        = 0;
            static _USE_CONSTEXPR int _n_specials   = 2;
            static _USE_CONSTEXPR CharT _specials[] = {END_V, ESC_V};
            static _USE_CONSTEXPR CharT _escapes[]  = {ESCEND_V, ESCESC_V};
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
            *(dbuf++) = END_V;
            return dbuf - dstart;
        }
    };

    template <typename CharT, CharT END_V, CharT ESC_V, CharT ESCEND_V, CharT ESCESC_V>
    struct decoder_base : public slip_base<CharT, END_V, ESC_V, ESCEND_V, ESCESC_V> {

        static inline size_t decoded_size(const CharT* buf, size_t buflen) {
            const CharT* bufend = buf + buflen;
            size_t nescapes     = 0;
            for (; buf < bufend; buf++) {
                if (buf[0] == ESC_V) {
                    nescapes++;
                    buf++;
                } else if (buf[0] == END_V) {
                    buflen--;
                    buf = bufend;
                }
            }
            return buflen - nescapes;
        }

        static inline size_t decode(CharT* dbuf, size_t dsize, const CharT* sbuf,
                                    size_t ssize) {
            _USE_CONSTEXPR size_t BAD_DECODE        = 0;
            static _USE_CONSTEXPR int _n_special    = 2;
            static _USE_CONSTEXPR CharT _specials[] = {END_V, ESC_V};
            static _USE_CONSTEXPR CharT _escapes[]  = {ESCEND_V, ESCESC_V};
            const CharT* send                       = sbuf + ssize;
            CharT* dstart                           = dbuf;
            CharT* dend                             = dbuf + dsize;
            if (!dbuf || !sbuf || ssize < 1 || dsize < 1) return BAD_DECODE;
            int isp;

            while (sbuf < send) {
                if (sbuf[0] == END_V) return dbuf - dstart;
                if (sbuf[0] != ESC_V) { // regular character
                    if (dbuf >= dend) return BAD_DECODE;
                    *(dbuf++) = *(sbuf++);
                } else {
                    // check char after escape
                    sbuf++;
                    if (sbuf >= send || dbuf >= dend) return BAD_DECODE;
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

    template <typename CharT>
    struct encoder
        : public encoder_base<CharT,
                              stdcodes<CharT>::SLIP_END,
                              stdcodes<CharT>::SLIP_ESC,
                              stdcodes<CharT>::SLIP_ESCEND,
                              stdcodes<CharT>::SLIP_ESCESC> {
    };

    template <typename CharT>
    struct decoder
        : public decoder_base<CharT,
                              stdcodes<CharT>::SLIP_END,
                              stdcodes<CharT>::SLIP_ESC,
                              stdcodes<CharT>::SLIP_ESCEND,
                              stdcodes<CharT>::SLIP_ESCESC> {
    };

    //using byte_encoder = encoder<unsigned char>;
    //using char_encoder = encoder<char>;
    //using byte_decoder = decoder<unsigned char>;
    //using char_decoder = decoder<char>;
}

#endif // __SLIPINPLACE_H__