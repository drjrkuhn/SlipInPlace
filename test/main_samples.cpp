#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <SlipInPlace.h>
#include <sstream>
#include <string>
#include "sliputils.h"

using namespace std;

void oop_encoding_and_decoding() {
    string out;

    char ebuf[16]; 
    const char* source = "Lo\300rus"; // Note END in middle of string
    // encoding
    size_t esize = slip::encoder::encode(ebuf, 16, source, strlen(source));
    out = slip::escaped(ebuf,esize);
    cout << "// ebuf == " << out << "; esize == " << esize << ";" << endl;
    // ebuf == "Lo\333\334rus\300"; esize == 8;

    // decoding
    char dbuf[16]; 
    size_t dsize = slip::decoder::decode(dbuf, 16, ebuf, esize);
    out = slip::escaped(dbuf,dsize);
    cout << "// dbuf == " << out << "; dsize == " << dsize << ";" << endl;
    // dbuf == "Lo\300rus"; esize == 6;

    string final(dbuf, dsize);
    cout << "// final == " << slip::escaped(final) << ";" << endl;
    // final == "Lo\300rus";

}

void ip_encoding_and_decoding() {
    std::string out;

    char buffer[16]; 
    memset(buffer, '.', 16); 
    const char* source = "Lo\300rus"; // Note END in middle of string
    strcpy(buffer, source);

    // encoding
    size_t esize = slip::encoder::encode(buffer, 16, buffer, strlen(source));
    out = slip::escaped(buffer,esize);
    std::cout << "// buffer == " << out << "; esize == " << esize << ";" << endl;
    // buffer == "Lo\333\334rus\300"; esize == 8;

    // decoding
    size_t dsize = slip::decoder::decode(buffer, 16, buffer, esize);
    out = slip::escaped(buffer,dsize);
    std::cout << "// buffer == " << out << "; dsize == " << dsize << ";" << endl;
    // buffer == "Lo\300rus"; dsize == 6;

    string final(buffer, dsize);
    cout << "// final == " << slip::escaped(final) << ";" << endl;
    // final == "Lo\300rus";
}
int main() {
    using namespace std;

    cout << endl << endl << "## README.md sample code" << endl << endl;

    cout << "Out-of-place encoding and decoding:" << endl;
    oop_encoding_and_decoding();
    cout << endl << endl;
    cout << "In-place encoding and decoding:" << endl;
    ip_encoding_and_decoding();
    return 0;
}
