//
// Created by pc on 2021/11/1.
//

#ifndef SECOND_SAVE_H
#define SECOND_SAVE_H

#include <cstdio>

int saveBmpForRGBA(unsigned char *img, unsigned width, unsigned height, const char *name) {
    unsigned char header[54] = {
            0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, \
            0x36, 0, 0, 0, 0x28, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0x1, 0, 0x20, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0 \
};
    char imgName[64] = {0};
    snprintf(imgName, sizeof(imgName), "./%s.bmp", name);
    long file_size = (long) width * (long) height;
    header[2] = (unsigned char) (file_size & 0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;

    long w = long(width);
    header[18] = w & 0x000000ff;
    header[19] = (w >> 8) & 0x000000ff;
    header[20] = (w >> 16) & 0x000000ff;
    header[21] = (w >> 24) & 0x000000ff;

    long h = long(height);
    header[22] = h & 0x000000ff;
    header[23] = (h >> 8) & 0x000000ff;
    header[24] = (h >> 16) & 0x000000ff;
    header[25] = (h >> 24) & 0x000000ff;

    FILE *fp = nullptr;
    if (!(fp = fopen(imgName, "wb")))
        return -1;
    fwrite(header, sizeof(unsigned char), 54, fp);
    fwrite(img, sizeof(unsigned char), width * height * 4, fp);
    fclose(fp);
    return 0;
}


#endif //SECOND_SAVE_H
