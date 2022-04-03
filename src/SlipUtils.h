#pragma once

#ifndef __SLIPUTILS_H__
    #define __SLIPUTILS_H__

    #include <iomanip>
    #include <string>
    #include <ostream>
    #include <sstream>
    #include <algorithm>

namespace slip {
    inline std::string escaped(const char* buf, size_t size, const char* brackets = "\"\"") {
        std::stringstream oss;
        size_t br_len = brackets ? strlen(brackets) : 0;
        if (br_len > 0) {
            oss << brackets[0];
        }
        static char c_escapes[] = {
            '\0', '0', // NULL
            '\'', '\'', // single quote
            '\"', '"', // double quote
            '\?', '?', // question mark
            '\\', '\\', // backslash
            '\a', 'a', // audible bell
            '\b', 'b', // backspace
            '\f', 'f', // formfeed
            '\n', 'n', // line feed
            '\r', 'r', // carriage return
            '\t', 't', // horizontal tab
            '\v', 'v' // vertical tab
        };
        std::for_each(buf, (buf + size), [&oss,&brackets](char c) {
            unsigned char uc = static_cast<unsigned char>(c);
            int c_esc;
            for (c_esc = 0; c_esc < sizeof(c_escapes); c_esc += 2) {
                if (uc == c_escapes[c_esc])
                    break;
            }
            if (c_esc < sizeof(c_escapes)) {
                char sc = c_escapes[c_esc+1];
                oss << '\\' << sc;
            } else if (isprint(uc)) {
                oss << c;
            } else {
                oss << "\\" << std::setfill('0') << std::setw(3) << std::oct
                    << static_cast<unsigned int>(uc);
            }
        });
        if (br_len > 0) {
            if (br_len > 1)
                oss << brackets[1];
            else
                oss << brackets[0];
        }
        return oss.str();
    }

    inline std::string escaped(std::string& src, const char* brackets = "\"\"") {
        return escaped(src.c_str(), src.length(), brackets);
    }

}; // namespace slip

#endif // __SLIPUTILS_H__