#include <stdint.h> // int32_t
#include ".\libraries\miniz.h"

// compile with : cl .\q1.c .\libraries\miniz.c /Fe:q1

//#define FIXED

// from https://www.w3.org/TR/png/#D-CRCAppendix
uint32_t calc_crc32(unsigned char * buffer, uint32_t length)
{
    // Init table
    uint32_t c, crc_table[256];
    uint16_t n, k;

    for (n = 0; n < 256; n++)
    {
        c = (uint32_t)n;
        for (k = 0; k < 8; k++)
        {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    // Compute
    c = 0xffffffffu;
    for (n = 0; n < length; n++) {
        c = crc_table[(c ^ buffer[n]) & 0xff] ^ (c >> 8);
    }
    printf("crc32 = %u\n", c ^ 0xffffffffu);
    return c ^ 0xffffffffu;
}

int main(int argc, const char *argv[])
{
    const char * file_path = "q1.png";

    printf("Version of miniz.c : %s\n", MZ_VERSION);

    // PNG signature
    uint8_t signature[8] = {0};
    signature[0] = 0x89;
    signature[1] = 0x50; // P in ASCII
    signature[2] = 0x4E; // N in ASCII
    signature[3] = 0x47; // G in ASCII
    signature[4] = 0x0D; // CR
    signature[5] = 0x0A; // LF (CRLF = DOS line ending)
    signature[6] = 0x1A; // DOS end of file
    signature[7] = 0x0A; // LF (UNIX line ending)
    printf("Signature OK\n");

    // IHDR = image header
    // IHDR - LENGTH (4)
    uint32_t length = 13;
    uint32_t swapped_length = _byteswap_ulong(length); // 00 00 00 0D;
    // IHDR - CHUNK TYPE (4)
    uint8_t type[4] = {'I', 'H', 'D', 'R'}; // 49 48 44 52
    uint32_t width = _byteswap_ulong(1);
    uint32_t height = _byteswap_ulong(1);
    uint8_t bits_per_channel = 8;
    uint8_t color_type = 2;
    uint8_t compression_method = 0;
    uint8_t filter_method = 0;
    uint8_t interlaced = 0;
    // Writing to buffer
    uint32_t idhr_length = 4 + 4 + length + 4;
    uint8_t idhr[4 + 4 + 13 + 4] = {0};
    memcpy(idhr, &swapped_length, 4);
    memcpy(idhr + 4, &type, 4);
    memcpy(idhr + 8, &width, 4);
    memcpy(idhr + 12, &height, 4);
    memcpy(idhr + 16, &bits_per_channel, 1);
    memcpy(idhr + 17, &color_type, 1);
    memcpy(idhr + 18, &compression_method, 1);
    memcpy(idhr + 19, &filter_method, 1);
    memcpy(idhr + 20, &interlaced, 1);
    // IHDR - CRC (4) on type & content but not length
    //uint32_t crc = _byteswap_ulong(0x907753DE); // 2423739358
    uint32_t crc = _byteswap_ulong(calc_crc32(idhr + 4, length + 4)); // ignore length, add chunck type length
    memcpy(idhr + 21, &crc, 4);
    printf("IHDR OK\n");

    // IDAT
    // IDAT - LENGTH (4)
    length = 12;
    swapped_length = _byteswap_ulong(length); // 00 00 00 0C
    // IDAT - CHUNK TYPE (4)
    type[0] = 'I'; // 49
    type[1] = 'D'; // 44
    type[2] = 'A'; // 41
    type[3] = 'T'; // 54
    // IDAT - DATA (12)
#ifdef FIXED
    compression_method = 0x08; // Obligatoire et fixe pour PNG
    uint8_t fcheck = 0xD7;
    uint8_t *data = malloc(sizeof(uint8_t) * 6);
    data[0] = 0x63;
    data[1] = 0xF8;
    data[2] = 0xCF;
    data[3] = 0xC0;
    data[4] = 0x00;
    data[5] = 0x00;
    uint32_t adler32 = _byteswap_ulong(0x03010100);

    // Writing to buffer
    uint32_t idat_length = 4 + 4 + length + 4;
    uint8_t idat[4 + 4 + 12 + 4] = {0};
    memcpy(idat, &length, 4);
    memcpy(idat + 4, &type, 4);
    memcpy(idat + 8, &compression_method, 1);
    memcpy(idat + 9, &fcheck, 1);
    memcpy(idat + 10, data, 6);
    memcpy(idat + 16, &adler32, 4);
    // IDAT - CRC (4) on type & content but not length
    //crc = _byteswap_ulong(0x18DD8DB0); // 417172912
    crc = _byteswap_ulong(calc_crc32(idat + 4, length + 4)); // ignore length idat + 4, add chunck type length + 4 (avant)
    memcpy(idat + 20, &crc, 4);
    printf("IDAT OK\n");
#else
    // Compression test
    uint8_t buffer_in[4] = {0x00, 0xFF, 0x00, 0x00}; // type 0 de method filtre 0 puis un pixel rouge
    size_t buffer_in_length = sizeof(buffer_in);
    // Le résultat doit être : 63 F8 CF C0 00 00
    uint8_t buffer_out[256] = {0};
    size_t buffer_out_length = sizeof(buffer_out);
    // Test1 int status = mz_compress(buffer_out, &buffer_out_length, buffer_in, buffer_in_length);
    // Test2
    int status = mz_compress2(buffer_out, &buffer_out_length, buffer_in, buffer_in_length, MZ_DEFLATED | MZ_FIXED | MZ_DEFAULT_COMPRESSION);
    if (status != Z_OK)
    {
        printf("mz_compress() failed!\n");
        return;
    }
    printf("Compressed output %zu bytes:\n", buffer_out_length);
    for (uint16_t i = 0; i < buffer_out_length; i++) // stream.total_out; i++)
    {
        printf("%02X\n", buffer_out[i], i);
    }
    uint8_t buffer_uncompressed[256] = {0};
    size_t buffer_uncompressed_length = sizeof(buffer_uncompressed);
    status = mz_uncompress2(buffer_uncompressed, &buffer_uncompressed_length, buffer_out, &buffer_out_length);
    printf("Uncompressed %zu bytes: \n", buffer_out_length);
    for (uint16_t i = 0; i < buffer_uncompressed_length; i++)
    {
        printf("0x%02x\n", buffer_uncompressed[i]);
    }
    length = buffer_out_length;
    swapped_length = _byteswap_ulong(length);
    uint32_t idat_length = 4 + 4 + length + 4;
    uint8_t * idat = calloc(idat_length, sizeof(uint8_t));
    crc = _byteswap_ulong(calc_crc32(idat + 4, length + 4)); // ignore length idat + 4, add chunck type length + 4 (avant)
    memcpy(idat, &length, 4);
    memcpy(idat + 4, &type, 4);
    memcpy(idat + 8, buffer_out, buffer_out_length);
    memcpy(idat + 20, &crc, 4);
    printf("IDAT OK\n");
#endif

    // IEND
    // IEND - LENGTH (4)
    length = 0;
    swapped_length = _byteswap_ulong(length);
    // IEND - CHUNK TYPE (4)
    type[0] = 'I'; // 49
    type[1] = 'E'; // 45
    type[2] = 'N'; // 4E
    type[3] = 'D'; // 44
    // Writing to buffer
    uint32_t iend_length = 4 + 4 + length + 4;
    uint8_t iend[4 + 4 + 0 + 4] = {0};
    memcpy(iend, &swapped_length, 4);
    memcpy(iend + 4, &type, 4);
    // IEND - CRC (4) on type & content but not length
    // crc = _byteswap_ulong(0xAE426082); // 2923585666
    crc = _byteswap_ulong(calc_crc32(iend + 4, length + 4)); // ignore length, add chunck type length
    memcpy(iend + 8, &crc, 4);
    printf("IDEND OK\n");

    // Writing file
    FILE *f = fopen(file_path, "wb");
    if (f == NULL)
    {
        printf("ERROR: Unable to open file %s for writing.\n", file_path);
        return;
    }
    fwrite(signature, 1, 8, f);
    fwrite(idhr, 1, idhr_length, f);
    fwrite(idat, 1, idat_length, f);
    fwrite(iend, 1, iend_length, f);
    fclose(f);
    printf("Saved at : %s\n", file_path);
}

/*

output with FIXED q1.png
89 50 4E 47 0D 0A 1A 0A 00 00 00 0D 49 48 44 52 00 00 00 01 00 00 00 01 08 02 00 00 00 90 77 53 DE 0C 00 00 00 49 44 41 54 08 D7 63 F8 CF C0 00 00 03 01 01 00 18 DD 8D B0 00 00 00 00 49 45 4E 44 AE 42 60 82

output without FIXED T1 (mz_compress) q1.png
89 50 4E 47 0D 0A 1A 0A 00 00 00 0D 49 48 44 52 00 00 00 01 00 00 00 01 08 02 00 00 00 90 77 53 DE 0F 00 00 00 49 44 41 54 78 9C 01 04 00 FB FF 00 FF 00 00 03 DA 08 C9 6F 00 00 00 00 00 00 00 49 45 4E 44 AE 42 60 82

output without FIXED T2 (mz_compress2 with MZ_FIXED | MZ_DEFAULT_COMPRESSION) q1.png
89 50 4E 47 0D 0A 1A 0A 00 00 00 0D 49 48 44 52 00 00 00 01 00 00 00 01 08 02 00 00 00 90 77 53 DE 0F 00 00 00 49 44 41 54 78 9C 01 04 00 FB FF 00 FF 00 00 03 DA 08 C9 6F 00 00 00 00 00 00 00 49 45 4E 44 AE 42 60 82

*/
