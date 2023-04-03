#ifndef BMPTOOLS_H
#define BMPTOOLS_H

int bmp_WriteImage(int outputFileDescriptor_fd,
                   unsigned int **pixelData_u32,
                   signed int imageWidth_s32,
                   signed int imageHeight_s32,
                   unsigned int bitsPerPixel_u32);

#endif /* BMPTOOLS_H */