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
    channels = 0;
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

bool Image::write(const char* filename) {
    ImageType type = get_file_type(filename);
    int success;
    switch (type) {
      case PNG:
        success = stbi_write_png(filename, w, h, channels, data, w*channels);
        break;
      case BMP:
         success = stbi_write_bmp(filename, w, h, channels, data);
         break;
       case JPG:
         success = stbi_write_jpg(filename, w, h, channels, data, 100);
         break;
       case TGA:
         success = stbi_write_tga(filename, w, h, channels, data);
         break;
     }
     return success != 0;
}

ImageType Image::get_file_type(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if(ext != nullptr) {
        if(strcmp(ext, ".png") == 0) {
            return PNG;
        }
        else if(strcmp(ext, ".jpg") == 0) {
            return JPG;
        }
        else if(strcmp(ext, ".bmp") == 0) {
            return BMP;
        }
        else if(strcmp(ext, ".tga") == 0) {
            return TGA;
        }
    }
    return PNG;
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
            p1 = &data[(x + y * w) * channels];
            p2 = &data[((w - 1 - x) + y * w) * channels];

            memcpy(tmp, p1, channels);
            memcpy(p1, p2, channels);
            memcpy(p2, tmp, channels);
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

Image& Image::brightness(int b) {
    for (int i = 0; i < w*h*channels; i++) {
        int temp = data[i] + b;
        if (b >= 0) {
            data[i] = temp < 255 ? temp : 255;
        }
        else {
            data[i] = temp <= 0 ? 0 : temp;
        }
    }

    return *this;
}

Image& Image::contrast(float a) {
    for (int i = 0; i < w*h*channels; i++) {
        int temp = a * data[i];
        data[i] = temp > 255 ? 255 : temp;
    }

    return *this;
}

Image& Image::negative(){
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h*3; y++) {
            uint8_t new_value = 255 - data[(x+y*w)];
            data[(x+y*w)] = new_value;
        }
    }
    return *this;
}

std::map<int,float> Image::histogram() {
    std::map<int, int> histogram;
    std::map<int, float> normalized_histogram;

    for (int i = 0; i < w*h*channels; i++) {
        histogram[data[i]]++;
    }

    for (auto const& [key, val]: histogram) {
        normalized_histogram[key] = histogram[key];
    }

    return normalized_histogram;
}

map <int, float> Image::hist_histogram_equalization() {
    float alpha = 255.0 / (w*h*channels);
    map<int, float> original_histogram;
    map<int, float> cumulative_histogram;
    map<int, float> equalized_histogram;
    map<int, float> normalized_equalized_histogram;
    map<int, float>::iterator it;

    original_histogram = this->histogram();

    cumulative_histogram[0] = alpha * original_histogram.begin()->second;

    for (it = original_histogram.begin(), ++it; it != original_histogram.end(); it++) {
        cumulative_histogram[it->first] = cumulative_histogram[it->first - 1] + (alpha * original_histogram[it->first]);
    }

    return cumulative_histogram;
}

Image& Image::histogram_equalization(Image* new_img) {
    float alpha = 255.0 / (w*h*channels);
    map<int, float> original_histogram;
    map<int, float> cumulative_histogram;
    map<int, float> equalized_histogram;
    map<int, float> normalized_equalized_histogram;
    map<int, float>::iterator it;

    original_histogram = this->histogram();

    cumulative_histogram[0] = alpha * original_histogram.begin()->second;

    for (it = original_histogram.begin(), ++it; it != original_histogram.end(); it++) {
        cumulative_histogram[it->first] = cumulative_histogram[it->first - 1] + (alpha * original_histogram[it->first]);
    }

    for (int i = 0; i < w*h*channels; i++) {
        new_img->data[i] = uint8_t(cumulative_histogram[data[i]]);
    }

    return *new_img;
}

