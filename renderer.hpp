#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cstdint>
#include <functional>
#include <vector>
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

union Color {
    uint8_t bgr[3];
    uint32_t i;
};

struct Camera {
    Vector4f pos;
    Vector4f rot;
};

class dirLight {
    private:
        Vector3f pos;
    public:
        dirLight(Vector3f pos, Color col) : pos(pos), col(col) { this->pos.normalize(); };
        void setPos(const Vector3f& pos) { this->pos = pos.normalized(); };
        const Vector3f& getPos() const { return pos; }
        Color col;
};

class Renderer
{
    private:
        int width;
        int height;
        float* zBuffer;
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
        Renderer(uint32_t width, uint32_t height, std::function<void(const int&, const int&, const Color&)> setPixel, std::function<void()> clear) :
            width(width),
            height(height),
            setPixel(setPixel), 
            clear(clear) { 
                setCameraRotation(camera.rot);
                // setCameraPos({ -width / 2.0f, -height / 2.0f, 0 });
                aspectRatio = float(height) / (float)width; 
                zBuffer = new float[width * height];
            };
        ~Renderer() { delete[] zBuffer; }
        void clearZBuff();
        std::function<void(const int& x, const int& y, const Color&)> setPixel;
        // void (*setPixel)(int, int, Color);
        std::function<void()> clear;
        void setCameraRotation(const Vector4f& rot);
        void setCameraPos(const Vector4f& pos);
        Vector2i project(const Vector4f& a);
        void line(Vector2i a, Vector2i b,const Color& c);
        void tri(Vector4f pts[3], const Color& c);
        void triFilled(Vector4f pts[3], const Color& c);
        void barycentric(const Vector2i& p, Vector2i pts[3], Vector3f& bary);
        void triGradient(Vector4f pts[3], Color cols[3]);
        bool dirLightColor(const Vector3f& normal, const std::vector<dirLight>& lights, Color& c);
        Matrix4f transMat(const Vector3f& trans);
        Matrix4f scaleMat(const Vector3f& scale);
        Matrix4f rotXMat(const float& angle);
        Matrix4f rotYMat(const float& angle);
        Matrix4f rotZMat(const float& angle);
        Matrix4f projMat();
};


#endif //__RENDERER_HPP__