#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "bmptools.h"




/* DEFINES */
#define BMP_HEADER_SIZE           14u    /* each BMP file has a 14-byte header */
#define BMP_FILE_IDENTIFIER       0x4D42 /* 'BM' in ASCII, big endian */
#define BMP_BITMAPINFOHEADER_SIZE 40u    /* BITMAPINFOHEADER has a length of 40 bytes */
#define BMP_COLOR_PLANES            1
#define BMP_COLOR_DEPTH                8
#define BMP_COMPRESSION_METHOD        0
#define BMP_HORIZONTAL_RESOLUTION    0
#define BMP_VERTICAL_RESOLUTION        0
#define BMP_COLOR_PALETTE            0
#define BMP_IMPORTANT_COLORS        0

#define BMP_COLOR_BLACK        0
#define BMP_COLOR_WHITE        255




/* STRUCTURES/TYPES */
typedef struct __attribute__((__packed__)) bmp_BitmapFileHeader_st {
    uint16_t fileType_u16;
    uint32_t fileSize_u32;
    uint16_t reserved1_u16;
    uint16_t reserved2_u16;
    uint32_t pixelOffset_u32;
} bmp_BitmapFileHeader_t;

typedef struct __attribute__((__packed__)) bmp_BitmapInfoHeader_st {
    uint32_t infoHeaderSize_u32;
    int32_t  bitmapWidth_s32;
    int32_t  bitmapHeight_u32;
    uint16_t colorPlanes_u16;
    uint16_t colorDepth_u16;
    uint32_t compressionMethod_u32;
    uint32_t rawImageSize_u32;
    int32_t  horizontalResolution_s32;
    int32_t  verticalResolution_s32;
    uint32_t colorPaletteColors_u32;
    uint32_t importantColors_u32;
} bmp_BitmapInfoHeader_t;




/* GLOBAL VARIABLES*/




/* PRIVATE FUNCTIONS */
static int bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint32_t bitsPerPixel_u32);


static int bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint32_t bitsPerPixel_u32)
{
    int returnValue = 0;

    bmp_BitmapFileHeader_t bitmapFileHeader_t;

    bitmapFileHeader_t.fileType_u16 = BMP_FILE_IDENTIFIER;
    bitmapFileHeader_t.fileSize_u32 = BMP_HEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE + ((1 << bitsPerPixel_u32) * (uint32_t)4u);
    bitmapFileHeader_t.fileSize_u32 += imageHeight_s32 * ((imageWidth_s32 * bitsPerPixel_u32 + 31) / 32) * 4;
    bitmapFileHeader_t.reserved1_u16 = 0x0000;
    bitmapFileHeader_t.reserved2_u16 = 0x0000;
    bitmapFileHeader_t.pixelOffset_u32 = BMP_HEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE + ((1 << bitsPerPixel_u32) * (uint32_t)4u);

    returnValue = write(outputFileDescriptor_fd, &bitmapFileHeader_t, sizeof(bitmapFileHeader_t));
    if(returnValue < 0)
    {
        returnValue = -1;
    }
    else
    {
        returnValue = 1;
    }

    return returnValue;
}

int bmp_WriteImage(int outputFileDescriptor_fd,
                   unsigned int **pixelData_u32,
                   signed int imageWidth_s32,
                   signed int imageHeight_s32,
                   unsigned int bitsPerPixel_u32)
{
    return bmp_WriteFileHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32, bitsPerPixel_u32);
}