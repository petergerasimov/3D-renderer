#include "renderer.hpp"
#include <iostream>
#include <limits>

void Renderer::clearZBuff() {
    const float min = -std::numeric_limits<float>::max();
    
    // memset(zBuffer, d.i, (width * height) * sizeof(*zBuffer));
    std::fill(zBuffer, zBuffer + (width * height), min);
    // std::cout << zBuffer[0] << std::endl;
}
void Renderer::setCameraRotation(const Vector4f& rot) {
    camera.rot = rot;
    cameraRotation = (rotXMat(rot[0]) * rotYMat(rot[1]) * rotZMat(rot[2]));
}
void Renderer::setCameraPos(const Vector4f& pos)
{
    camera.pos = pos;
}
Vector2i Renderer::project(Vector4f a, float& w) {
    // a -= camera.pos;
    const static Matrix4f proj = projMat();
    // a = proj * (cameraRotation * a);
    a = proj * a;

    w = a(3);
    a /= w;
    a(0) *= -1;
    a(1) *= -1;

    Vector4f offset = {1, 1, 0 , 0};
    a += offset;

    a(0) *= 0.5f * width; 
    a(1) *= 0.5f * height; 

    Vector2i b;
    b(0) = a(0);
    b(1) = a(1);
    b(0) = (b(0) < 0) ? std::max(b(0), -(int)width) : std::min(b(0), (int)width);
    b(1) = (b(1) < 0) ? std::max(b(1), -(int)height) : std::min(b(1), (int)height);
    return b;
}
void Renderer::line(Vector2i a, Vector2i b, const Color& c)
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
void Renderer::tri(Vector4f pts[3], const Color& color)
{
    Vector2i a = project(pts[0]);
    Vector2i b = project(pts[1]);
    Vector2i c = project(pts[2]);
    line(a, b, color);
    line(b, c, color);
    line(c, a, color);
}
void Renderer::triFilled(Vector4f pts[3], const Color& c) {
    Color cols[3] = {c, c, c};
    triGradient(pts, cols);
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
void Renderer::triGradient(Vector4f pts[3], Color cols[3]) {
    int minX = width;
    int maxX = 0;
    int minY = height;
    int maxY = 0;

    Vector2i projected[3];
    float w[3] = { 0, };

    for(int i = 0; i < 3; i++) {
        // if(pts[i][2] < camera.pos[2]) return;
        
        projected[i] = project(pts[i], w[i]);

        minX = std::min(minX, projected[i][0]);
        maxX = std::max(maxX, projected[i][0]);
        minY = std::min(minY, projected[i][1]);
        maxY = std::max(maxY, projected[i][1]);
    }

    for(int i = minX; i < maxX; i++) {
        for(int j = minY; j < maxY; j++) {
            Vector2i pt = {i, j};
            Vector3f bary = {0, 0, 0};
            barycentric(pt, projected, bary);

            if( (bary(0) + bary(1) + bary(2)) <= 1.00001f ) {
                Color c = {0, 0, 0};
                
                for(int k = 0; k < 3; k++) {
                    c.bgr[k] = cols[0].bgr[k] * bary(0) + cols[1].bgr[k] * bary(1) + cols[2].bgr[k] * bary(2);
                }

                // float pixelZ = pts[0](2) * bary(0) + pts[1](2) * bary(1) + pts[2](2) * bary(2);
                float pixelZ = w[0] * bary(0) + w[1] * bary(1) + w[2] * bary(2);
                // std::cout << zBuffer[0] << std::endl;

                int pos = j * width + i;

                if (pixelZ > zBuffer[pos])
				{
					setPixel(i, j, c);
					zBuffer[pos] = pixelZ;
				}
                
            }
        }
    }

}
void Renderer::triTextured(Vector4f pts[3], Color cols[3], Vector2f uv[3], Image& img) {
    int minX = width;
    int maxX = 0;
    int minY = height;
    int maxY = 0;

    Vector2i projected[3];
    float w[3] = { 0, };

    for(int i = 0; i < 3; i++) {
        // if(pts[i][2] < camera.pos[2]) return;
        
        projected[i] = project(pts[i], w[i]);
        // projected[i] = {(int)(pts[i][0]*5 + 600), (int)(pts[i][1]*5 + 300)};

        minX = std::min(minX, projected[i][0]);
        maxX = std::max(maxX, projected[i][0]);
        minY = std::min(minY, projected[i][1]);
        maxY = std::max(maxY, projected[i][1]);
    }

    for(int i = minX; i < maxX; i++) {
        for(int j = minY; j < maxY; j++) {
            Vector2i pt = {i, j};
            Vector3f bary = {0, 0, 0};
            barycentric(pt, projected, bary);
            int pos = j * width + i;
            float pixelZ = w[0] * bary(0) + w[1] * bary(1) + w[2] * bary(2);
            if( (bary(0) + bary(1) + bary(2)) <= 1.00001f && pixelZ > zBuffer[pos]) {
                Color c = {0, 0, 0};
                
                Vector2f pUV = uv[0] * bary(0) + uv[1] * bary(1) + uv[2] * bary(2);
                int u = pUV(0)*img.width;
                int v = pUV(1)*img.height;
                Color tc = img.imgData[v*img.width + u];
                // c = tc;
                for(int k = 0; k < 3; k++) {
                    float tcf = tc.bgr[k] / 255.0f;
                    c.bgr[k] = cols[0].bgr[k] * bary(0) + cols[1].bgr[k] * bary(1) + cols[2].bgr[k] * bary(2);
                    c.bgr[k] *= tcf;
                }

				setPixel(i, j, c);
				zBuffer[pos] = pixelZ;
                
            }
        }
    }

}
Color Renderer::dirLightColor(const Vector4f& normal, const std::vector<dirLight>& lights) {
    std::vector<float> intensities;
    bool existsPositive = false;

    for (auto& light : lights) {
        float intensity = normal.dot(-light.getDir());
        intensities.push_back(intensity);
        existsPositive |= (intensity > 0);
    }

    Color c = {0, 0, 0};

    if (!existsPositive) return c;
      
    for (int i = 0, sz = intensities.size(); i < sz; i++) {
      Color newColor = lights[i].col;

      for (int j = 0; j < 3; j++) {
        newColor.bgr[j] *= std::min( std::max(intensities[i], 0.0f), 1.0f);
        c.bgr[j] = (uint8_t)(std::min(((int)c.bgr[j] + (int)newColor.bgr[j]), 255));
      }
    }

    return c;
}
Matrix4f Renderer::transMat(const Vector3f& trans)
{
    Matrix4f toReturn {
        {1,0,0,trans(0)},
        {0,1,0,trans(1)},
        {0,0,1,trans(2)},
        {0,0,0,1}
    };
    return toReturn;

}
Matrix4f Renderer::scaleMat(const Vector3f& scale)
{
    Matrix4f toReturn {
        {scale(0),0,0,0},
        {0,scale(1),0,0},
        {0,0,scale(2),0},
        {0,0,0,1}
    };
    return toReturn;
}
Matrix4f Renderer::rotXMat(const float& angle)
{
    
    float s = sinf(angle);
    float c = cosf(angle);
    Matrix4f toReturn {
        {1,0, 0,0},
        {0,c,-s,0},
        {0,s, c,0},
        {0,0, 0,1}
    };
    return toReturn;
}
Matrix4f Renderer::rotYMat(const float& angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    Matrix4f toReturn {
        { c,0,s,0},
        { 0,1,0,0},
        {-s,0,c,0},
        { 0,0,0,1}
    };
    return toReturn;
}
Matrix4f Renderer::rotZMat(const float& angle)
{
    
    float s = sinf(angle);
    float c = cosf(angle);
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
        {0                   ,0,(-zFar * zNear) / (zFar - zNear),0}
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