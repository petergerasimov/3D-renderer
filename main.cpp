#include <MiniFB.h>
#include "renderer.hpp"
#include "obj.hpp"
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

using namespace Eigen;

// #include "linalg.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <string.h>
#include <iostream>

static uint32_t g_width = 1280;
static uint32_t g_height = 720;
static uint32_t *g_buffer = 0x0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void resize(struct mfb_window *window, int width, int height)
{
  (void)window;
  g_width = width;
  g_height = height;
  g_buffer = (uint32_t *)realloc(g_buffer, g_width * g_height * 4);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  //INIT
  struct mfb_window *window = mfb_open_ex("Renderer", g_width, g_height, WF_RESIZABLE);
  if (!window)
    return 0;

  g_buffer = (uint32_t*) malloc(g_width * g_height * 4);
  mfb_set_resize_callback(window, resize);

  mfb_set_viewport(window, 0, 0, g_width, g_height);
  resize(window, g_width, g_height); // to resize buffer

  mfb_update_state state;

  static Renderer r(
      g_width,
      g_height,
      //set pixel
      [](int x, int y, Color color) {
        //maybe make it "draw" things off screen
        // x += g_width/2;
        // y += g_height/2;
        if(y >= 0 && y < (int)g_height && x >= 0 && x < (int)g_width) {
          g_buffer[y * g_width + x] = color.i;
        }
      },
      //clear screen
      [&]() {
        state = mfb_update_ex(window, g_buffer, g_width, g_height);
        if (state != STATE_OK) {
          window = 0x0;
          exit(0);
        }
        memset(g_buffer, 0, g_width * g_height * sizeof(*g_buffer));
      }
  );
  
  

  // Load object
  Obj obj("./bunny.obj");

  std::vector<Vector4f> vertices = obj.getVertices();
  std::vector<Vector4f> vertexNormals = obj.getVertexNormals();
  vec2di faces = obj.getFaceVertIds();
  vec2di faceNormals = obj.getFaceNormalIds();


  // MAIN LOOP
  int hw = g_width / 2;
  int hh = g_height / 2;
  float s = 0;

  Vector3f light = {0,0,1};
  Color lightColor = {255, 0, 0};
  light.normalize();

  Vector3f light2 = {0,1,1};
  Color lightColor2 = {0, 0, 255};
  light2.normalize();

  std::vector<Vector4f> rotated = vertices;

  do {
    r.clear();
    for (uint i = 0; i < vertices.size(); i++) {
      Matrix<float, 4, 1> rotatedMat;
      
      rotatedMat = ( r.rotYMat(s) * r.rotXMat(3.14) ) * vertices[i];
      rotatedMat = r.transMat({0,2,3}) * rotatedMat;
      // rotatedMat = r.scaleMat({100,100,100}) * rotatedMat;
      
      
      
      rotated[i] = { rotatedMat(0, 0), rotatedMat(1, 0), rotatedMat(2, 0), 1 };
    }
    //aids
    std::sort(faces.begin(), faces.end(), [&](const std::vector<int>& a, const std::vector<int>& b) { 
        float aAvg = (rotated[a[0] - 1][2] + rotated[a[1] - 1][2] + rotated[a[2] - 1][2]) / 3.0f;
        float bAvg = (rotated[b[0] - 1][2] + rotated[b[1] - 1][2] + rotated[b[2] - 1][2]) / 3.0f;
        return aAvg < bAvg;
    } );
    for (const auto& face : faces) {
      Vector4f points[3];
      Vector3f points3f[3];
      for (int i = 0; i < 3; i++) {
        points[i] = rotated[face[i] - 1];
        points3f[i] = {points[i][0], points[i][1], points[i][2]};
      }
      auto n = (points3f[2] - points3f[0]).cross((points3f[2] - points3f[1]));
      n.normalize();
      float intensity = n.dot(light);
      float intensity2 = n.dot(light2);


      if(intensity > 0 || intensity2 > 0)
      {
        Color newColor = lightColor;
        Color newColor2 = lightColor2;
        Color twoLights;
        for(int i = 0; i < 3; i++)
        {
          newColor.bgr[i] *= (intensity > 0) ? intensity : 0;
          newColor2.bgr[i] *= (intensity2 > 0) ? intensity2 : 0;
          twoLights.bgr[i] = (uint8_t)(std::min(((int)newColor.bgr[i] + (int)newColor2.bgr[i]), 255));
          // twoLights.bgr[i] = (uint8_t)(((int)newColor.bgr[i] + (int)newColor2.bgr[i])/2);
        }

        r.triFilled(points, twoLights);
      }
      
    }
    s+=0.01;
  } while (mfb_wait_sync(window));

  return 0;
}
