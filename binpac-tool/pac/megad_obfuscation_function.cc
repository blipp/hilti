
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#define ntohll(x) (((uint64_t)(ntohl((uint32_t)(x & 0xffffffffULL))) << 32) | \
  		  (((uint64_t)(ntohl((uint32_t)(x >> 32)))) & 0xffffffffULL))

#define htonll(x) ntohll(x)

/*
uint64_t ntohll(uint64_t x) {
  uint64_t lw = (uint64_t)(ntohl((uint32_t)(x & 0xffffffffULL)));
  uint64_t hw = (uint64_t)(ntohl((uint32_t)(x >> 32)));
  return (lw << 32) | (hw & 0xffffffffULL);
}
*/

extern "C" {

/* Some tests */
uint64_t encode_test =  0x0000000000000001ULL; // Result: 0xe5b901117f37fddb
uint64_t decode_test =  0x957f1369733e3bf0ULL; // Result: 0x68aa575601000001

/* Keys and tables used by the function */
uint8_t decode_key[] = {
  0x00, 0x34, 0x26, 0x0b, 0x0a, 0x28, 0x38, 0x3c, 0x35, 0x10, 0x26, 0x0b, 0x08, 0x28, 0x38, 0x3c,
  0x10, 0x03, 0x2e, 0x0a, 0x22, 0x24, 0x3a, 0x34, 0x2c, 0x14, 0x2c, 0x1a, 0x28, 0x00, 0x32, 0x36,
  0x30, 0x05, 0x2d, 0x32, 0x00, 0x36, 0x32, 0x16, 0x08, 0x11, 0x0d, 0x32, 0x38, 0x02, 0x36, 0x07,
  0x38, 0x0c, 0x1d, 0x36, 0x14, 0x30, 0x26, 0x07, 0x00, 0x29, 0x19, 0x34, 0x21, 0x12, 0x27, 0x07,
  0x00, 0x05, 0x19, 0x35, 0x31, 0x19, 0x27, 0x07, 0x2a, 0x0a, 0x19, 0x35, 0x04, 0x03, 0x0f, 0x0b,
  0x02, 0x21, 0x1b, 0x35, 0x15, 0x18, 0x0d, 0x2b, 0x09, 0x2a, 0x33, 0x2d, 0x00, 0x0d, 0x0d, 0x2b,
  0x03, 0x02, 0x32, 0x2d, 0x0f, 0x10, 0x0d, 0x39, 0x04, 0x20, 0x36, 0x0d, 0x02, 0x2d, 0x19, 0x39,
  0x07, 0x02, 0x36, 0x0b, 0x0c, 0x04, 0x19, 0x38, 0x02, 0x32, 0x26, 0x0b, 0x2a, 0x04, 0x39, 0x38,
};

uint8_t encode_key[] = {
  0x02, 0x32, 0x26, 0x0b, 0x2a, 0x04, 0x39, 0x38, 0x07, 0x02, 0x36, 0x0b, 0x0c, 0x04, 0x19, 0x38,
  0x04, 0x20, 0x36, 0x0d, 0x02, 0x2d, 0x19, 0x39, 0x03, 0x02, 0x32, 0x2d, 0x0f, 0x10, 0x0d, 0x39,
  0x09, 0x2a, 0x33, 0x2d, 0x00, 0x0d, 0x0d, 0x2b, 0x02, 0x21, 0x1b, 0x35, 0x15, 0x18, 0x0d, 0x2b,
  0x2a, 0x0a, 0x19, 0x35, 0x04, 0x03, 0x0f, 0x0b, 0x00, 0x05, 0x19, 0x35, 0x31, 0x19, 0x27, 0x07,
  0x00, 0x29, 0x19, 0x34, 0x21, 0x12, 0x27, 0x07, 0x38, 0x0c, 0x1d, 0x36, 0x14, 0x30, 0x26, 0x07,
  0x08, 0x11, 0x0d, 0x32, 0x38, 0x02, 0x36, 0x07, 0x30, 0x05, 0x2d, 0x32, 0x00, 0x36, 0x32, 0x16,
  0x2c, 0x14, 0x2c, 0x1a, 0x28, 0x00, 0x32, 0x36, 0x10, 0x03, 0x2e, 0x0a, 0x22, 0x24, 0x3a, 0x34,
  0x35, 0x10, 0x26, 0x0b, 0x08, 0x28, 0x38, 0x3c, 0x00, 0x34, 0x26, 0x0b, 0x0a, 0x28, 0x38, 0x3c,
};

uint8_t tbl_424868[256] = {
  0x00, 0x04, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x04, 0x01, 0x01,
  0x04, 0x00, 0x01, 0x01, 0x04, 0x04, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x01, 0x01, 0x04, 0x04, 0x01, 0x01, 0x00, 0x04, 0x00, 0x00,
  0x04, 0x04, 0x00, 0x01, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00,
  0x04, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01, 0x00,
  0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x04, 0x04, 0x00, 0x01,
  0x04, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x01, 0x04, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x01, 0x00, 0x04, 0x04, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
  0x00, 0x04, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00,
  0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x04, 0x00, 0x00, 0x01,
  0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x01, 0x04, 0x04, 0x01, 0x00,
  0x04, 0x04, 0x01, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x04, 0x04, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x01, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x01,
  0x04, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x01,
};

uint8_t tbl_424968[256] = {
  0x20, 0x80, 0x10, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x20, 0x80, 0x10, 0x00,
  0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x10, 0x80, 0x20, 0x80, 0x00, 0x80,
  0x20, 0x00, 0x00, 0x80, 0x20, 0x80, 0x10, 0x80, 0x00, 0x80, 0x10, 0x80, 0x00, 0x00, 0x00, 0x80,
  0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x10, 0x80,
  0x00, 0x80, 0x10, 0x00, 0x20, 0x00, 0x10, 0x00, 0x20, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x20, 0x80, 0x10, 0x00, 0x00, 0x00, 0x10, 0x80,
  0x20, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x00,
  0x20, 0x80, 0x00, 0x00, 0x00, 0x80, 0x10, 0x80, 0x00, 0x00, 0x10, 0x80, 0x20, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 0x10, 0x00, 0x20, 0x00, 0x10, 0x80, 0x00, 0x00, 0x10, 0x00,
  0x20, 0x80, 0x00, 0x80, 0x00, 0x00, 0x10, 0x80, 0x00, 0x80, 0x10, 0x80, 0x00, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x10, 0x80, 0x00, 0x80, 0x00, 0x80, 0x20, 0x00, 0x00, 0x00, 0x20, 0x80, 0x10, 0x80,
  0x20, 0x80, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x20, 0x80, 0x00, 0x00, 0x00, 0x80, 0x10, 0x80, 0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x80,
  0x20, 0x00, 0x10, 0x00, 0x20, 0x80, 0x00, 0x80, 0x20, 0x00, 0x00, 0x80, 0x20, 0x00, 0x10, 0x00,
  0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x20, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x20, 0x00, 0x10, 0x80, 0x20, 0x80, 0x10, 0x80, 0x00, 0x80, 0x10, 0x00,
};

uint8_t tbl_424a68[256] = {
  0x08, 0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x08,
  0x00, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x02, 0x02, 0x00, 0x00, 0x02, 0x00, 0x08,
  0x08, 0x00, 0x02, 0x00, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00,
  0x08, 0x02, 0x02, 0x08, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x08, 0x08, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x08, 0x00, 0x02, 0x00, 0x00,
  0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x02, 0x08, 0x08, 0x00, 0x02, 0x08, 0x08, 0x02, 0x02, 0x00,
  0x08, 0x02, 0x00, 0x08, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x08, 0x02, 0x00, 0x08,
  0x08, 0x00, 0x00, 0x00, 0x08, 0x02, 0x02, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x02, 0x02, 0x08, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x02, 0x00, 0x08, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x02, 0x02, 0x08, 0x00, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x08, 0x02, 0x02, 0x08, 0x00, 0x02, 0x00, 0x08,
  0x08, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x08,
  0x08, 0x02, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x08,
  0x08, 0x00, 0x00, 0x00, 0x08, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x00, 0x08, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x02, 0x08, 0x08, 0x02, 0x00, 0x08, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x08,
  0x08, 0x02, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x08, 0x00, 0x02, 0x02, 0x00,
};

uint8_t tbl_424b68[256] = {
  0x01, 0x20, 0x80, 0x00, 0x81, 0x20, 0x00, 0x00, 0x81, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x80, 0x20, 0x80, 0x00, 0x81, 0x00, 0x80, 0x00, 0x01, 0x00, 0x80, 0x00, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 0x00, 0x00, 0x20, 0x80, 0x00, 0x81, 0x20, 0x80, 0x00,
  0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x01, 0x00, 0x80, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x01, 0x20, 0x80, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x01, 0x20, 0x00, 0x00, 0x80, 0x20, 0x00, 0x00,
  0x81, 0x00, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x20, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00,
  0x00, 0x20, 0x00, 0x00, 0x80, 0x20, 0x80, 0x00, 0x81, 0x20, 0x80, 0x00, 0x81, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x80, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0x20, 0x80, 0x00, 0x81, 0x20, 0x80, 0x00,
  0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 0x00,
  0x80, 0x20, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x81, 0x00, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x20, 0x80, 0x00, 0x81, 0x20, 0x00, 0x00, 0x81, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x81, 0x20, 0x80, 0x00, 0x81, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
  0x01, 0x00, 0x80, 0x00, 0x01, 0x20, 0x00, 0x00, 0x80, 0x20, 0x80, 0x00, 0x81, 0x00, 0x80, 0x00,
  0x01, 0x20, 0x00, 0x00, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x01, 0x20, 0x80, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x80, 0x20, 0x80, 0x00,
};

uint8_t tbl_424c68[256] = {
  0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x02, 0x00, 0x00, 0x08, 0x02, 0x00, 0x01, 0x00, 0x42,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x02,
  0x00, 0x01, 0x08, 0x40, 0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x08, 0x40,
  0x00, 0x01, 0x00, 0x42, 0x00, 0x00, 0x08, 0x42, 0x00, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x40,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x40, 0x00, 0x01, 0x08, 0x42, 0x00, 0x01, 0x08, 0x42, 0x00, 0x01, 0x00, 0x02,
  0x00, 0x00, 0x08, 0x42, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42,
  0x00, 0x01, 0x08, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x42, 0x00, 0x01, 0x08, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x02, 0x00, 0x01, 0x00, 0x42, 0x00, 0x01, 0x08, 0x40,
  0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x42, 0x00, 0x01, 0x08, 0x02,
  0x00, 0x01, 0x08, 0x40, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x08, 0x42,
  0x00, 0x01, 0x08, 0x42, 0x00, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x01, 0x08, 0x42,
  0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, 0x42,
  0x00, 0x01, 0x08, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x01, 0x08, 0x02, 0x00, 0x01, 0x00, 0x40,
};

uint8_t tbl_424d68[256] = {
  0x10, 0x00, 0x00, 0x20, 0x00, 0x00, 0x40, 0x20, 0x00, 0x40, 0x00, 0x00, 0x10, 0x40, 0x40, 0x20,
  0x00, 0x00, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x10, 0x40, 0x40, 0x20, 0x00, 0x00, 0x40, 0x00,
  0x00, 0x40, 0x00, 0x20, 0x10, 0x40, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x10, 0x00, 0x00, 0x20,
  0x10, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x10, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x40, 0x00, 0x10, 0x40, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00,
  0x00, 0x40, 0x40, 0x00, 0x10, 0x40, 0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x40, 0x20,
  0x10, 0x00, 0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0x20,
  0x10, 0x40, 0x00, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0x20, 0x00, 0x00, 0x00, 0x20,
  0x00, 0x40, 0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x40, 0x20, 0x00, 0x40, 0x40, 0x00,
  0x10, 0x40, 0x40, 0x20, 0x00, 0x00, 0x40, 0x00, 0x10, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x20,
  0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x10, 0x40, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x20, 0x10, 0x40, 0x40, 0x20, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00, 0x40, 0x20,
  0x10, 0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x40, 0x20,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x10, 0x40, 0x40, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x40, 0x00, 0x10, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x40, 0x40, 0x20, 0x00, 0x00, 0x00, 0x20, 0x10, 0x00, 0x40, 0x00, 0x10, 0x40, 0x00, 0x20,
};

uint8_t tbl_424e68[256] = {
  0x00, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x04, 0x02, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x02, 0x08, 0x00, 0x04, 0x02, 0x08, 0x20, 0x00, 0x00, 0x08, 0x20, 0x04,
  0x02, 0x08, 0x20, 0x04, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x20, 0x04, 0x02, 0x08, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x04, 0x02, 0x08, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x00, 0x08, 0x00, 0x04,
  0x02, 0x00, 0x00, 0x04, 0x00, 0x00, 0x20, 0x04, 0x00, 0x08, 0x20, 0x04, 0x02, 0x00, 0x20, 0x00,
  0x00, 0x00, 0x20, 0x04, 0x00, 0x08, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00, 0x02, 0x08, 0x20, 0x04,
  0x00, 0x08, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x20, 0x00,
  0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x02, 0x08, 0x00, 0x04,
  0x02, 0x08, 0x00, 0x04, 0x02, 0x00, 0x20, 0x04, 0x02, 0x00, 0x20, 0x04, 0x02, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00,
  0x00, 0x08, 0x20, 0x04, 0x02, 0x08, 0x00, 0x00, 0x02, 0x08, 0x20, 0x00, 0x00, 0x08, 0x20, 0x04,
  0x02, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x02, 0x08, 0x20, 0x04, 0x00, 0x00, 0x20, 0x04,
  0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x08, 0x20, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x08, 0x20, 0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x08, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00,
};

uint8_t tbl_424f68[256] = {
  0x40, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x40, 0x10, 0x04, 0x10,
  0x00, 0x00, 0x00, 0x10, 0x40, 0x10, 0x00, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x40, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x10, 0x40, 0x10, 0x04, 0x10, 0x00, 0x10, 0x04, 0x00,
  0x00, 0x10, 0x04, 0x10, 0x40, 0x10, 0x04, 0x00, 0x00, 0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x04, 0x10, 0x40, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x40, 0x10, 0x00, 0x00,
  0x00, 0x10, 0x04, 0x00, 0x40, 0x00, 0x04, 0x00, 0x40, 0x00, 0x04, 0x10, 0x00, 0x10, 0x04, 0x10,
  0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x04, 0x10,
  0x40, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x40, 0x10, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00,
  0x40, 0x10, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x10, 0x04, 0x10, 0x00, 0x10, 0x00, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x04, 0x10, 0x00, 0x10, 0x00, 0x00, 0x40, 0x10, 0x04, 0x00,
  0x00, 0x10, 0x00, 0x10, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x04, 0x10,
  0x40, 0x00, 0x04, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x40, 0x10, 0x00, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x40, 0x10, 0x04, 0x10, 0x40, 0x00, 0x04, 0x00, 0x40, 0x00, 0x00, 0x10,
  0x00, 0x00, 0x04, 0x10, 0x00, 0x10, 0x00, 0x10, 0x40, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
  0x40, 0x10, 0x04, 0x10, 0x00, 0x10, 0x04, 0x00, 0x00, 0x10, 0x04, 0x00, 0x40, 0x10, 0x00, 0x00,
  0x40, 0x10, 0x00, 0x00, 0x40, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x04, 0x10,
};

void megadfun( uint8_t *key, uint64_t *src, uint64_t *src2 )
{
__asm__ __volatile__ (
"push %%ecx\n\t"
"push %%ecx\n\t"
"push %%ebx\n\t"
"push %%esi\n\t"
"push %%edi\n\t"
"mov 0xc(%%ebp),%%eax\n\t"
"add $0x0,%%eax\n\t"
"mov (%%eax),%%eax\n\t"
"bswap %%eax\n\t"
"mov %%eax,-0x8(%%ebp)\n\t"
"mov 0xc(%%ebp),%%eax\n\t"
"add $0x4,%%eax\n\t"
"mov (%%eax),%%eax\n\t"
"bswap %%eax\n\t"
"mov %%eax,-0x4(%%ebp)\n\t"
"mov -0x8(%%ebp),%%ecx\n\t"
"mov -0x4(%%ebp),%%esi\n\t"
"mov %%ecx,%%eax\n\t"
"shr $0x4,%%eax\n\t"
"xor %%esi,%%eax\n\t"
"and $0xf0f0f0f,%%eax\n\t"
"xor %%eax,%%esi\n\t"
"shl $0x4,%%eax\n\t"
"xor %%eax,%%ecx\n\t"
"mov %%ecx,%%eax\n\t"
"shr $0x10,%%eax\n\t"
"xor %%esi,%%eax\n\t"
"and $0xffff,%%eax\n\t"
"xor %%eax,%%esi\n\t"
"shl $0x10,%%eax\n\t"
"xor %%eax,%%ecx\n\t"
"mov %%esi,%%eax\n\t"
"shr $0x2,%%eax\n\t"
"xor %%ecx,%%eax\n\t"
"and $0x33333333,%%eax\n\t"
"xor %%eax,%%ecx\n\t"
"shl $0x2,%%eax\n\t"
"xor %%eax,%%esi\n\t"
"mov %%esi,%%edx\n\t"
"shr $0x8,%%edx\n\t"
"xor %%ecx,%%edx\n\t"
"and $0xff00ff,%%edx\n\t"
"xor %%edx,%%ecx\n\t"
"shl $0x8,%%edx\n\t"
"xor %%esi,%%edx\n\t"
"rol %%edx\n\t"
"mov %%edx,%%esi\n\t"
"xor %%ecx,%%esi\n\t"
"and $0xaaaaaaaa,%%esi\n\t"
"xor %%esi,%%edx\n\t"
"xor %%ecx,%%esi\n\t"
"mov 0x8(%%ebp),%%ecx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"push $0x3f\n\t"
"pop %%eax\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"rol %%esi\n\t"
"xor %%ebx,%%esi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%edx\n\t"
"mov %%esi,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"xor %%ebx,%%esi\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%edx\n\t"
"mov %%esi,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"mov %%edi,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%edx\n\t"
"mov %%esi,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"mov %%esi,%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"add $0x4,%%ecx\n\t"
"and %%eax,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"mov %%esi,%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"shr $0x18,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"and %%eax,%%edi\n\t"
"add $0x4,%%ecx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"mov %%esi,%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"add $0x4,%%ecx\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"mov %%esi,%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%edx\n\t"
"xor %%edx,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"add $0x4,%%ecx\n\t"
"xor %%ebx,%%esi\n\t"
"mov %%edx,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor (%%ecx),%%edi\n\t"
"add $0x4,%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x10,%%edi\n\t"
"shr $0x18,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424868(,%%ebx,4),%%ebx\n\t"
"xor _tbl_424a68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424c68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424e68(,%%edi,4),%%ebx\n\t"
"mov (%%ecx),%%edi\n\t"
"xor %%ebx,%%esi\n\t"
"xor %%esi,%%edi\n\t"
"mov %%edi,%%ebx\n\t"
"mov %%edi,0xc(%%ebp)\n\t"
"shr $0x18,%%ebx\n\t"
"shr $0x10,%%edi\n\t"
"and %%eax,%%ebx\n\t"
"mov _tbl_424968(,%%ebx,4),%%ebx\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424b68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"shr $0x8,%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424d68(,%%edi,4),%%ebx\n\t"
"mov 0xc(%%ebp),%%edi\n\t"
"and %%eax,%%edi\n\t"
"xor _tbl_424f68(,%%edi,4),%%ebx\n\t"
"xor %%ebx,%%edx\n\t"
"mov %%esi,%%edi\n\t"
"ror $0x4,%%edi\n\t"
"xor 0x4(%%ecx),%%edi\n\t"
"mov %%edi,%%ecx\n\t"
"shr $0x18,%%ecx\n\t"
"and %%eax,%%ecx\n\t"
"mov _tbl_424868(,%%ecx,4),%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"shr $0x10,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"xor _tbl_424a68(,%%ebx,4),%%ecx\n\t"
"mov %%edi,%%ebx\n\t"
"and %%eax,%%edi\n\t"
"shr $0x8,%%ebx\n\t"
"and %%eax,%%ebx\n\t"
"xor _tbl_424c68(,%%ebx,4),%%ecx\n\t"
"xor _tbl_424e68(,%%edi,4),%%ecx\n\t"
"xor %%edx,%%ecx\n\t"
"ror %%ecx\n\t"
"mov %%ecx,%%eax\n\t"
"xor %%esi,%%eax\n\t"
"and $0xaaaaaaaa,%%eax\n\t"
"xor %%eax,%%ecx\n\t"
"xor %%esi,%%eax\n\t"
"ror %%eax\n\t"
"mov %%eax,%%edx\n\t"
"shr $0x8,%%edx\n\t"
"xor %%ecx,%%edx\n\t"
"and $0xff00ff,%%edx\n\t"
"xor %%edx,%%ecx\n\t"
"shl $0x8,%%edx\n\t"
"xor %%edx,%%eax\n\t"
"mov %%eax,%%edx\n\t"
"shr $0x2,%%edx\n\t"
"xor %%ecx,%%edx\n\t"
"and $0x33333333,%%edx\n\t"
"xor %%edx,%%ecx\n\t"
"shl $0x2,%%edx\n\t"
"xor %%edx,%%eax\n\t"
"mov %%ecx,%%edx\n\t"
"shr $0x10,%%edx\n\t"
"xor %%eax,%%edx\n\t"
"and $0xffff,%%edx\n\t"
"xor %%edx,%%eax\n\t"
"shl $0x10,%%edx\n\t"
"xor %%edx,%%ecx\n\t"
"mov %%ecx,%%edx\n\t"
"shr $0x4,%%edx\n\t"
"xor %%eax,%%edx\n\t"
"and $0xf0f0f0f,%%edx\n\t"
"xor %%edx,%%eax\n\t"
"shl $0x4,%%edx\n\t"
"xor %%edx,%%ecx\n\t"
"mov %%eax,-0x8(%%ebp)\n\t"
"mov %%ecx,-0x4(%%ebp)\n\t"
"mov -0x4(%%ebp),%%eax\n\t"
"bswap %%eax\n\t"
"push %%eax\n\t"
"mov 0x10(%%ebp),%%eax\n\t"
"add $0x0,%%eax\n\t"
"popl (%%eax)\n\t"
"mov -0x8(%%ebp),%%eax\n\t"
"bswap %%eax\n\t"
"push %%eax\n\t"
"mov 0x10(%%ebp),%%eax\n\t"
"add $0x4,%%eax\n\t"
"popl (%%eax)\n\t"
"pop %%edi\n\t"
"pop %%esi\n\t"
"pop %%ebx\n\t"
//"leave\n\t"
//"ret $0xc"
"add $0x8,%%esp"
: /* no output */
: "m" (key), "m" (src), "m" (src2) /* inputs */
: "%eax", "%ecx", "%edx", "memory" /* clobbered */
);
}

}
    
