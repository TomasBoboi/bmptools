#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../src/bmptools.h"

#define IMAGE_WIDTH  800u /* pixels */
#define IMAGE_HEIGHT 600u /* pixels */
#define IMAGE_DEPTH  24u   /* bits per pixel (supported values: 1, 4, 8, 16, 24, 32) */

uint32_t **pixelData_ppu32;

int main(int argc, char **argv)
{
    /* allocate the pixel array memory location */
    pixelData_ppu32 = (uint32_t**)malloc(IMAGE_HEIGHT * sizeof(uint32_t*));
    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < IMAGE_HEIGHT; rowIndex_u32++)
    {
        pixelData_ppu32[rowIndex_u32] = (uint32_t*)malloc(IMAGE_WIDTH * sizeof(uint32_t));
    }

    /* populate the pixel array */
    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < IMAGE_HEIGHT; rowIndex_u32++)
    {
        for(uint32_t columnIndex_u32 = 0; columnIndex_u32 < IMAGE_WIDTH; columnIndex_u32++)
        {
            // pixelData_ppu32[rowIndex_u32][columnIndex_u32] = rand() % (1 << IMAGE_DEPTH);
            pixelData_ppu32[rowIndex_u32][columnIndex_u32] = columnIndex_u32 % (rowIndex_u32 + 1) << ((columnIndex_u32 * (IMAGE_DEPTH - 1)) / IMAGE_WIDTH);
        }
    }

    /* write into the BMP file */
    bmp_WriteImage("output.bmp", pixelData_ppu32, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_DEPTH);

    /* deallocate the allocated pixel array memory locations */
    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < IMAGE_HEIGHT; rowIndex_u32++)
    {
        free(pixelData_ppu32[rowIndex_u32]);
    }
    free(pixelData_ppu32);

    return 0;
}