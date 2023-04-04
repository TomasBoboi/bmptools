#ifndef BMPTOOLS_H
#define BMPTOOLS_H

#include <stdint.h>

void bmp_WriteImage(char * outputFileName_str,
                   uint32_t **pixelData_ppu32,
                   int32_t imageWidth_s32,
                   int32_t imageHeight_s32,
                   uint16_t bitsPerPixel_u16);

#endif /* BMPTOOLS_H */