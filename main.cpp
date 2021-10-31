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

  std::vector<int> faceIdx(faces.size());
  for(int i = 0; i < faceIdx.size(); i++) {
    faceIdx[i] = i;
  }


  // MAIN LOOP
  int hw = g_width / 2;
  int hh = g_height / 2;
  float s = 0;


  dirLight l1({0,0,1}, {255, 0, 0}); 
  dirLight l2({0,1,1}, {0, 0, 255});

  std::vector<dirLight> lights = {l1, l2};

  std::vector<Vector4f> rotated = vertices;
  std::vector<Vector4f> rotatedNormals = vertexNormals;

  do {
    r.clear();

    

    for (uint i = 0; i < vertices.size(); i++) {
      Matrix<float, 4, 1> rotatedMat;
      Matrix<float, 4, 1> rotatedMatNorm;
      
      rotatedMat = ( r.rotYMat(s) * r.rotXMat(3.14) ) * vertices[i];
      rotatedMat = r.transMat({0,2,3}) * rotatedMat;

      rotatedMatNorm = ( r.rotYMat(s) * r.rotXMat(3.14) ) * vertexNormals[i];
      rotatedMatNorm = r.transMat({0,2,3}) * rotatedMatNorm;
      
      rotated[i] = { rotatedMat(0, 0), rotatedMat(1, 0), rotatedMat(2, 0), 1 };
      rotatedNormals[i] = { rotatedMatNorm(0, 0), rotatedMatNorm(1, 0), rotatedMatNorm(2, 0), 1 };
    }

    std::sort(faceIdx.begin(), faceIdx.end(), [&](const int& aId, const int& bId) { 
        std::vector<int>& a = faces[aId];
        std::vector<int>& b = faces[bId];
        float aAvg = (rotated[a[0] - 1][2] + rotated[a[1] - 1][2] + rotated[a[2] - 1][2]) / 3.0f;
        float bAvg = (rotated[b[0] - 1][2] + rotated[b[1] - 1][2] + rotated[b[2] - 1][2]) / 3.0f;
        return aAvg < bAvg;
    } );

    for (const auto& id : faceIdx) {
      std::vector<int> face = faces[id];
      Vector4f points[3];
      Vector3f points3f[3];
      for (int i = 0; i < 3; i++) {
        points[i] = rotated[face[i] - 1];
        points3f[i] = {points[i][0], points[i][1], points[i][2]};
      }
      auto n = (points3f[2] - points3f[0]).cross((points3f[2] - points3f[1]));
      n.normalize();

      
      Color c;
      if(r.dirLightColor(n, lights, c)) r.triFilled(points, c);
    }
    s+=0.1;

    Vector4f pts[3] = {{2,0,0,1}, {0,2,0,1}, {4,4,0,1}};
    r.triGradient(pts, {255,0,0}, {0,255,0}, {0,0,255});
  } while (mfb_wait_sync(window));

  return 0;
}
