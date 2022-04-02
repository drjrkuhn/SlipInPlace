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

/**
 * @brief Unroll codelet testing loop, defaults to true (1).
 *
 * To turn off codelet loop unrolling, set this macro to
 * false (0) before including the library header.
 *
 * ```c++
 * #define SLIP_UNROLL_LOOPS 0
 * #include <slipinplace.h>
 * ```
 */

#ifndef __SLIPINPLACE_H
    #define __SLIPINPLACE_H__

    #ifndef SLIP_UNROLL_LOOPS
        #define SLIP_UNROLL_LOOPS 1
    #endif

    #include <string.h> // for memmove

    #if !defined(__ALWAYS_INLINE__)
        #if defined(__GNUC__) && __GNUC__ > 3
            #define __ALWAYS_INLINE__ inline __attribute__((__always_inline__))
        #elif defined(_MSC_VER)
            // Does nothing in Debug mode (with standard option /Ob0)
            #define __ALWAYS_INLINE__ inline __forceinline
        #else
            #define __ALWAYS_INLINE__ inline
        #endif
    #endif

namespace slip {

    /* Standard SLIP codes: END=\300 ESC=\333 ESCEND=\334 ESCESC=\335. */
    struct stdcodes {
        static constexpr unsigned char SLIP_END      = 0300; ///< 0xC0
        static constexpr unsigned char SLIP_ESC      = 0333; ///< 0xDB
        static constexpr unsigned char SLIPX_NULL    = 0;    ///< 0 (nonstandard)
        static constexpr unsigned char SLIP_ESCEND   = 0334; ///< 0xDC
        static constexpr unsigned char SLIP_ESCESC   = 0335; ///< 0xDD
        static constexpr unsigned char SLIPX_ESCNULL = 0336; ///< 0xDE (nonstandard)
    };

    /**
     * @brief Base class for custom SLIP codes.
     *
     * targets C++11 - no variable or static data member at class scope
     * [(since C++14)](https://en.cppreference.com/w/cpp/language/variable_template)
     *
     * @tparam _CharT       unsigned char or char
     * @tparam _EndC        end character code \300
     * @tparam _EscC        escape character code \333
     * @tparam _EscEndC     escaped end character code \334
     * @tparam _EscEscC     escaped escape character code \334
     * @tparam _NullC       NULL character code \000
     * @tparam _EscNullC    escaped NULL character code. If set to anything other than zero, NULLs will be processed
     *
     */
    template <typename _CharT, unsigned char _EndC, unsigned char _EscC,
              unsigned char _EscEndC, unsigned char _EscEscC,
              unsigned char _NullC = 0, unsigned char _EscNullC = 0>
    struct slip_base {
        static constexpr _CharT end_code() noexcept { return (_CharT)_EndC; }         ///< end code
        static constexpr _CharT esc_code() noexcept { return (_CharT)_EscC; }         ///< escape code
        static constexpr _CharT null_code() noexcept { return (_CharT)_NullC; }       ///< NULL code
        static constexpr _CharT escend_code() noexcept { return (_CharT)_EscEndC; }   ///< escaped end code
        static constexpr _CharT escesc_code() noexcept { return (_CharT)_EscEscC; }   ///< escped escape code
        static constexpr _CharT escnull_code() noexcept { return (_CharT)_EscNullC; } ///< escaped NULL code if present

        /** Maximum number of special characters to escape while encoding */
        static constexpr int max_specials = 3;

        /** doest this encoder/decoder encode for the NULL character? */
        static constexpr bool encodes_null = (_EscNullC != 0);

        /**
         *  Number of special characters in this codec.
         *  Standard SLIP uses two (end and esc). SLIP+NULL adds a third code.
         */
        static constexpr int n_specials = (encodes_null ? 3 : 2);

