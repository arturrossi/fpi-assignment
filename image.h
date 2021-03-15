#ifndef IMAGE_H
#define IMAGE_H
#include <stdint.h>
#include <cstdio>
#include <map>
#include <vector>

enum ImageType {
    PNG, JPG, BMP, TGA
};

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
    bool write(const char* filename);
    ImageType get_file_type(const char* filename);

    Image& luminance();
    Image& quantize(int tons);
    Image& flipX();
    Image& flipY();

    std::map<int,float> histogram();
    Image& brightness(int b);
    Image& contrast(float a);
    Image& negative();
    Image& histogram_equalization(Image* new_img);
    Image& histogram_matching(Image& img);
    Image& convolution_no_borders(std::vector<std::vector<float>> kernel, Image *new_img, int soma);
    int getPixel(int x, int y);
    Image& rotate90(Image *new_img);
    std::map<int, float> hist_histogram_equalization();
};

#endif // IMAGE_H
