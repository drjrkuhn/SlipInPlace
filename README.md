## SlipInPlace

An Arduino-compatible (C++11) Serial Line Internet Protocol ([SLIP](https://datatracker.ietf.org/doc/html/rfc1055)) encoder and decoder.

SlipInPlace can perform both in-place and out-of-place encoding and decoding of SLIP packets. This is useful for fixed size buffer.

### SLIP Encoding 
The normal SLIP protocol encodes a packet of bytes with unique `END` code at the end of a stream and a guarantee that the end code will **not** appear anywhere else in the packet. The standard SLIP code table is below.

| Char Code     | Octal   | Hex    | encoded as             |
|:--------------|--------:|-------:|------------------------|
| END           | `\300`  | `0xC0` | `\333\334`, `0xDBDC`   |
| ESC           | `\333`  | `0xDB` | `\333\335`, `0xDBDD`   |
| EscapedEND    | `\334`  | `0xDC` | _never alone_          |
| EscapedESC    | `\335`  | `0xDD` | _never alone_          |

Before adding the `END` code to the end of the stream, SLIP searches for any existing characters matching `END` and replaces them with a two-character `ESC-EscapedEND` codelet. It also has to guarantee that there are no spurious `ESC` codes in the original data, so it also searches for `ESC` codes in the original data and replaces them with a two-character `ESC-EscapedESC` codelet.

The protocol then adds an END code at the end of the packet that serial handlers can search for as the 'terminator' character in an incoming stream. The terminator (END) is guaranteed only to appear at the end of a packet.

### SLIP+NULL Encoding

This library has another SLIP+NULL encoder that replaces `END` and `ESC` as above, but _also_ searches for and replaces any `NULL` characters (`\0`) in the middle of the packet with a two-byte `ESC-EscapedNULL` sequence. That way we can guarantee no `NULL` bytes in the encoded packet. Serial communication handlers [^1] that only deal with C-strings (ending in `\0`) can safely handle data containing the byte zero after encoding. 

| Char Code     | Octal   | Hex    | encoded as             |
|:--------------|--------:|-------:|------------------------|
| NULL          |  `\000` | `0x00` | `\333\336`, `0xDBDE`   |
| EscapedNULL   |  `\336` | `0xDE` | _never alone_          |

[^1]: I'm looking at you, [MicroManager](https://micro-manager.org/) device driver `GetSerialAnswer()`.

