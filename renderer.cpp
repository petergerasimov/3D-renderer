#include "renderer.hpp"
#include <iostream>

void Renderer::setCameraRotation(Vector4f rot) {
    camera.rot = rot;
    cameraRotation = (rotXMat(rot[0]) * rotYMat(rot[1]) * rotZMat(rot[2]));
}
void Renderer::setCameraPos(Vector4f pos)
{
    camera.pos = pos;
}
Vector2i Renderer::project(const Vector4f& a) {
    Vector4f toBeProjected = a - camera.pos;
    static Matrix4f proj = projMat();
    Matrix<float, 4, 1> d = proj * (cameraRotation * toBeProjected);
    if (d(3,0) != 0.0f)
	{
		d(0,0) /= d(3,0); d(1,0) /= d(3,0); d(2,0) /= d(3,0);
	}
    
    Vector2i b;
    b(0) = d(0,0)*10 + 600;
    b(1) = d(1,0)*10 + 600;
    // b(0) = d(0,0);
    // b(1) = d(1,0);
    // std::cout << b << std::endl;
    b(0) = (b(0) < 0) ? std::max(b(0), -(int)width) : std::min(b(0), (int)width);
    b(1) = (b(1) < 0) ? std::max(b(1), -(int)height) : std::min(b(1), (int)height);
    return b;
}
void Renderer::line(Vector2i a, Vector2i b, Color c)
{
    // http://www.edepot.com/linee.html
    bool yLonger = false;
	int shortLen = b(1)- a(1);
	int longLen = b(0)- a(0);
	if (abs(shortLen) > abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;				
		yLonger = true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=a(1);
			for (int j=0x8000 + (a(0) << 16);a(1) <= longLen;++a(1)) {
				setPixel(j >> 16, a(1) , c);	
				j+=decInc;
			}
			return;
		}
		longLen += a(1);
		for (int j = 0x8000 + (a(0) << 16);a(1) >= longLen;--a(1)) {
			setPixel(j >> 16, a(1), c);	
			j-=decInc;
		}
		return;	
	}

	if (longLen > 0) {
		longLen += a(0);
		for (int j = 0x8000 + (a(1) << 16);a(0) <= longLen;++a(0)) {
			setPixel(a(0), j >> 16, c);
			j+=decInc;
		}
		return;
	}
	longLen += a(0);
	for (int j = 0x8000+(a(1) << 16);a(0) >= longLen;--a(0)) {
		setPixel(a(0),j >> 16, c);
		j-=decInc;
	}
}
void Renderer::tri(Vector4f pts[3], Color color)
{
    Vector2i a = project(pts[0]);
    Vector2i b = project(pts[1]);
    Vector2i c = project(pts[2]);
    line(a, b, color);
    line(b, c, color);
    line(c, a, color);
}
Matrix4f Renderer::transMat(Vector3f trans)
{
    Matrix4f toReturn {
        {1,0,0,trans(0)},
        {0,1,0,trans(1)},
        {0,0,1,trans(2)},
        {0,0,0,1}
    };
    return toReturn;

}
Matrix4f Renderer::scaleMat(Vector3f scale)
{
    Matrix4f toReturn {
        {scale(0),0,0,0},
        {0,scale(1),0,0},
        {0,0,scale(2),0},
        {0,0,0,1}
    };
    return toReturn;
}
Matrix4f Renderer::rotXMat(float angle)
{
    
    float s = fastSin(angle);
    float c = fastCos(angle);
    Matrix4f toReturn {
        {1,0, 0,0},
        {0,c,-s,0},
        {0,s, c,0},
        {0,0, 0,1}
    };
    return toReturn;
}
Matrix4f Renderer::rotYMat(float angle)
{
    float s = fastSin(angle);
    float c = fastCos(angle);
    Matrix4f toReturn {
        { c,0,s,0},
        { 0,1,0,0},
        {-s,0,c,0},
        { 0,0,0,1}
    };
    return toReturn;
}
Matrix4f Renderer::rotZMat(float angle)
{
    
    float s = fastSin(angle);
    float c = fastCos(angle);
    Matrix4f toReturn {
        {c,-s,0,0},
        {s, c,0,0},
        {0, 0,1,0},
        {0, 0,0,1}
    };
    return toReturn;
}
Matrix4f Renderer::projMat()
{
    
    float invTan = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
    Matrix4f toReturn {
        {aspectRatio * invTan,0     ,0                    ,0},
        {0                   ,invTan,0                    ,0},
        {0                   ,0     ,zFar / (zFar - zNear),1},
        {0                   ,0,(-zFar*zNear) / (zFar - zNear),0}
    };
    return toReturn;
}

float Renderer::fastSin(float x) {
    float t = x * 0.15915f;
    t = t - (int)t;
    return 20.785f * (t - 0.0f) * (t - 0.5f) * (t - 1.0f);
}
float Renderer::fastCos(float x) {
    return fastSin(x + PI/2);
}