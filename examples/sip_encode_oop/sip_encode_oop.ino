#include <slipinplace.h>

typedef slip::encoder_base<char, '#', '^', 'D', '['> encoder_hr;
typedef slip::decoder_base<char, '#', '^', 'D', '['> decoder_hr;

String to_escaped_string(const char* buf, size_t size) {
    String out;
    while(size--) {
      char c = *(buf++);
      if (isPrintable(c)) {
        out.concat(c);
      } else if (c == '\0') {
        out.concat('\0');
      } else {
        String oct("000");
        oct += String((int)c,OCT);
        int len = oct.length();
        out += oct.substring(len-3,len);
      }
    }
    return out;
}
void test_encode_decode(String srcstr, size_t bufsize, bool inplace) {
    char buf[bufsize];
    const char* src           = srcstr.c_str();
    size_t srcsize       = srcstr.length();
    memset(buf, '.', bufsize);
    if (inplace) {
        memcpy(buf, src, srcsize);
        src = buf;
    }
    if (inplace) {
      Serial.println("___in-place encode and decode___");
    } else {
      Serial.println("___out-of-place encode and decode___");
    }
    Serial.print("src:  \"");    Serial.print(srcstr);    Serial.print("\"");
    size_t est_nencoded = encoder_hr::encoded_size(src, srcsize);
    if (est_nencoded > bufsize) {
        Serial.print("<<< !!Warning!! dsize not big enough to hold encoded string");
    }
    Serial.println();
    size_t ndecoded, nencoded;

    // encode
    nencoded = encoder_hr::encode(buf, bufsize, src, srcsize);
    String encstr(buf, nencoded);
    if (nencoded < bufsize) buf[nencoded] = '\0';
    Serial.print("encs: \"");    Serial.print(encstr);    Serial.println("\"");
    Serial.print("ebuf: [");    Serial.print(to_escaped_string(buf, bufsize));    Serial.println("]");

    // decode
    size_t est_ndecoded = decoder_hr::decoded_size(buf, nencoded);

    if (inplace) {
        ndecoded = decoder_hr::decode(buf, bufsize, buf, nencoded);
    } else {
        ndecoded = decoder_hr::decode(buf, bufsize, encstr.c_str(), encstr.length());
    }

    String decstr(buf, ndecoded);
    Serial.print("decs: \"");    Serial.print(decstr);    Serial.println("\"");
    Serial.print("dbuf: [");    Serial.print(to_escaped_string(buf, bufsize));    Serial.println("]");

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
    if (decstr != srcstr) {
        Serial.println("!!Warning!! decoded string did not match source string");
    }
    Serial.print(" bufsize:"); Serial.print(bufsize);
    cout Serial.print( " srcsize:"); Serial.print( srcsize);
    cout Serial.print( " est_nencoded:"); Serial.print( est_nencoded);
    cout Serial.print( " nencoded:"); Serial.print( nencoded);
    cout Serial.print( " est_ndecoded:"); Serial.print( est_ndecoded);
    cout Serial.print( " ndecoded:"); Serial.print( ndecoded);
    cout Serial.println();
    cout Serial.println();
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("=== sip_encode_oop ===");

}

void loop() {
    String out1 = "Lorus Ipsum";
    String out2 = "Favius## Rex\\ \\\\#\\##Aeturnum padre##";

    test_encode_decode(out1, 40, false);
    test_encode_decode(out2, 40, false);

    delay(1000);
}
