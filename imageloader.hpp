#include <stdio.h>
#include <string>
#include <vector>
#include "renderer.hpp"

class Image {
    public:
        
        int width, height;
        std::vector<Color> imgData;
        Image(std::string path) {
            std::string cmd = "convert " + path + " out.bmp";
            system(cmd.c_str());
            readBMP("./out.bmp");
            system("rm out.bmp");
        }
    private:
        // ty https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file/38440684
        void readBMP(char* filename)
        {
            int i;
            FILE* f = fopen(filename, "rb");
            unsigned char info[54];

            // read the 54-byte header
            fread(info, sizeof(unsigned char), 54, f); 

            // extract image height and width from header
            width = *(int*)&info[18];
            height = *(int*)&info[22];

            // allocate 3 bytes per pixel
            int size = 3 * width * height;
            unsigned char* data = new unsigned char[size];

            // read the rest of the data at once
            fread(data, sizeof(unsigned char), size, f); 
            fclose(f);

            for(i = 0; i < size; i += 3)
            {
                // flip the order of every 3 bytes
                unsigned char tmp = data[i];
                data[i] = data[i+2];
                data[i+2] = tmp;
            }
            imgData.resize(size / 3);
            for(i = 0; i < size; i += 3)
            {
                int idx = i / 3;
                imgData[idx].bgr[0] = data[i + 2];
                imgData[idx].bgr[1] = data[i + 1];
                imgData[idx].bgr[2] = data[i];
            }
        }
};