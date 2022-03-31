#include "hrslip.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <slipinplace.h>
#include <sstream>

using encoder = hrslip::encoder_hr;
using decoder = hrslip::decoder_hr;

inline std::string to_escaped_string(char* buf, size_t size) {
    std::stringstream oss;
    std::for_each(buf, (buf + size), [&oss](char c) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (isprint(uc)) {
            oss << c;
        } else if (uc == '\0') {
            oss << "\\0";
        } else {
            oss << "\\" << std::setfill('0') << std::setw(3) << std::oct
                << static_cast<unsigned int>(uc);
        }
    });
    return oss.str();
}

inline void print_encode_results(char* buf, size_t bufsize, const char* src,
                                 size_t srcsize, bool inplace) {
    using namespace std;
    string srcstr = hrslip::hr_to_base<encoder>(src, srcsize);
    src           = srcstr.c_str();
    srcsize       = srcstr.length();
    memset(buf, '.', bufsize);
    if (inplace) {
        memcpy(buf, src, srcsize);
        src = buf;
    }
    cout << "src:  \"" << srcstr << "\"";
    size_t est_nencoded = encoder::encoded_size(src, srcsize);
    if (est_nencoded > bufsize) {
        cout << "<<< !!Warning!! dsize not big enough to hold encoded string";
    }
    cout << endl;
    size_t ndecoded, nencoded;

    // encode

    nencoded = encoder::encode(buf, bufsize, src, srcsize);
    string encstr(buf, nencoded);
    if (nencoded < bufsize) buf[nencoded] = '\0';
    cout << "encs: \"" << encstr << "\"" << endl;
    cout << "ebuf: [" << to_escaped_string(buf, bufsize) << "]" << endl;

    // decode
    size_t est_ndecoded = decoder::decoded_size(buf, nencoded);

    if (inplace) {
        ndecoded = decoder::decode(buf, bufsize, buf, nencoded);
    } else {
        ndecoded = decoder::decode(buf, bufsize, encstr.c_str(), encstr.length());
    }

    string decstr(buf, ndecoded);
    cout << "decs: \"" << decstr << "\"" << endl;
    cout << "dbuf: [" << to_escaped_string(buf, bufsize) << "]" << endl;

    if (est_nencoded != nencoded)
        cout << "!!Warning!! encoded size estimate:" << est_nencoded
             << " did not match encoded size:" << nencoded << endl;
    if (est_ndecoded != srcsize)
        cout << "!!Warning!! decoded size estimate:" << est_ndecoded
             << " did not match source size:" << srcsize << endl;
    if (decstr != srcstr)
        cout << "!!Warning!! decoded string did not match source string" << endl;
    cout << " bufsize:" << bufsize;
    cout << " srcsize:" << srcsize;
    cout << " est_nencoded:" << est_nencoded;
    cout << " nencoded:" << nencoded;
    cout << " est_ndecoded:" << est_ndecoded;
    cout << " ndecoded:" << ndecoded;
    cout << endl
         << endl;
}

int main() {
    using namespace std;
    const size_t bsize = 32;
    char buf[bsize];

    char out1[]    = "Lorus";    //"Lorus Ipsum";
    char out2[]    = "Lo^#r#us"; // Favius## Rex\\ \\\\#\\##Aeturnum padre##";
    char out3[]    = "Lorus##";
    size_t out1len = strlen(out1);
    size_t out2len = strlen(out2);
    size_t out3len = strlen(out3);

    cout << "===== slip out-of-place =====" << endl;
    if (out1len < bsize + 1)
        print_encode_results(buf, bsize, out1, out1len, false);
    if (out1len < bsize + 1)
        print_encode_results(buf, bsize, out2, out2len, false);
    if (out1len < bsize + 1)
        print_encode_results(buf, bsize, out3, out3len, false);

    cout << "===== slip in-place =====" << endl;
    if (out1len < bsize + 1)
        print_encode_results(buf, bsize, out1, out1len, true);
    if (out2len < bsize + 1)
        print_encode_results(buf, bsize, out2, out2len, true);
    if (out3len < bsize + 1)
        print_encode_results(buf, bsize, out3, out3len, true);

    return 0;
}
