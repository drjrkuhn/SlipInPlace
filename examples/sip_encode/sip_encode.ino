/*********************************************************************
 * \file   sip_encode_oop.ino
 * \brief  Arduino example of out-of-place and in-place SLIP encoding
 * 
 * Uses a human-readable SLIP encoding scheme instead of the standard
 * octal END and ESC codes.
 * 
 * \author Jeffrey Kuhn <jrkuhn@mit.edu>
 * \date   April 2022
 *********************************************************************/

#include <Arduino.h>
#include <SlipInPlace.h>

// human-readable encoders and decoders
//                               END  ESC ESCEND ESCESC
typedef slip::encoder_base<char, '#', '^', 'D', '['> encoder_hr;
typedef slip::decoder_base<char, '#', '^', 'D', '['> decoder_hr;

String to_escaped_string(const char* buf, size_t size) {
    String out;
    while (size--) {
        char c = *(buf++);
        if (isPrintable(c)) {
            out.concat(c);
        } else if (c == '\0') {
            out.concat("\\0");
        } else {
            String octstr("000");
            octstr += String((int)c, OCT);
            out += '\\';
            int len = octstr.length();
            out += octstr.substring(len - 3, len);
        }
    }
    return out;
}
void test_encode_decode(String srcstr, size_t bufsize, bool inplace) {
    char buf[bufsize];
    const char* src = srcstr.c_str();
    size_t srcsize  = srcstr.length();
    memset(buf, '.', bufsize);
    if (inplace) {
        memcpy(buf, src, srcsize);
        src = buf;
    }
    Serial.print("src:  \"");
    Serial.print(srcstr);
    Serial.print("\"");
    size_t est_nencoded = encoder_hr::encoded_size(src, srcsize);
    if (est_nencoded > bufsize) {
        Serial.print("<<< !!Warning!! dsize not big enough to hold encoded string");
    }
    Serial.println();
    size_t ndecoded, nencoded;

    // encode
    nencoded = encoder_hr::encode(buf, bufsize, src, srcsize);
    String encstr;
    if (nencoded < bufsize) buf[nencoded] = '\0';
    encstr.copy(buf, nencoded);
    if (nencoded < bufsize) buf[nencoded] = '\0';
    Serial.print("encs: \"");
    Serial.print(encstr);
    Serial.println("\"");
    Serial.print("ebuf: [");
    Serial.print(to_escaped_string(buf, bufsize));
    Serial.println("]");

    // decode
    size_t est_ndecoded = decoder_hr::decoded_size(buf, nencoded);

    if (inplace) {
        ndecoded = decoder_hr::decode(buf, bufsize, buf, nencoded);
    } else {
        ndecoded = decoder_hr::decode(buf, bufsize, encstr.c_str(), encstr.length());
    }

    String decstr;
    if (ndecoded < bufsize) buf[ndecoded] = '\0';
    decstr.copy(buf, ndecoded);
    Serial.print("decs: \"");
    Serial.print(decstr);
    Serial.println("\"");
    Serial.print("dbuf: [");
    Serial.print(to_escaped_string(buf, bufsize));
    Serial.println("]");

    if (est_nencoded != nencoded) {
        Serial.print("!!Warning!! encoded size estimate:");
        Serial.print(est_nencoded);
        Serial.print(" did not match encoded size:");
        Serial.println(nencoded);
    }
    if (est_ndecoded != srcsize) {
        Serial.print("!!Warning!! decoded size estimate:");
        Serial.print(est_ndecoded);
        Serial.print(" did not match source size:");
        Serial.println(srcsize);
    }
    if (decstr.compareTo(srcstr) != 0) {
        Serial.println("!!Warning!! decoded string did not match source string");
    }
    Serial.print("       bufsize:");
    Serial.print(bufsize);
    Serial.print(" srcsize:");
    Serial.print(srcsize);
    Serial.print(" est_nencoded:");
    Serial.print(est_nencoded);
    Serial.print(" nencoded:");
    Serial.print(nencoded);
    Serial.print(" est_ndecoded:");
    Serial.print(est_ndecoded);
    Serial.print(" ndecoded:");
    Serial.print(ndecoded);
    Serial.println();
}

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
}

void loop() {
    Serial.println("OUT-OF-PLACE ENCODE AND DECODE");
    Serial.println("========================================");
    test_encode_decode(String(""), 40, false);
    test_encode_decode(String("Lorus"), 40, false);
    test_encode_decode(String("Lorus#"), 40, false);
    test_encode_decode(String("Lo^#rus"), 40, false);
    test_encode_decode(String("Lorus^"), 40, false);
    test_encode_decode(String("Lorus#"), 40, false);
    test_encode_decode(String("Lorus^##"), 40, false);
    test_encode_decode(String("Lo^r#us i^ps#um#"), 40, false);
    Serial.println("      ____exact fit_____");
    test_encode_decode(String("Lor^us##"), 12, false);
    Serial.println("      ___will overrun___");
    test_encode_decode(String("Lor^us##"), 11, false);

    Serial.println("IN-PLACE ENCODE AND DECODE");
    Serial.println("========================================");
    test_encode_decode(String(""), 40, true);
    test_encode_decode(String("Lorus"), 40, true);
    test_encode_decode(String("Lorus#"), 40, true);
    test_encode_decode(String("Lo^#rus"), 40, true);
    test_encode_decode(String("Lorus^"), 40, true);
    test_encode_decode(String("Lorus#"), 40, true);
    test_encode_decode(String("Lorus^##"), 40, true);
    test_encode_decode(String("Lo^r#us i^ps#um#"), 40, true);
    Serial.println("      ____exact fit_____");
    test_encode_decode(String("Lor^us##"), 12, true);
    Serial.println("      ___will overrun___");
    test_encode_decode(String("Lor^us##"), 11, true);

    delay(10000);
}