     protected:
        /** An array of special characters to escape */
        static __ALWAYS_INLINE__ const _CharT* special_codes() noexcept {
            static constexpr _CharT specials[] = {end_code(), esc_code(), null_code()};
            return specials;
        }
        /** An array of special character escapes in the same order as special_codes(). */
        static __ALWAYS_INLINE__ const _CharT* escaped_codes() noexcept {
            static constexpr _CharT escapes[] = {escend_code(), escesc_code(), escnull_code()};
            return escapes;
        }

    #ifdef SLIP_UNROLL_LOOPS
        static __ALWAYS_INLINE__ int test_codes(const _CharT c, const _CharT* codes) {
            static_assert(max_specials == 3, "too many codecs to unroll. Recompile is -DSLIP_UNROLL_LOOPS=0");
            // a good compiler will notice the short-circuit constexpr evaluation
            // in the first term and eliminate the entire test if false
            if (n_specials > 2 && c == codes[2]) return 2;
            if (n_specials > 1 && c == codes[1]) return 1;
            if (n_specials > 0 && c == codes[0]) return 0;
            return -1;
        }
    };
    #else
        static __ALWAYS_INLINE__ int test_codes(const _CharT c, const _CharT* codes) {
            int i = n_specials;
            while (--i >= 0) {
                if (c == codes[i])
                    break;
            };
            return i;
        }
    };
    #endif

    /**
     * @brief Basic SLIP encoder.
     *
     * Automatically handles out-of-place encoding via copy or in-place encoding given
     * a buffer of sufficient size.
     *
     * @tparam _CharT       unsigned char or char
     * @tparam _EndC        end character code \300
     * @tparam _EscC        escape character code \333
     * @tparam _EscEndC     escaped end character code \334
     * @tparam _EscEscC     escaped escape character code \334
     * @tparam _EscNullC    escaped NULL character code \335 (non-standard)
     */
    template <typename _CharT, unsigned char _EndC, unsigned char _EscC,
              unsigned char _EscEndC, unsigned char _EscEscC,
              unsigned char _NullC = 0, unsigned char _EscNullC = 0>
    struct encoder_base : public slip_base<_CharT, _EndC, _EscC, _EscEndC, _EscEscC, _NullC, _EscNullC> {
        typedef slip_base<_CharT, _EndC, _EscC, _EscEndC, _EscEscC, _NullC, _EscNullC> base;
        using base::encodes_null;
        using base::end_code;
        using base::esc_code;
        using base::escaped_codes;
        using base::escend_code;
        using base::escesc_code;
        using base::escnull_code;
        using base::max_specials;
        using base::n_specials;
        using base::null_code;
        using base::special_codes;

        /**
         * @brief Pre-calculate the size after SLIP encoding.
         *
         * @param src       pointer to source buffer
         * @param srcsize   size of source buffer to parse
         * @return size_t   size needed to encode this buffer
         */
        static inline size_t encoded_size(const _CharT* src, size_t srcsize) noexcept {
            static const _CharT* specials = special_codes();
            const _CharT* buf_end         = src + srcsize;
            size_t nspecial               = 0;
            int isp;
            for (; src < buf_end; src++) {
                isp = base::test_codes(src[0], specials);
                if (isp >= 0) nspecial++;
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
        static inline size_t encode(_CharT* dest, size_t destsize, const _CharT* src,
                                    size_t srcsize) noexcept {
            static constexpr size_t BAD_DECODE = 0;
            static const _CharT* specials      = special_codes();
            static const _CharT* escapes       = escaped_codes();
            const _CharT* send                 = src + srcsize;
            _CharT* dstart                     = dest;
            _CharT* dend                       = dest + destsize;
            if (!dest || !src || destsize < srcsize + 1)
                return BAD_DECODE;
            if (dest <= src && src <= dend) { // sbuf somewhere in dbuf. So in-place
                // copy source to end of the dest_buf. memmove copies overlaps in reverse. Use
                // std::copy_backward if converting this function to iterators
                src  = (_CharT*)memmove(dest + destsize - srcsize, src, srcsize);
                send = src + srcsize;
            }
            int isp;

            while (src < send) {
                isp = base::test_codes(src[0], specials);
                if (isp < 0) { // regular character
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
     * @tparam _CharT       unsigned char or char
     * @tparam _EndC        end character code \300
     * @tparam _EscC        escape character code \333
     * @tparam _EscEndC     escaped end character code \334
     * @tparam _EscEscC     escaped escape character code \334
     * @tparam _NullC       NULL character code \000
     * @tparam _EscNullC    escaped NULL character code. If set to anything other than zero, NULLs will be processed
     */
    template <typename _CharT, unsigned char _EndC, unsigned char _EscC,
              unsigned char _EscEndC, unsigned char _EscEscC,
              unsigned char _NullC = 0, unsigned char _EscNullC = 0>
    struct decoder_base : public slip_base<_CharT, _EndC, _EscC, _EscEndC, _EscEscC, _NullC, _EscNullC> {
        typedef slip_base<_CharT, _EndC, _EscC, _EscEndC, _EscEscC, _NullC, _EscNullC> base;
        using base::encodes_null;
        using base::end_code;
        using base::esc_code;
        using base::escaped_codes;
        using base::escend_code;
        using base::escesc_code;
        using base::escnull_code;
        using base::max_specials;
        using base::n_specials;
        using base::null_code;
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
        static inline size_t decoded_size(const _CharT* src, size_t srcsize) noexcept {
            const _CharT* bufend = src + srcsize;
            size_t nescapes      = 0;
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
        static inline size_t decode(_CharT* dest, size_t destsize, const _CharT* src,
                                    size_t srcsize) noexcept {
            static constexpr size_t BAD_DECODE = 0;
            static const _CharT* specials      = special_codes();
            static const _CharT* escapes       = escaped_codes();
            const _CharT* send                 = src + srcsize;
            _CharT* dstart                     = dest;
            _CharT* dend                       = dest + destsize;
            if (!dest || !src || srcsize < 1 || destsize < 1) return BAD_DECODE;
            int isp;

            while (src < send) {
                if (src[0] == end_code()) return dest - dstart;
                if (src[0] != esc_code()) {              // regular character
                    if (dest >= dend) return BAD_DECODE; // not enough room for results
                    *(dest++) = *(src++);
                } else {
                    // check char after escape
                    src++;
                    if (src >= send || dest >= dend) return BAD_DECODE;
                    isp = base::test_codes(src[0], escapes);
                    if (isp < 0) return BAD_DECODE; // invalid escape code
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
     * @tparam _CharT    may be char or unsigned char (uint8_t)
     */
    template <typename _CharT>
    struct encoder : public encoder_base<_CharT,
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
     * @tparam _CharT    may be char or unsigned char (uint8_t)
     */
    template <typename _CharT>
    struct encoder_null : public encoder_base<_CharT,
                                              stdcodes::SLIP_END,
                                              stdcodes::SLIP_ESC,
                                              stdcodes::SLIP_ESCEND,
                                              stdcodes::SLIP_ESCESC,
                                              stdcodes::SLIPX_NULL,
                                              stdcodes::SLIPX_ESCNULL> {
    };

    /**
     * @brief Standard slip decoder.
     * @copydoc decoder_base
     *
     * @see "hrslip.h" in test-code for an example of a human-readable SLIP fornmat.
     *
     * @tparam _CharT    may be char or unsigned char (uint8_t)
     */
    template <typename _CharT>
    struct decoder : public decoder_base<_CharT,
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
     * @tparam _CharT    may be char or unsigned char (uint8_t)
     */
    template <typename _CharT>
    struct decoder_null : public decoder_base<_CharT,
                                              stdcodes::SLIP_END,
                                              stdcodes::SLIP_ESC,
                                              stdcodes::SLIP_ESCEND,
                                              stdcodes::SLIP_ESCESC,
                                              stdcodes::SLIPX_NULL,
                                              stdcodes::SLIPX_ESCNULL> {
    };
}

#endif // __SLIPINPLACE_H__