#if 0
void usage( char *progname ) {
  fprintf (stdout,"%s <-n> <-N> <-d hex64string> <-e hex64string>\n",
    progname);
  fprintf (stdout,"\t hex64string: a uint64_t in hexadecimal. "
    "Eg: ca1d7b5e7cbda928\n");
  fprintf (stdout,"\t <-n> convert given input to host order\n");
  fprintf (stdout,"\t <-N> convert result to network order\n");
  fprintf (stdout,"\t <-e hex64string> string to encode\n");
  fprintf (stdout,"\t <-d hex64string> string to decode\n");
}

int main(int argc, char **argv) {
  uint8_t *key = encode_key;
  uint64_t input = encode_test;
  int decode_flag = 0;
  int encode_flag = 0;
  int input_is_network_order = 0;
  int output_to_network_order = 0;
  int c = 0;
  char *data = NULL;

  /* Parse command line */
  opterr = 1;
  while ((c = getopt (argc, argv, "d:e:hnN")) != -1) {
    switch (c) {
      case 'd':
	data = optarg;
	decode_flag = 1;
	break;
      case 'e':
	data = optarg;
	encode_flag = 1;
	break;
      case 'h':
	usage(argv[0]);
	return 0;
	break;
      case 'n':
	input_is_network_order = 1;
	break;
      case 'N':
	output_to_network_order = 1;
	break;
      default:
	return -1;
    }    
  }

  if ((!encode_flag) && (!decode_flag)) {
    fprintf(stderr,"Need to provide encode (-e) or decode (-d) option\n");
    return -1;
  }

  // Convert to uint64_t
  char *end = data;
  input = (uint64_t) strtoull (data, &end, 16);
  if (end == data) {
    fprintf(stderr,"Invalid value: %s\n", data);
    return -1;
  }

  // Convert to host order if requested
  if (input_is_network_order) {
    input = ntohll(input);
  }

  // Select key
  if (encode_flag) {
    fprintf(stdout,"Encoding: 0x%016Lx\n", input);
    key = encode_key;
  }
  else {
    fprintf(stdout,"Decoding: 0x%016Lx\n", input);
    key = decode_key;
  }

  // Run function
  megadfun(key,&input,&input);

  // Output result
  if (output_to_network_order) {
    input = htonll(input);
  }
  printf ("Result: 0x%016Lx\n", input);

  return 0;
}
#endif
