#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "bmptools.h"


/* DEFINES */
#define BMP_HEADER_SIZE                   (uint32_t)14u    /* each BMP file has a 14-byte header */
#define BMP_HEADER_FILE_ID                (uint16_t)0x4D42 /* 'BM' in ASCII, big endian */

#define BMP_BITMAPINFOHEADER_SIZE         (uint32_t)40u /* BITMAPINFOHEADER has a length of 40 bytes */
#define BMP_BITMAPINFOHEADER_COMPRESSION  (uint32_t)0u  /* BI_RGB compression */


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
    int32_t  bitmapHeight_s32;
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
static int bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16);
static int bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16);
static int bmp_WriteColorPalette(int outputFileDescriptor_fd, uint16_t bitsPerPixel_u16);


/* IMPLEMENTATIONS */
static int bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16)
{
    int returnValue = 0;

    bmp_BitmapFileHeader_t bitmapFileHeader_t;

    bitmapFileHeader_t.fileType_u16 = BMP_HEADER_FILE_ID;
    bitmapFileHeader_t.fileSize_u32 = BMP_HEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE;
    bitmapFileHeader_t.fileSize_u32 += imageHeight_s32 * ((imageWidth_s32 * bitsPerPixel_u16 + 31) / 32) * 4;
    bitmapFileHeader_t.reserved1_u16 = 0x0000;
    bitmapFileHeader_t.reserved2_u16 = 0x0000;
    bitmapFileHeader_t.pixelOffset_u32 = BMP_HEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE;

    if(bitsPerPixel_u16 <= (uint16_t)8u)
    {
        bitmapFileHeader_t.fileSize_u32 += ((1 << bitsPerPixel_u16) * (uint32_t)4u);
        bitmapFileHeader_t.pixelOffset_u32 += ((1 << bitsPerPixel_u16) * (uint32_t)4u);
    }

    returnValue = write(outputFileDescriptor_fd, &bitmapFileHeader_t, sizeof(bitmapFileHeader_t));

    return returnValue;
}

static int bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16)
{
    int returnValue = 0;

    bmp_BitmapInfoHeader_t bitmapInfoHeader_t;

    bitmapInfoHeader_t.infoHeaderSize_u32       = BMP_BITMAPINFOHEADER_SIZE;
    bitmapInfoHeader_t.bitmapWidth_s32          = imageWidth_s32;
    bitmapInfoHeader_t.bitmapHeight_s32         = imageHeight_s32;
    bitmapInfoHeader_t.colorPlanes_u16          = (uint16_t)1u;
    bitmapInfoHeader_t.colorDepth_u16           = bitsPerPixel_u16;
    bitmapInfoHeader_t.compressionMethod_u32    = BMP_BITMAPINFOHEADER_COMPRESSION;
    bitmapInfoHeader_t.rawImageSize_u32         = (uint32_t)0u;
    bitmapInfoHeader_t.horizontalResolution_s32 = (uint32_t)0u;
    bitmapInfoHeader_t.verticalResolution_s32   = (uint32_t)0u;
    bitmapInfoHeader_t.colorPaletteColors_u32   = (uint32_t)0u;
    bitmapInfoHeader_t.importantColors_u32      = (uint32_t)0u;

    returnValue = write(outputFileDescriptor_fd, &bitmapInfoHeader_t, sizeof(bitmapInfoHeader_t));

    return returnValue;
}

static int bmp_WriteColorPalette(int outputFileDescriptor_fd, uint16_t bitsPerPixel_u16)
{
    int returnValue = 0;

    if(bitsPerPixel_u16 <= (uint16_t)8u)
    {
        uint32_t currentColor_u32, colorToBeWritten_u32;
        uint32_t numberOfColors_u32 = (uint32_t)(1 << bitsPerPixel_u16);

        for(currentColor_u32 = 0u; currentColor_u32 < numberOfColors_u32; currentColor_u32++)
        {
            colorToBeWritten_u32 = currentColor_u32 * 0xFF / (numberOfColors_u32 - 1u);  colorToBeWritten_u32 <<= 8;
            colorToBeWritten_u32 |= currentColor_u32 * 0xFF / (numberOfColors_u32 - 1u); colorToBeWritten_u32 <<= 8;
            colorToBeWritten_u32 |= currentColor_u32 * 0xFF / (numberOfColors_u32 - 1u); colorToBeWritten_u32 &= 0x00FFFFFF;

            returnValue = write(outputFileDescriptor_fd, &colorToBeWritten_u32, sizeof(colorToBeWritten_u32));
        }
    }

    return returnValue;
}

int bmp_WriteImage(int outputFileDescriptor_fd,
                   unsigned int **pixelData_ppu32,
                   signed int imageWidth_s32,
                   signed int imageHeight_s32,
                   unsigned short bitsPerPixel_u16)
{
    int returnValue = 0;

    uint32_t zero = (uint32_t)0u;

    returnValue = bmp_WriteFileHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32, bitsPerPixel_u16);
    returnValue = bmp_WriteInfoHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32, bitsPerPixel_u16);
    returnValue = bmp_WriteColorPalette(outputFileDescriptor_fd, bitsPerPixel_u16);

    for(int32_t rowIndex_s32 = imageHeight_s32 - 1; rowIndex_s32 >= 0 ; rowIndex_s32--)
    {
        for(int32_t columnIndex_s32 = 0; columnIndex_s32 < imageWidth_s32; columnIndex_s32++)
        {
            returnValue = write(outputFileDescriptor_fd, &pixelData_ppu32[rowIndex_s32][columnIndex_s32], sizeof(uint8_t));
        }
        if(imageWidth_s32 % 4 != 0)
        {
            write(outputFileDescriptor_fd, &zero, (4 - (imageWidth_s32 % 4)));
        }
    }

    return returnValue;
}