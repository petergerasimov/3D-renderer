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
    // static Matrix4f proj = projMat();
    Matrix<float, 4, 1> d = (cameraRotation * toBeProjected);
    // Matrix<float, 4, 1> d = proj * (cameraRotation * toBeProjected);
    // if (d(3,0) != 0.0f)
	// {
	// 	d(0,0) /= d(3,0); d(1,0) /= d(3,0); d(2,0) /= d(3,0);
	// }
    
    Vector2i b;
    b(0) = d(0,0)*20 + 600;
    b(1) = d(1,0)*20 + 300;
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
void Renderer::triFilled(Vector4f pts[3], Color c) {
    int minX = width;
    int maxX = 0;
    int minY = height;
    int maxY = 0;
    Vector2i projected[3];
    // https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
    const static std::function<float(Vector2i p1, Vector2i p2, Vector2i p3)> sign = 
    [](Vector2i p1, Vector2i p2, Vector2i p3) {
        return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1]);
    };
    const static std::function<bool(Vector2i pt, Vector2i v1, Vector2i v2, Vector2i v3)> pointInTri = 
    [&](Vector2i pt, Vector2i v1, Vector2i v2, Vector2i v3) {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    };
    for(int i = 0; i < 3; i++) {
        if(pts[i][2] < camera.pos[2]) return;

        projected[i] = project(pts[i]);
        minX = std::min(minX, projected[i][0]);
        maxX = std::max(maxX, projected[i][0]);
        minY = std::min(minY, projected[i][1]);
        maxY = std::max(maxY, projected[i][1]);
    }
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, width);
    maxY = std::min(maxY, height);
    for(int i = minX; i < maxX; i++) {
        for(int j = minY; j < maxY; j++) {
            Vector2i pt = {i, j};
            if( pointInTri(pt, projected[0], projected[1], projected[2]) ) {
                setPixel(i, j, c);
            }
        }
    }
}
// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
void Renderer::barycentric(const Vector2i& p, Vector2i pts[3], Vector3f& bary)
{
    Vector2i v0 = pts[1] - pts[0], v1 = pts[2] - pts[0], v2 = p - pts[0];
    float den = v0(0) * v1(1) - v1(0) * v0(1);
    bary(1) = (v2(0) * v1(1) - v1(0) * v2(1)) / den;
    bary(2) = (v0(0) * v2(1) - v2(0) * v0(1)) / den;
    bary(0) = 1.0f - bary(1) - bary(2);
    // Make them absolute
    bary = bary.cwiseAbs();
}
void Renderer::triGradient(Vector4f pts[3], Color colA, Color colB, Color colC) {
    int minX = width;
    int maxX = 0;
    int minY = height;
    int maxY = 0;

    Vector2i projected[3];

    for(int i = 0; i < 3; i++) {
        if(pts[i][2] < camera.pos[2]) return;

        projected[i] = project(pts[i]);

        minX = std::min(minX, projected[i][0]);
        maxX = std::max(maxX, projected[i][0]);
        minY = std::min(minY, projected[i][1]);
        maxY = std::max(maxY, projected[i][1]);
    }

    // minX = std::max(minX, 0);
    // minY = std::max(minY, 0);
    // maxX = std::min(maxX, width);
    // maxY = std::min(maxY, height);

    for(int i = minX; i < maxX; i++) {
        for(int j = minY; j < maxY; j++) {
            Vector2i pt = {i, j};
            Vector3f bary = {0, 0, 0};
            barycentric(pt, projected, bary);

            if( (bary(0) + bary(1) + bary(2)) > 1.00001f ) {
                continue;
            }

            Color c = {0, 0, 0};
            for(int k = 0; k < 3; k++) {
                c.bgr[k] = colA.bgr[k] * bary(0) + colB.bgr[k] * bary(1) + colC.bgr[k] * bary(2);
            }

            setPixel(i, j, c);
        }
    }

}
bool Renderer::dirLightColor(const Vector3f& normal, const std::vector<dirLight>& lights, Color& c) {
    std::vector<float> intensities;
    bool existsPositive = false;

    for (auto& light : lights) {
        float intensity = normal.dot(light.getPos());
        intensities.push_back(intensity);
        existsPositive |= (intensity > 0);
    }

    c = {0, 0, 0};

    if (!existsPositive) return false;
      
    for (int i = 0, sz = intensities.size(); i < sz; i++) {
      Color newColor = lights[i].col;

      for (int j = 0; j < 3; j++) {
        newColor.bgr[j] *= (intensities[i] > 0) ? intensities[i] : 0;
        c.bgr[j] = (uint8_t)(std::min(((int)c.bgr[j] + (int)newColor.bgr[j]), 255));
      }
    }

    return true;
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