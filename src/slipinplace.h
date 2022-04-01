/*!
 *  @file slipinplace.h
 *
 *  @mainpage SlipInPlace
 *
 *  @section intro_sec Introduction
 *
 *  Library for in-place SLIP encoding and decoding.
 *  Optional SLIP+NUL codec allows encoding of buffers with NULL characters
 *  in the middle for string-based communication.
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

#    define _NOEXCEPT noexcept

namespace slip {

    /* Standard SLIP codes: END=\300 ESC=\333 ESCEND=\334 ESCESC=\335. */
    struct stdcodes {
        static _USE_CONSTEXPR unsigned char SLIP_END     = 0300; ///< 0xC0
        static _USE_CONSTEXPR unsigned char SLIP_ESC     = 0333; ///< 0xDB
        static _USE_CONSTEXPR unsigned char SLIPX_NUL    = 0;    ///< 0 (nonstandard)
        static _USE_CONSTEXPR unsigned char SLIP_ESCEND  = 0334; ///< 0xDC
        static _USE_CONSTEXPR unsigned char SLIP_ESCESC  = 0335; ///< 0xDD
        static _USE_CONSTEXPR unsigned char SLIPX_ESCNUL = 0336; ///< 0xDE (nonstandard)
    };

    /**
     * @brief Base class for custom SLIP codes.
     *
     * targets C++11 - no variable or static data member at class scope
     * [(since C++14)](https://en.cppreference.com/w/cpp/language/variable_template)
     *
     * @tparam CharT        unsigned char or char
     * @tparam END_C        end character code \300
     * @tparam ESC_C        escape character code \333
     * @tparam ESCEND_C     escaped end character code \334
     * @tparam ESCESC_C     escaped escape character code \334
     * @tparam NUL_C        NULL character code \000
     * @tparam ESCNUL_C     escaped NULL character code. If set to anything other than zero, NULLs will be processed
     *
     */
    template <typename CharT, unsigned char END_C, unsigned char ESC_C,
              unsigned char ESCEND_C, unsigned char ESCESC_C,
              unsigned char NUL_C = 0, unsigned char ESCNUL_C = 0>
    struct slip_base {
        static _USE_CONSTEXPR CharT end_code() _NOEXCEPT { return (CharT)END_C; }       ///< end code
        static _USE_CONSTEXPR CharT esc_code() _NOEXCEPT { return (CharT)ESC_C; }       ///< escape code
        static _USE_CONSTEXPR CharT nul_code() _NOEXCEPT { return (CharT)NUL_C; }       ///< NULL code
        static _USE_CONSTEXPR CharT escend_code() _NOEXCEPT { return (CharT)ESCEND_C; } ///< escaped end code
        static _USE_CONSTEXPR CharT escesc_code() _NOEXCEPT { return (CharT)ESCESC_C; } ///< escped escape code
        static _USE_CONSTEXPR CharT escnul_code() _NOEXCEPT { return (CharT)ESCNUL_C; } ///< escaped NULL code if present

        /** Maximum number of special characters to escape while encoding */
        static _USE_CONSTEXPR int max_specials = 3;
        /**
         *  Number of special characters in this codec.
         *  Standard SLIP uses two (end and esc). SLIP+NULL adds a third code.
         */
        static _USE_CONSTEXPR int n_specials = ESCNUL_C == 0 ? 2 : 3;

        /** An array of special characters to escape */
        static inline const CharT* special_codes() _NOEXCEPT {
            static _USE_CONSTEXPR CharT specials[] = {end_code(), esc_code(), 0};
            return specials;
        }
        /** An array of special character escapes in the same order as special_codes(). */
        static inline const CharT* escaped_codes() _NOEXCEPT {
            static _USE_CONSTEXPR CharT escapes[] = {escend_code(), escesc_code(), escnul_code()};
            return escapes;
        }
    };

    /**
     * @brief Basic SLIP encoder.
     *
     * Automatically handles out-of-place encoding via copy or in-place encoding given
     * a buffer of sufficient size.
     *
     * @tparam CharT        unsigned char or char
     * @tparam END_C        end character code \300
     * @tparam ESC_C        escape character code \333
     * @tparam ESCEND_C     escaped end character code \334
     * @tparam ESCESC_C     escaped escape character code \334
     * @tparam ESCNUL_C     escaped NULL character code \335 (non-standard)
     */
    template <typename CharT, unsigned char END_C, unsigned char ESC_C,
              unsigned char ESCEND_C, unsigned char ESCESC_C,
              unsigned char NUL_C = 0, unsigned char ESCNUL_C = 0>
    struct encoder_base : public slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C, NUL_C, ESCNUL_C> {
        typedef slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C, NUL_C, ESCNUL_C> base;
        using base::end_code;
        using base::esc_code;
        using base::escaped_codes;
        using base::escend_code;
        using base::escesc_code;
        using base::escnul_code;
        using base::max_specials;
        using base::n_specials;
        using base::nul_code;
        using base::special_codes;

        /**
         * @brief Pre-calculate the size after SLIP encoding.
         *
         * @param src       pointer to source buffer
         * @param srcsize   size of source buffer to parse
         * @return size_t   size needed to encode this buffer
         */
        static inline size_t encoded_size(const CharT* src, size_t srcsize) _NOEXCEPT {
            // C++11 statics allowed inside functions but not classes
            static const CharT* specials = special_codes();
            const CharT* buf_end         = src + srcsize;
            size_t nspecial              = 0;
            int isp;
            for (; src < buf_end; src++) {
                for (isp = 0; isp < n_specials; isp++) {
                    if (src[0] == specials[isp]) {
                        nspecial++;
                        break;
                    }
                }
            }
            return srcsize + nspecial + 1;
        }

        /**
         * @brief Encode a buffer using a SLIP protocol.
         *
         * Automatically handles out-of-place encoding via copy or in-place encoding given
         * a buffer of sufficient size.
         *
         * Encoding always expands the size of the source packet.
         * For in-place encoding, the algorithm first copies the string to the end of
         * the destination buffer, then begins encoding left-to-right.
         *
         * > :warning: Encode in-place clobbers the end of the destiation buffer past
         * >            the returned size!
         *
         * @param dest      destination buffer
         * @param destsize  dest buffer size - must be sufficiently large
         * @param src       source buffer
         * @param srcsize   size of source to encode
         * @return size_t   final encoded size or 0 if there was an error while encoding
         */
        static inline size_t encode(CharT* dest, size_t destsize, const CharT* src,
                                    size_t srcsize) _NOEXCEPT {
            // C++11 statics allowed inside functions but not classes
            static _USE_CONSTEXPR size_t BAD_DECODE = 0;
            static const CharT* specials            = special_codes();
            static const CharT* escapes             = escaped_codes();
            const CharT* send                       = src + srcsize;
            CharT* dstart                           = dest;
            CharT* dend                             = dest + destsize;
            if (!dest || !src || destsize < srcsize + 1)
                return BAD_DECODE;
            if (dest <= src && src <= dend) { // sbuf somewhere in dbuf. So in-place
                // copy source to end of the dest_buf. memmove copies overlaps in reverse. Use
                // std::copy_backward if converting this function to iterators
                src  = (CharT*)memmove(dest + destsize - srcsize, src, srcsize);
                send = src + srcsize;
            }
            int isp;

            while (src < send) {
                isp = 0;
                for (; isp < n_specials; isp++) { // encode this char?
                    if (src[0] == specials[isp]) break;
                }
                if (isp >= n_specials) { // regular character
                    if (dest >= dend) return BAD_DECODE;
                    *(dest++) = *(src++); // copy it
                } else {
                    if (dest + 1 >= dend) return BAD_DECODE;
                    *(dest++) = esc_code();
                    *(dest++) = escapes[isp];
                    src++;
                }
            }

            if (dest >= dend) {
                return BAD_DECODE;
            }
            *(dest++) = end_code();
            return dest - dstart;
        }
    };

    /**
     * @brief Basic SLIP decoder.
     *
     * Automatically handles both out-of-place and in-place decoding.
     *
     * @tparam CharT        unsigned char or char
     * @tparam END_C        end character code \300
     * @tparam ESC_C        escape character code \333
     * @tparam ESCEND_C     escaped end character code \334
     * @tparam ESCESC_C     escaped escape character code \334
     * @tparam NUL_C        NULL character code \000
     * @tparam ESCNUL_C     escaped NULL character code. If set to anything other than zero, NULLs will be processed
     */
    template <typename CharT, unsigned char END_C, unsigned char ESC_C,
              unsigned char ESCEND_C, unsigned char ESCESC_C,
              unsigned char NUL_C = 0, unsigned char ESCNUL_C = 0>
    struct decoder_base : public slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C, NUL_C, ESCNUL_C> {
        typedef slip_base<CharT, END_C, ESC_C, ESCEND_C, ESCESC_C, NUL_C, ESCNUL_C> base;
        using base::end_code;
        using base::esc_code;
        using base::escaped_codes;
        using base::escend_code;
        using base::escesc_code;
        using base::escnul_code;
        using base::max_specials;
        using base::n_specials;
        using base::nul_code;
        using base::special_codes;

        /**
         * @brief Pre-calculate the size after SLIP decoding.
         *
         * Does not check the validity of two-byte escape sequences, just their presence.
         *
         * @param src       pointer to source buffer
         * @param srcsize   size of source buffer to parse
         * @return size_t   size needed to decode this buffer
         */
        static inline size_t decoded_size(const CharT* src, size_t srcsize) _NOEXCEPT {
            const CharT* bufend = src + srcsize;
            size_t nescapes     = 0;
            for (; src < bufend; src++) {
                if (src[0] == esc_code()) {
                    nescapes++;
                    src++;
                } else if (src[0] == end_code()) {
                    srcsize--;
                    src = bufend;
                }
            }
            return srcsize - nescapes;
        }

        /**
         * @brief Encode a buffer using a SLIP protocol.
         *
         * Automatically handles both out-of-place and in-place decoding.
         *
         * Since the decoded size is always smaller, in-place decoding works
         * witout copying.
         *
         * > :warning: The end of destiation buffer past the returned size is not cleared.
         *
         * @param dest      destination buffer
         * @param destsize  dest buffer size - must be sufficiently large
         * @param src       source buffer
         * @param srcsize   size of source to decode
         * @return size_t   final decoded size or 0 if there was an error while decoding
         */
        static inline size_t decode(CharT* dest, size_t destsize, const CharT* src,
                                    size_t srcsize) _NOEXCEPT {
            // C++11 statics allowed inside functions but not classes
            static _USE_CONSTEXPR size_t BAD_DECODE = 0;
            static const CharT* specials            = special_codes();
            static const CharT* escapes             = escaped_codes();
            const CharT* send                       = src + srcsize;
            CharT* dstart                           = dest;
            CharT* dend                             = dest + destsize;
            if (!dest || !src || srcsize < 1 || destsize < 1) return BAD_DECODE;
            int isp;

            while (src < send) {
                if (src[0] == end_code()) return dest - dstart;
                if (src[0] != esc_code()) { // regular character
                    if (dest >= dend) return BAD_DECODE;
                    *(dest++) = *(src++);
                } else {
                    // check char after escape
                    src++;
                    if (src >= send || dest >= dend) return BAD_DECODE;
                    for (isp = 0; isp < n_specials; isp++) {
                        if (src[0] == escapes[isp]) break;
                    }
                    if (isp >= n_specials) return BAD_DECODE;
                    *(dest++) = specials[isp];
                    src++;
                }
            }
            return dest - dstart;
        }
    };

    /**
     * @brief Standard slip encoder.
     * @copydoc encoder_base
     *
     * @see "hrslip.h" in test-code for an example of a human-readable SLIP fornmat.
     *
     * @tparam CharT    may be char or unsigned char (uint8_t)
     */
    template <typename CharT>
    struct encoder : public encoder_base<CharT,
                                         stdcodes::SLIP_END,
                                         stdcodes::SLIP_ESC,
                                         stdcodes::SLIP_ESCEND,
                                         stdcodes::SLIP_ESCESC> {
    };

    /**
     * @brief Standard slip encoder with additional NULL character encoding.
     * @copydoc encoder_base
     *
     * @see "hrslip.h" in test-code for an example of a human-readable SLIP fornmat.
     *
     * @tparam CharT    may be char or unsigned char (uint8_t)
     */
    template <typename CharT>
    struct encoder_null : public encoder_base<CharT,
                                              stdcodes::SLIP_END,
                                              stdcodes::SLIP_ESC,
                                              stdcodes::SLIP_ESCEND,
                                              stdcodes::SLIP_ESCESC,
                                              stdcodes::SLIPX_NUL,
                                              stdcodes::SLIPX_ESCNUL> {
    };

    /**
     * @brief Standard slip decoder.
     * @copydoc decoder_base
     *
     * @see "hrslip.h" in test-code for an example of a human-readable SLIP fornmat.
     *
     * @tparam CharT    may be char or unsigned char (uint8_t)
     */
    template <typename CharT>
    struct decoder : public decoder_base<CharT,
                                         stdcodes::SLIP_END,
                                         stdcodes::SLIP_ESC,
                                         stdcodes::SLIP_ESCEND,
                                         stdcodes::SLIP_ESCESC> {
    };
    /**
     * @brief Standard slip decoder with additional NULL character encoding.
     * @copydoc decoder_base
     *
     * @see "hrslip.h" in test-code for an example of a human-readable SLIP fornmat.
     *
     * @tparam CharT    may be char or unsigned char (uint8_t)
     */
    template <typename CharT>
    struct decoder_null : public decoder_base<CharT,
                                              stdcodes::SLIP_END,
                                              stdcodes::SLIP_ESC,
                                              stdcodes::SLIP_ESCEND,
                                              stdcodes::SLIP_ESCESC,
                                              stdcodes::SLIPX_NUL,
                                              stdcodes::SLIPX_ESCNUL> {
    };
}

#endif // __SLIPINPLACE_H__