Image& Image::histogram_matching(Image& img) {
    map <int, float> cumulative_src_histogram;
    map <int, float> cumulative_target_histogram;
    map <int, float> result_matching_histogram;

//    Image* source = new Image();
//    Image* target = new Image();

    cumulative_src_histogram = hist_histogram_equalization();
    cumulative_target_histogram = img.hist_histogram_equalization();

//    cumulative_src_histogram = source->histogram();
//    cumulative_target_histogram = target->histogram();

    int closest;

    for (int i = 0; i < 256; i++) {
        float distance = abs(cumulative_target_histogram[0] - cumulative_src_histogram[i]);
        for (int j = 1; j < 256; j ++) {
            float distanceTemp = abs(cumulative_src_histogram[i] - cumulative_target_histogram[j]);

            if (distanceTemp < distance) {
                distance = distanceTemp;
                closest = j;
            }
        }
        result_matching_histogram[i] = closest;
    }

    for (int i = 0; i < w*h*channels; i++) {
        data[i] = uint8_t(result_matching_histogram[data[i]]);
    }

    return *this;
}

Image& Image::convolution_no_borders(vector<vector<float>> kernel, Image *new_img, int soma) {
    int n2 = 3/2;
    int m2 = 3/2;
    float sum = 0.0;

    for (int y = n2; y <= (this->h) - n2 - 1; y++) {
        for (int x = m2; x <= (this->w*3) - m2 -1; x+=3) {
            sum = 0.0;
            for (int k = -n2; k <= n2; k++) {
                for (int j = -m2; j <= m2; j++) {
                    sum += kernel[m2+j][n2+k] * this->data[x-j+(y-k)*w*3];
                }
            }
            sum += soma;
            if (sum > 255) {
                sum = 255;
            }
            else if (sum < 0) {
                sum = 0;
            }
            int index = x+y*w*3;
            memset(new_img->data +index, uint8_t(sum), 3);
        }
    }

    return *new_img;
}

int Image::getPixel(int x, int y) {
    int dataToReturn = data[x + y * w];
    return dataToReturn;
}

Image& Image::rotate90(Image *new_img) {
    uint8_t tmp[4];
    uint8_t* p1;
    uint8_t* p2;

    // x-axis reflection
    for(int row_i = 0 ; row_i < h / 2; ++row_i) {
        int row_j = (h - row_i - 1);
        for(int col_i = 0 ; col_i < w*3 ; ++col_i) {
            p1 = &data[(row_i*w*3 + col_i)];
            p2 = &data[(row_j*w*3+ col_i)];
            memcpy(tmp, p1, 3);
            memcpy(p1, p2, 3);
            memcpy(p2, tmp, 3);
//            swap(data[(col_i + row_i*w)], data[(row_j*w + col_i)]);
        }
    }

    // diagonal reflection
    for(int row_i = 0 ; row_i < h*3; ++row_i) {
        for(int col_i = row_i + 1 ; col_i < w ; ++col_i){
            p1 = &data[(row_i*w + col_i)];
            p2 = &data[(col_i*w+ row_i)];
            memcpy(tmp, p1, 3);
            memcpy(p1, p2, 3);
            memcpy(p2, tmp, 3);
            //swap(data[(row_i*w + col_i)], data[(col_i*w + row_i)]);
        }
    }

//    int row, column;
//   for (int x = 0; x < this->w; x++) {
//       for (int y = 0; y < this->h*3; y+=3) {
//           p1 = &data[(x + (h*3 - y)*3)];

//           qDebug("%d", (x +this->h*3 - y));

//           memcpy(&(new_img->data[(x + (this->h*3 - y))]), p1, channels);
//       }
//    }

//    for (int x = 0; x < w; x++) {
//        for (int y = 0; y < h; y++) {
//            p1 = &data[(x + y * w) * channels];
//            p2 = &data[(y + x * h) * channels];

//            memcpy(&(new_img->data[(x + y*w*3)]), p2, channels);
//        }
//    }


//    unsigned char *tempBuffer = new unsigned char[w*h*3];

//    for (int y = 0, destinationColumn = h - 1; y < h; ++y, --destinationColumn) {
//            int offset = y * w;

//            for (int x = 0; x < w; x++) {
//                for (int i = 0; i < 3; i++) {
//                    tempBuffer[(x * h + destinationColumn) * 3 + i] = new_img->data[(offset + x) * 3 + i];
//            }
//        }
//    }

//    memcpy(new_img->data, tempBuffer, w*h*3);

    return *this;
}


