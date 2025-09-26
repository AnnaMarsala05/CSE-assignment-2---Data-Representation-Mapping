#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//CS 3503 - Assignment 2: Advanced Data Representation
/* Map a single octal digit ('0'..'7') to three bits */
static void map_octal_digit_to_bits(char d, char *out3)
{
    /* Ensure valid octal digit */
    int v = d - '0'; /* '7'->7 */
    out3[0] = ((v >> 2) & 1) ? '1' : '0';
    out3[1] = ((v >> 1) & 1) ? '1' : '0';
    out3[2] = ((v >> 0) & 1) ? '1' : '0';
}

/* Map a single hex digit (0-9, A-F, a-f) to four bits */
static void map_hex_digit_to_bits(char d, char *out4)
{
    int v;
    if (d >= '0' && d <= '9') v = d - '0';
    else if (d >= 'A' && d <= 'F') v = 10 + (d - 'A');
    else if (d >= 'a' && d <= 'f') v = 10 + (d - 'a');
    else v = 0; /* Defensive default; tests only provide valid digits */

    out4[0] = ((v >> 3) & 1) ? '1' : '0';
    out4[1] = ((v >> 2) & 1) ? '1' : '0';
    out4[2] = ((v >> 1) & 1) ? '1' : '0';
    out4[3] = ((v >> 0) & 1) ? '1' : '0';
}

/* Convert a 0..15 nibble value to a single uppercase hex char */
static char nibble_to_hex(unsigned v)
{
    static const char *HEX = "0123456789ABCDEF";
    return HEX[v & 0xF];
}

/* Write exactly 32 bits of value to out (plus NUL). out must be >= 33 bytes. */
static void to_32bit_binary(uint32_t value, char *out)
{
    for (int i = 31; i >= 0; --i) {
        out[31 - i] = ((value >> i) & 1U) ? '1' : '0';
    }
    out[32] = '\0';
}

/* Pad-left a binary string with '0' so its length is a multiple of group_size (3 or 4). */
static void left_pad_to_multiple(const char *bin_in, int group_size, char *bin_out, size_t out_cap)
{
    size_t len = strlen(bin_in);
    int rem = (int)(len % group_size);
    int pad = (rem == 0) ? 0 : (group_size - rem);

    size_t total = len + (size_t)pad;
    if (total + 1 > out_cap) {
        /* Truncate safely if ever needed (shouldn't happen with generous buffers). */
        total = out_cap - 1;
    }

    /* Fill pad zeros, then copy original */
    size_t pos = 0;
    for (int i = 0; i < pad && pos < out_cap - 1; ++i) bin_out[pos++] = '0';
    for (size_t i = 0; i < len && pos < out_cap - 1; ++i) bin_out[pos++] = bin_in[i];
    bin_out[pos] = '\0';
}

/* Strip leading '0' hex digits (but keep at least one character). */
static void strip_hex_leading_zeros(char *hex)
{
    size_t n = strlen(hex);
    size_t i = 0;
    while (i + 1 < n && hex[i] == '0') ++i;
    if (i > 0) memmove(hex, hex + i, n - i + 1);
}


void oct_to_bin(const char *oct, char *out);
void oct_to_hex(const char *oct, char *out);
void hex_to_bin(const char *hex, char *out);

void to_sign_magnitude(int32_t n, char *out32);
void to_ones_complement(int32_t n, char *out32);
void to_twos_complement(int32_t n, char *out32);


/* Convert octal string to binary by mapping each octal digit to exactly 3 bits.*/
void oct_to_bin(const char *oct, char *out)
{
    size_t len = strlen(oct);
    size_t pos = 0;
    for (size_t i = 0; i < len; ++i) {//processes each octal digit
        char triplet[3];
        map_octal_digit_to_bits(oct[i], triplet);//maps octal digit to 3 bits
        out[pos++] = triplet[0];
        out[pos++] = triplet[1];
        out[pos++] = triplet[2];
    }
    out[pos] = '\0';
}


void oct_to_hex(const char *oct, char *out)
{
    char bin[1024];// creates big enough buffer 
    char padded[1024];
    oct_to_bin(oct, bin);//calls oct to bin function
    left_pad_to_multiple(bin, 4, padded, sizeof(padded));

    size_t n = strlen(padded);
    size_t pos = 0;
    for (size_t i = 0; i + 3 < n; i += 4) {//processes 4 bits at a time
        unsigned v = 0;
        for (int k = 0; k < 4; ++k) {//maps 4 bits to a nibble
            v = (v << 1) | (unsigned)(padded[i + k] == '1');//moves bits to left and adds 1 if bit is 1
        }
        out[pos++] = nibble_to_hex(v); //maps nibble to hex
    }
    out[pos] = '\0';
    strip_hex_leading_zeros(out);//removes leading zeros
}

/* Convert hex string to binary by mapping each hex digit to exactly 4 bits. */
void hex_to_bin(const char *hex, char *out)
{
    size_t len = strlen(hex);//gets length of hex string
    size_t pos = 0;
    for (size_t i = 0; i < len; ++i) {//processes each hex digit
        char nibble[4];
        map_hex_digit_to_bits(hex[i], nibble);//maps hex digit to 4 bits
        out[pos++] = nibble[0];
        out[pos++] = nibble[1];
        out[pos++] = nibble[2];
        out[pos++] = nibble[3];
    }
    out[pos] = '\0';
}


void to_sign_magnitude(int32_t n, char *out32)
{
    uint32_t sign = (n < 0) ? 1U : 0U;
    uint32_t mag;
    if (n == INT32_MIN) {
        /* abs(INT32_MIN) overflows; clamp to 2^31 (but only 31 magnitude bits available -> becomes 0) */
        mag = (1U << 31); /* 0x80000000; lower 31 bits are 0 */
    } else {
        int32_t absn = (n < 0) ? -n : n;//gets absolute value
        mag = (uint32_t)absn;
    }
    uint32_t encoded = (sign << 31) | (mag & 0x7FFFFFFFU);//combines sign and magnitude
    to_32bit_binary(encoded, out32);
}

void to_ones_complement(int32_t n, char *out32)
{
    if (n >= 0) {
        to_32bit_binary((uint32_t)n, out32);//for positive numbers, just convert directly
    } else {
        uint32_t pos = (uint32_t)(-n); /* magnitude */
        uint32_t ones = ~pos;// invert bits for one's complement
        to_32bit_binary(ones, out32);//convert to binary
    }
}

void to_twos_complement(int32_t n, char *out32)
{
    uint32_t bits = (uint32_t)n;//interpret int32_t bits as uint32_t
    to_32bit_binary(bits, out32);//convert to binary
}
