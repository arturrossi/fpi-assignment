#ifndef IMAGE_H
#define IMAGE_H
#include <stdint.h>
#include <cstdio>
#include <map>

class Image
{
public:
    Image();
    Image(const Image& img);
    Image(int w, int h, int channels);

    uint8_t* data;
    size_t size;
    int w;
    int h;
    int channels;
    bool read(const char* filename);

    Image& luminance();
    Image& quantize(int tons);
    Image& flipX();
    Image& flipY();

    std::map<int,int> histogram();
    Image& brightness(int b);
    Image& contrast(int a);
    Image& negative();

};

#endif // IMAGE_H
