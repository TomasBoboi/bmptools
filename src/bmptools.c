#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

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


/* PRIVATE FUNCTIONS */
static void bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16);
static void bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16);
static void bmp_WriteColorPalette(int outputFileDescriptor_fd, uint16_t bitsPerPixel_u16);


/* IMPLEMENTATIONS */
static void bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16)
{
    bmp_BitmapFileHeader_t bitmapFileHeader_t;

    /* populate the header structure's fields with the appropriate data */
    bitmapFileHeader_t.fileType_u16    = BMP_HEADER_FILE_ID;
    bitmapFileHeader_t.fileSize_u32    = BMP_HEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE;
    bitmapFileHeader_t.fileSize_u32   += imageHeight_s32 * ((imageWidth_s32 * bitsPerPixel_u16 + 31) / 32) * 4;
    bitmapFileHeader_t.reserved1_u16   = (uint16_t)0u;
    bitmapFileHeader_t.reserved2_u16   = (uint16_t)0u;
    bitmapFileHeader_t.pixelOffset_u32 = BMP_HEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE;

    /* if the colod depth is less that 8 bits per pixel, we need to account for the color palette */
    if(bitsPerPixel_u16 <= (uint16_t)8u)
    {
        bitmapFileHeader_t.fileSize_u32 += ((1 << bitsPerPixel_u16) * (uint32_t)4u);
        bitmapFileHeader_t.pixelOffset_u32 += ((1 << bitsPerPixel_u16) * (uint32_t)4u);
    }

    /* write the header to the output file */
    if(write(outputFileDescriptor_fd, &bitmapFileHeader_t, sizeof(bitmapFileHeader_t)) < 0)
    {
        printf("Error while writing the bitmap header!\n"); exit(-1);
    }
}

static void bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16)
{
    bmp_BitmapInfoHeader_t bitmapInfoHeader_t;

    /* populate the information-header structure's fields with the appropriate data */
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

    /* write the information header to the output file */
    if(write(outputFileDescriptor_fd, &bitmapInfoHeader_t, sizeof(bitmapInfoHeader_t)) < 0)
    {
        printf("Error while writing the bitmap information header!\n"); exit(-1);
    }
}

static void bmp_WriteColorPalette(int outputFileDescriptor_fd, uint16_t bitsPerPixel_u16)
{
    uint32_t currentColorIndex_u32, currentColor_u32, colorToBeWritten_u32;
    uint32_t numberOfColors_u32 = (uint32_t)(1 << bitsPerPixel_u16);

    for(currentColorIndex_u32 = 0u; currentColorIndex_u32 < numberOfColors_u32; currentColorIndex_u32++)
    {
        currentColor_u32 = currentColorIndex_u32 * 0xFF / (numberOfColors_u32 - 1u);
        colorToBeWritten_u32 = currentColor_u32;  colorToBeWritten_u32 <<= 8;
        colorToBeWritten_u32 |= currentColor_u32; colorToBeWritten_u32 <<= 8;
        colorToBeWritten_u32 |= currentColor_u32; colorToBeWritten_u32 &= 0x00FFFFFF;

        if(write(outputFileDescriptor_fd, &colorToBeWritten_u32, sizeof(colorToBeWritten_u32)) < 0)
        {
            printf("Error while writing the bitmap color palette!\n"); exit(-1);
        }
    }
}

void bmp_WriteImage(char * outputFileName_str, uint32_t **pixelData_ppu32, int32_t imageWidth_s32, int32_t imageHeight_s32, uint16_t bitsPerPixel_u16)
{
    uint32_t zero_u32 = (uint32_t)0u;

    int outputFileDescriptor_fd = open(outputFileName_str, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

    bmp_WriteFileHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32, bitsPerPixel_u16);
    bmp_WriteInfoHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32, bitsPerPixel_u16);
    if(bitsPerPixel_u16 <= (uint16_t)8u)
    {
        bmp_WriteColorPalette(outputFileDescriptor_fd, bitsPerPixel_u16);
    }

    /* TODO: adapt writing for different color-depths */
    for(int32_t rowIndex_s32 = imageHeight_s32 - 1; rowIndex_s32 >= 0 ; rowIndex_s32--)
    {
        if(bitsPerPixel_u16 < (uint16_t)8u)
        {
            for(int32_t columnIndex_s32 = 0; columnIndex_s32 < imageWidth_s32; columnIndex_s32 += ((uint16_t)8u / bitsPerPixel_u16))
            {
                uint8_t currentByte_u8 = (uint8_t)0u;
                for(uint32_t pixelOffset_u32 = 0; pixelOffset_u32 < ((uint16_t)8u / bitsPerPixel_u16); pixelOffset_u32++)
                {
                    currentByte_u8 |= pixelData_ppu32[rowIndex_s32][columnIndex_s32 + pixelOffset_u32];
                    if(pixelOffset_u32 != ((uint16_t)8u / bitsPerPixel_u16) - 1)
                    {
                        currentByte_u8 = currentByte_u8 << bitsPerPixel_u16;
                    }
                }

                if(write(outputFileDescriptor_fd, &currentByte_u8, sizeof(currentByte_u8)) < 0)
                {
                    printf("Error while writing the pixels to the image!\n"); exit(-1);
                }
            }
        }
        else
        {
            for(int32_t columnIndex_s32 = 0; columnIndex_s32 < imageWidth_s32; columnIndex_s32++)
            {
                if(write(outputFileDescriptor_fd, &pixelData_ppu32[rowIndex_s32][columnIndex_s32], bitsPerPixel_u16 / (uint16_t)8u) < 0)
                {
                    printf("Error while writing the pixels to the image!\n"); exit(-1);
                }
            }
        }
        if(imageWidth_s32 % 4u != 0)
        {
            if(write(outputFileDescriptor_fd, &zero_u32, (4u - (imageWidth_s32 % 4u))) < 0)
            {
                printf("Error while writing the pixels to the image!\n"); exit(-1);
            }
        }
    }

    close(outputFileDescriptor_fd);
}