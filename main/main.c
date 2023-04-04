#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#include "../src/bmptools.h"

#define IMAGE_WIDTH  1920u /* pixels */
#define IMAGE_HEIGHT 1080u /* pixels */
#define IMAGE_DEPTH  8u   /* bits per pixel */

uint32_t **pixelData_ppu32;

int main(int argc, char **argv)
{
    /* open/create the output file */
    int fd = open("output.bmp", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

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
            pixelData_ppu32[rowIndex_u32][columnIndex_u32] = rowIndex_u32 % (columnIndex_u32 + 1);
        }
    }

    /* write into the BMP file */
    bmp_WriteImage(fd, pixelData_ppu32, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_DEPTH);

    /* close the output file */
    close(fd);

    /* deallocate the allocated pixel array memory locations */
    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < IMAGE_HEIGHT; rowIndex_u32++)
    {
        free(pixelData_ppu32[rowIndex_u32]);
    }
    free(pixelData_ppu32);

    return 0;
}