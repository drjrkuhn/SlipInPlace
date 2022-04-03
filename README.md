## SlipInPlace

An Arduino-compatible (C++11) Serial Line Internet Protocol ([SLIP](https://datatracker.ietf.org/doc/html/rfc1055)) encoder and decoder.

SlipInPlace can perform both in-place and out-of-place encoding and decoding of SLIP packets. This is useful for fixed size buffer.

### SLIP Encoding 
The normal SLIP protocol encodes a packet of bytes with unique `END` code at the end of a stream and a guarantee that the `END` code will **not** appear anywhere else in the packet. The standard SLIP code table is below.

| Char Code     | Octal   | Hex    | encoded as             |
|:--------------|--------:|-------:|------------------------|
| END           | `\300`  | `0xC0` | `\333\334`, `0xDBDC`   |
| ESC           | `\333`  | `0xDB` | `\333\335`, `0xDBDD`   |
| EscapedEND    | `\334`  | `0xDC` | _never alone_          |
| EscapedESC    | `\335`  | `0xDD` | _never alone_          |

Before adding the `END` code to the end of the stream, SLIP searches for any existing characters matching `END` and replaces them with a two-character `ESC-EscapedEND` codelet. It also has to guarantee that there are no spurious `ESC` codes in the original data. So it also searches for `ESC` codes in the original data and replaces them with a two-character `ESC-EscapedESC` codelet.

The protocol then adds an `END` code at the end of the packet that serial handlers can search for as the 'terminator' character in an incoming stream. The terminator (`END`) is guaranteed only to appear at the end of a packet.

Decoding works in reverse by shrinking the packet and replacing the escaped codes by their non-escaped originals.

### SLIP+NULL Encoding

This library has another SLIP+NULL encoder that replaces `END` and `ESC` as above, but _also_ searches for and replaces any `NULL` characters (`\0`) in the middle of the packet with a two-byte `ESC-EscapedNULL` sequence. That way we can guarantee no `NULL` bytes in the encoded packet. Serial communication handlers [^1] that only deal with C-strings (ending in `\0`) can safely handle data containing the byte zero after the packet has been encoded to eliminate any internal zeros. 

[^1]: I'm looking at you, [MicroManager](https://micro-manager.org/) device driver `GetSerialAnswer()`.


| Char Code     | Octal   | Hex    | encoded as             |
|:--------------|--------:|-------:|------------------------|
| NULL          |  `\000` | `0x00` | `\333\336`, `0xDBDE`   |
| EscapedNULL   |  `\336` | `0xDE` | _never alone_          |




### Standard encoding

The standard SLIP encoder and decoder are pre-defined. Two simple use-cases are below

Out-of-place encoding and decoding:
```C++
char ebuf[32];
char* source = "Lo\300rus"; // Note END in middle of string

// encoding
size_t esize = slip::encoder<char>::encode(ebuf,32,source,strlen(source));
//> ebuf == "Lo\333\334rus\300"; esize == 8; END(\300) at end

// decoding
char dbuf[32];
size_t dsize = slip::decoder<char>::decode(dbuf,32,ebuf,esize);
//> dbuf == "Lo\300rus"; dsize == 6;
std::string decoded(dbuf,dsize);
```

In-place encoding and decoding:
```C++
char buffer[32];
char* source = "Lo\300rus"; // Note END in middle of string

// encoding
strcpy(buffer, source);
size_t esize = slip::encoder<char>::encode(buffer,32,source,strlen(source));
//> buffer == "Lo\333\334rus\300"; esize == 8; END(\300) at end

// decoding
size_t dsize = slip::decoder<char>::decode(buffer,32,ebuf,esize);
//> buffer == "Lo\300rus"; dsize == 6;
std::string decoded(buffer,dsize);
```

You can declare a byte encoder or decoder that works with `unsigned chars` (`uint8_t`) by replacing `char` in the template parameter by `unsigned char`.

```C++
typedef slip::encoder<uint8_t> byte_encoder;
typedef slip::decoder<uint8_t> byte_decoder;
```

### Other encodings

Codes are defined at compile time using template parameters. For example, `\test\hrslip.h` defines a human-readable SLIP coded used for almost all algorithm and unit testing. 

```C++
typedef slip::encoder_base<char,'#','^','D','[','0','@'> encoder_hrnull;
typedef slip::decoder_base<char,'#','^','D','[','0','@'> decoder_hrnull;
```

The resulting codelets (in template order)

| Char Code     | Char  | encoded as        |
|:--------------|------:|-------------------|
| END           |  `#`  | `^D`              |
| ESC           |  `^`  | `^[`              |
| EscapedEND    |  `D`  | _never alone_     |
| EscapedESC    |  `[`  | _never alone_     |
| NULL          |  `0`  | `^@`              |
| EscapedNULL   |  `@`  | _never alone_     |

So for example

```C++
const size_t bsize = 10;
char buffer[32];
char* source = "Lo^#rus";

// encoding
size_t esize = encoder_hrnull::encode(buffer,32,source,strlen(source));
//> ebuf == "Lo^[^Drus#"; esize == 10;
// Original ESC^ is now ^[, END# is now ^D and END# only at terminus

// decoding
size_t dsize = decoder_hrnull::decode(buffer,32,buffer,esize);
//> dbuf == "Lo^#rus"; dsize == 7;
```

The `\examples\sip_encode` folder contains a similar human-readable encoding that prints encoded and decoded streams on the Arduino's serial port. 

(You can get a glimpse of how in-place _vs_ out-of-place encoding works by looking at the diagnostic buffer outputs.)

### Tests and Examples

The encoding and decoding libraries have unit tests of various scenarios. See the `\tests` directory for Unit tests.

See `\examples` for Arduino sample sketches.
