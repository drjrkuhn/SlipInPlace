#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <SlipInPlace.h>
#include <sstream>
#include <string>

#if false
int main() {
    using namespace std;
    cout << "Special character table" << endl;
    uint8_t high[] = {0x00, 0x10, 0x70};
    for (uint8_t cl = 0; cl <= 0x0F; cl++) {
        for (int i = 0; i < 3; i++) {
            uint8_t ch = high[i];
            if (ch == 0x70 && cl < 0x0F)
                continue;
            uint8_t code = cl | ch;
            int hcode    = code;
            cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << hcode;
            cout << "\t'" << char(code) << "'\t";
        }
        cout << endl;
    }
    cout << endl;

    return 0;
}

#endif

#if !defined(__ALWAYS_INLINE__)
    #if defined(__GNUC__) && __GNUC__ > 3
        #define __ALWAYS_INLINE__ inline __attribute__((__always_inline__))
    #elif defined(_MSC_VER)
        #define __ALWAYS_INLINE__ __forceinline
    #else
        #define __ALWAYS_INLINE__ inline
    #endif
#endif

#if false
static constexpr int NCHARS = 3;
static char tofind[]{'a', 'b', 'c'};
#else
static constexpr int NCHARS = 2;
static char tofind[]{'a', 'b'};
#endif

__ALWAYS_INLINE__ int testchar_loop(const char c) {
    int f = NCHARS;
    while (--f >= 0) {
        if (c == tofind[f])
            break;
    };
    return f;
}

__ALWAYS_INLINE__ int testchar_unrolled(const char c) {
    if (NCHARS > 2 && c == tofind[2]) return 2;
    if (NCHARS > 1 && c == tofind[1]) return 1;
    if (NCHARS > 0 && c == tofind[0]) return 0;
    return -1;
}

#if true
int main() {
    using namespace std;

    std::string test(".acbd!");

    cout << "LOOP" << endl;
    for (int t = 0; t < test.length(); t++) {
        char tc = test[t];
        int f   = testchar_loop(tc);
        cout << '[' << t << "]:" << test[t];
        cout << " ? " << ((f < 0) ? '-' : tofind[f]);
        cout << endl;
    }
    cout << "UNROLLED" << endl;
    for (int t = 0; t < test.length(); t++) {
        char tc = test[t];
        int f   = testchar_unrolled(tc);
        cout << '[' << t << "]:" << test[t];
        cout << " ? " << ((f < 0) ? '-' : tofind[f]);
        cout << endl;
    }

    return 0;
}
#endif