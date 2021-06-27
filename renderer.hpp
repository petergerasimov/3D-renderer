#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cstdint>
#include <functional>

struct Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {};
};

class Renderer
{
    public:
        Renderer(uint32_t width, uint32_t height, std::function<void(int,int,Color)> setPixel, std::function<void()> clear) :
            width(width),
            height(height),
            setPixel(setPixel), 
            clear(clear){};
        std::function<void(int,int,Color)> setPixel;
        // void (*setPixel)(int, int, Color);
        std::function<void()> clear;
        void line(int x1, int y1, int x2, int y2, Color c);
    private:
        int width;
        int height;
};


#endif //__RENDERER_HPP__