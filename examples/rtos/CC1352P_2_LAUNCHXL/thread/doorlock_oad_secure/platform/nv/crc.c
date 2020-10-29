/**
 * \file
 * Functions and types for CRC checks.
 *
 * Generated on Thu Nov 16 17:30:09 2017
 * by pycrc v0.9.1, https://pycrc.org
 * using the configuration:
 *  - Width         = 8
 *  - Poly          = 0x97
 *  - XorIn         = 0x00
 *  - ReflectIn     = False
 *  - XorOut        = 0x00
 *  - ReflectOut    = False
 *  - Algorithm     = table-driven
 */
#include "crc.h"     /* include the header file generated with pycrc */
#include <stdlib.h>
#include <stdint.h>



/**
 * Static table used for the table_driven implementation.
 */
static const crc_t crc_table[256] = {
    0x00, 0x97, 0xb9, 0x2e, 0xe5, 0x72, 0x5c, 0xcb, 0x5d, 0xca, 0xe4, 0x73, 0xb8, 0x2f, 0x01, 0x96,
    0xba, 0x2d, 0x03, 0x94, 0x5f, 0xc8, 0xe6, 0x71, 0xe7, 0x70, 0x5e, 0xc9, 0x02, 0x95, 0xbb, 0x2c,
    0xe3, 0x74, 0x5a, 0xcd, 0x06, 0x91, 0xbf, 0x28, 0xbe, 0x29, 0x07, 0x90, 0x5b, 0xcc, 0xe2, 0x75,
    0x59, 0xce, 0xe0, 0x77, 0xbc, 0x2b, 0x05, 0x92, 0x04, 0x93, 0xbd, 0x2a, 0xe1, 0x76, 0x58, 0xcf,
    0x51, 0xc6, 0xe8, 0x7f, 0xb4, 0x23, 0x0d, 0x9a, 0x0c, 0x9b, 0xb5, 0x22, 0xe9, 0x7e, 0x50, 0xc7,
    0xeb, 0x7c, 0x52, 0xc5, 0x0e, 0x99, 0xb7, 0x20, 0xb6, 0x21, 0x0f, 0x98, 0x53, 0xc4, 0xea, 0x7d,
    0xb2, 0x25, 0x0b, 0x9c, 0x57, 0xc0, 0xee, 0x79, 0xef, 0x78, 0x56, 0xc1, 0x0a, 0x9d, 0xb3, 0x24,
    0x08, 0x9f, 0xb1, 0x26, 0xed, 0x7a, 0x54, 0xc3, 0x55, 0xc2, 0xec, 0x7b, 0xb0, 0x27, 0x09, 0x9e,
    0xa2, 0x35, 0x1b, 0x8c, 0x47, 0xd0, 0xfe, 0x69, 0xff, 0x68, 0x46, 0xd1, 0x1a, 0x8d, 0xa3, 0x34,
    0x18, 0x8f, 0xa1, 0x36, 0xfd, 0x6a, 0x44, 0xd3, 0x45, 0xd2, 0xfc, 0x6b, 0xa0, 0x37, 0x19, 0x8e,
    0x41, 0xd6, 0xf8, 0x6f, 0xa4, 0x33, 0x1d, 0x8a, 0x1c, 0x8b, 0xa5, 0x32, 0xf9, 0x6e, 0x40, 0xd7,
    0xfb, 0x6c, 0x42, 0xd5, 0x1e, 0x89, 0xa7, 0x30, 0xa6, 0x31, 0x1f, 0x88, 0x43, 0xd4, 0xfa, 0x6d,
    0xf3, 0x64, 0x4a, 0xdd, 0x16, 0x81, 0xaf, 0x38, 0xae, 0x39, 0x17, 0x80, 0x4b, 0xdc, 0xf2, 0x65,
    0x49, 0xde, 0xf0, 0x67, 0xac, 0x3b, 0x15, 0x82, 0x14, 0x83, 0xad, 0x3a, 0xf1, 0x66, 0x48, 0xdf,
    0x10, 0x87, 0xa9, 0x3e, 0xf5, 0x62, 0x4c, 0xdb, 0x4d, 0xda, 0xf4, 0x63, 0xa8, 0x3f, 0x11, 0x86,
    0xaa, 0x3d, 0x13, 0x84, 0x4f, 0xd8, 0xf6, 0x61, 0xf7, 0x60, 0x4e, 0xd9, 0x12, 0x85, 0xab, 0x3c
};


crc_t crc_update(crc_t crc, const void *data, size_t data_len)
{
    const unsigned char *d = (const unsigned char *)data;
    unsigned int tbl_idx;

    while (data_len--) {
        tbl_idx = crc ^ *d;
        crc = crc_table[tbl_idx] & 0xff;
        d++;
    }
    return crc;
}
