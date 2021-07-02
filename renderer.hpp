#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cstdint>
#include <functional>
#include <cmath>
#include "linalg.hpp"

#define RGB(r, g, b)    (((uint32_t) r) << 16) | (((uint32_t) g) << 8) | ((uint32_t) b)

//http://www.codinglabs.net/article_world_view_projection_matrix.aspx
//https://github.com/ssloy/tinyrenderer/wiki
//https://www.youtube.com/watch?v=cvcAjgMUPUA
//https://github.com/emoon/minifb

#define PI 3.14159f

union Color
{
    uint8_t bgr[3];
    uint32_t i;
};

struct Point
{
    int x;
    int y;
};

class Renderer
{
    public:
        Renderer(uint32_t width, uint32_t height, std::function<void(Point,Color)> setPixel, std::function<void()> clear) :
            width(width),
            height(height),
            setPixel(setPixel), 
            clear(clear) {};
        std::function<void(Point,Color)> setPixel;
        // void (*setPixel)(int, int, Color);
        std::function<void()> clear;
        void line(Point a, Point b, Color c);
        Matrix<float> transMat(std::vector<float> trans);
        Matrix<float> scaleMat(std::vector<float> scale);
        Matrix<float> rotXMat(float angle);
        Matrix<float> rotYMat(float angle);
        Matrix<float> rotZMat(float angle);
        Matrix<float> projMat();
    private:
        int width;
        int height;
        float fovY = PI/2;
        float fovX = PI/2;
        float zNear = 1.0f;
        float zFar = 100.0f;
};


#endif //__RENDERER_HPP__