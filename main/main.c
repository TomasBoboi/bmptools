#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../src/bmptools.h"

uint32_t **pixelData_ppu32;

int32_t width, height;

int main(int argc, char **argv)
{
    width = 1423;
    height = 3143;
    /* this is just a plyground to test the implementation / provide some examples in the future */
    int fd = open("output.bmp", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    pixelData_ppu32 = (uint32_t**)malloc(height * sizeof(uint32_t*));
    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < height; rowIndex_u32++)
    {
        pixelData_ppu32[rowIndex_u32] = (uint32_t*)malloc(width * sizeof(uint32_t));
    }

    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < height; rowIndex_u32++)
    {
        for(uint32_t columnIndex_u32 = 0; columnIndex_u32 < width; columnIndex_u32++)
        {
            pixelData_ppu32[rowIndex_u32][columnIndex_u32] = rand() % 255;
        }
    }
    bmp_WriteImage(fd, pixelData_ppu32, width, height, (unsigned short)8u);
    close(fd);

    
    pixelData_ppu32 = (uint32_t**)malloc(47 * sizeof(uint32_t*));
    for(uint32_t rowIndex_u32 = 0; rowIndex_u32 < 47; rowIndex_u32++)
    {
        free(pixelData_ppu32[rowIndex_u32]);
    }
    free(pixelData_ppu32);
    return 0;
}