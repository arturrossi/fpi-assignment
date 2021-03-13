#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "ui_mainwindow.h"

using namespace std;

Image::Image() {
    data = NULL;
    size = 0;
    w = 0;
    h = 0;
    channels = 3;
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
    size = w*h*channels;
    data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
    memcpy(data, img.data, size);
}

bool Image::read(const char* filename) {
    data = stbi_load(filename, &w, &h, &channels, 0);
    return data != NULL;
}

Image& Image::luminance() {
    if(channels < 3) {
        printf("Image %p has less than 3 channels, it is assumed to already be grayscale.", this);
    }
    else {
        for(int i = 0; i < w*h*channels; i+=channels) {
            int gray = 0.299*data[i] + 0.587*data[i+1] + 0.114*data[i+2];
            memset(data+i, gray, 3);
        }
    }
    return *this;
}

Image& Image::quantize(int tons) {
    int maior = data[0];
    int menor = maior;

    int tam_int = 0;

    for(int i = 0; i < w*h*channels; i+=channels) {
        if (data[i] > maior) {
            maior = data[i];
        }
        if (data[i] < menor) {
            menor = data[i];
        }
    }

    tam_int = maior - menor + 1;

    if (tons < tam_int) {
        float tb = tam_int / tons;
        float comeco = menor - 0.5;
        float fim = menor - 0.5 + tb;
        for (int i = 0; i < w*h*channels; i+= channels) {
            while(!(data[i] >= comeco && data[i] < fim)) {
                comeco = fim;
                fim += tb;
            }
            int meio = (fim + comeco) / 2;
            memset(data+i, meio, 3);
            comeco = menor - 0.5;
            fim = menor - 0.5 + tb;
        }
    }

    return *this;
}


Image& Image::flipX() {
    uint8_t tmp[4];
    uint8_t* p1;
    uint8_t* p2;
    for(int y = 0;y < h;++y) {
        for(int x = 0;x < w/2;++x) {
            p1 = &data[(x + y * w) * 3];
            p2 = &data[((w - 1 - x) + y * w) * 3];

            memcpy(tmp, p1, 3);
            memcpy(p1, p2, 3);
            memcpy(p2, tmp, 3);
        }
    }
    return *this;
}

Image& Image::flipY() {
    uint8_t tmp[4];
    uint8_t* p1;
    uint8_t* p2;
    for(int x = 0; x < w; ++x) {
        for(int y = 0; y < h/2; ++y) {
            p1 = &data[(x + y * w) * channels];
            p2 = &data[(x + (h - 1 - y) * w) * channels];

            memcpy(tmp, p1, channels);
            memcpy(p1, p2, channels);
            memcpy(p2, tmp, channels);
        }
    }
    return *this;
}

std::map<int,int> Image::histogram() {
    std::map<int, int> counts;

    for (int i = 0; i < w*h*channels; i+=3) {
        counts[data[i]]++;
    }

    return counts;
}

Image& Image::brightness(int b) {
    for (int i = 0; i < w*h*channels; i++) {
        if (b >= 0) {
            data[i] + b >= 255 ? data[i] = 255 : data[i] + b;
        }
        else {
            data[i] + b <= 0 ? data[i] = 0 : data[i] + b;
        }
    }

    return *this;
}

Image& Image::contrast(int a) {
    for (int i = 0; i < w*h*channels; i++) {
        int temp = a * data[i];

        temp > 255 ? data[i] = 255 : data[i] = temp;
    }

    return *this;
}

Image& Image::negative(){
    for (int i = 0; i < w*h*channels; i++) {
        uint8_t new_value = 255 - data[i];
        data[i] = new_value;
    }

    return *this;
}
