#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cstdint>
#include <functional>
#include <vector>
#include <cmath>
#include <fstream>
#include <iterator>
#include <iostream>
// #include "linalg.hpp"
#include <eigen3/Eigen/Core>

using namespace Eigen;

#define RGB(r, g, b) (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | ((uint32_t)b)

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

class Image
{
    public:
        int width, height;
        std::vector<Color> imgData;
        Image(std::string path)
        {
            std::string cmd = "convert " + path + " out.bmp";
            if(system(cmd.c_str()) == -1) std::cout << "command failed";
            readBMP("./out.bmp");
            if(system("rm out.bmp") == -1) std::cout << "command failed";
        }

    private:
        // ty https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file/38440684
        void readBMP(const std::string &filename)
        {
            static const size_t HEADER_SIZE = 54;

            std::ifstream bmp(filename, std::ios::binary);

            std::array<char, HEADER_SIZE> header;
            bmp.read(header.data(), header.size());

            size_t dataOffset = *(uint32_t*)(&header[10]);
            width = *(int*)(&header[18]);
            height = *(int*)(&header[22]);

            std::vector<char> img(dataOffset - HEADER_SIZE);
            bmp.read(img.data(), img.size());

            size_t dataSize = ((width * 3 + 3) & (~3)) * height;
            img.resize(dataSize);
            bmp.read(img.data(), img.size());

            size_t newSz = dataSize / 3;
            imgData.resize(newSz);
            for (size_t i = 0; i < newSz; i++)
            {   
                imgData[i].i = *(int*)(&img[i * 3]);
            }
        }
};

struct Camera {
    Vector4f pos;
    Vector4f rot;
};

class dirLight {
    private:
        Vector4f dir;

    public:
        dirLight(Vector4f dir, Color col) : dir(dir), col(col) { this->dir.normalize(); };
        void setDir(const Vector4f &dir) { this->dir = dir.normalized(); };
        const Vector4f &getDir() const { return dir; }
        Color col;
};

class Renderer {
    private:
        int width;
        int height;
        float *zBuffer;
        float aspectRatio;
        float fov = 160;
        float zNear = 0.1f;
        float zFar = 1000.0f;
        Camera camera {
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
        Vector3f displaySeurfacePos = {1.0f, 1.0f, 1.0f};
        Matrix4f cameraRotation;
        float fastSin(float x);
        float fastCos(float x);

    public:
        Renderer(uint32_t width, uint32_t height, 
        std::function<void(const int &, const int &, const Color &)> setPixel, 
        std::function<void()> clear) : 
        width(width), height(height), setPixel(setPixel), clear(clear) {
            setCameraRotation(camera.rot);
            // setCameraPos({ -width / 2.0f, -height / 2.0f, 0 });
            aspectRatio = float(height) / (float)width;
            zBuffer = new float[width * height];
        };
        ~Renderer() { delete[] zBuffer; }
        void clearZBuff();
        std::function<void(const int &x, const int &y, const Color &)> setPixel;
        std::function<void()> clear;
        void setCameraRotation(const Vector4f &rot);
        void setCameraPos(const Vector4f &pos);
        Vector2i project(const Vector4f &a)
        {
            float w = 0.0f;
            return project(a, w);
        }
        Vector2i project(Vector4f a, float &w);
        void line(Vector2i a, Vector2i b, const Color &c);
        void tri(Vector4f pts[3], const Color &c);
        void triFilled(Vector4f pts[3], const Color &c);
        void barycentric(const Vector2i &p, Vector2i pts[3], Vector3f &bary);
        void triGradient(Vector4f pts[3], Color cols[3]);
        void triTextured(Vector4f pts[3], Color cols[3], Vector2f uv[3], Image &img);
        Color dirLightColor(const Vector4f &normal, const std::vector<dirLight> &lights);
        Matrix4f transMat(const Vector3f &trans);
        Matrix4f scaleMat(const Vector3f &scale);
        Matrix4f rotXMat(const float &angle);
        Matrix4f rotYMat(const float &angle);
        Matrix4f rotZMat(const float &angle);
        Matrix4f projMat();
};

#endif //__RENDERER_HPP__