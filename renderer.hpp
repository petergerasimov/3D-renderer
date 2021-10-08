#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cstdint>
#include <functional>
#include <cmath>
// #include "linalg.hpp"
#include <eigen3/Eigen/Core>

using namespace Eigen;

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

struct Camera
{
    Vector4f pos;
    Vector4f rot;
};

class Renderer
{
    private:
        int width;
        int height;
        float aspectRatio;
        float fov = 60;
        float zNear = 0.1f;
        float zFar = 1000.0f;
        Camera camera {
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        Vector3f displaySeurfacePos = { 1.0f, 1.0f, 1.0f };
        Matrix4f cameraRotation;
        float fastSin(float x);
        float fastCos(float x);
    public:
        Renderer(uint32_t width, uint32_t height, std::function<void(int x, int y, Color)> setPixel, std::function<void()> clear) :
            width(width),
            height(height),
            setPixel(setPixel), 
            clear(clear) { 
                setCameraRotation(camera.rot);
                // setCameraPos({ -width / 2.0f, -height / 2.0f, 0 });
                aspectRatio = float(height) / (float)width; 
            };
        std::function<void(int x, int y, Color)> setPixel;
        // void (*setPixel)(int, int, Color);
        std::function<void()> clear;
        void setCameraRotation(Vector4f rot);
        void setCameraPos(Vector4f pos);
        Vector2i project(const Vector4f& a);
        void line(Vector2i a, Vector2i b, Color c);
        void tri(Vector4f pts[3], Color c);
        Matrix4f transMat(Vector3f trans);
        Matrix4f scaleMat(Vector3f scale);
        Matrix4f rotXMat(float angle);
        Matrix4f rotYMat(float angle);
        Matrix4f rotZMat(float angle);
        Matrix4f projMat();
};


#endif //__RENDERER_HPP__