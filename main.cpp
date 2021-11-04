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
      [](const int& x, const int& y, const Color& color) {
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
        memset(g_buffer, 255, g_width * g_height * sizeof(*g_buffer));
      }
  );
  
  

  // Load object
  Obj obj("./teapot.obj");

  std::vector<Vector4f> vertices = obj.getVertices();
  std::vector<Vector4f> vertexNormals = obj.getVertexNormals();
  vec2di faces = obj.getFaceVertIds();
  vec2di faceNormals = obj.getFaceNormalIds();

  for (Vector4f& norm : vertexNormals) {
    norm.normalize();
  }

  // MAIN LOOP
  float s = 0;


  dirLight l1({0,0,1}, {255, 0, 0}); 
  dirLight l2({0,1,1}, {0, 0, 255});
  dirLight l3({1,1,0}, {0, 255, 0});

  Vector3f cameraDir = {0, 0, 1};

  std::vector<dirLight> lights = {l1, l2, l3};

  std::vector<Vector4f> rotated = vertices;
  std::vector<Vector4f> rotatedNormals = vertexNormals;

  const bool enableCulling = false;

  do {
    r.clear();
    r.clearZBuff();
    

    for (uint i = 0; i < vertices.size(); i++) {
      Matrix<float, 4, 1> rotatedMat;
      Matrix<float, 4, 1> rotatedMatNorm;
      
      // auto rot = r.rotYMat(s) * r.rotXMat(3.14) * r.rotXMat(s) * r.rotZMat(s);
      auto rot = r.rotYMat(s) * r.rotXMat(PI);

      rotatedMat = rot * vertices[i];
      rotatedMat = r.transMat({0,0,250}) * rotatedMat;

      rotatedMatNorm = rot * vertexNormals[i];
      
      rotated[i] = { rotatedMat(0, 0), rotatedMat(1, 0), rotatedMat(2, 0), 1 };
      rotatedNormals[i] = { rotatedMatNorm(0, 0), rotatedMatNorm(1, 0), rotatedMatNorm(2, 0), 1 };
    }

    for (int i = 0, sz = faces.size(); i < sz; i++) {
      Vector4f points[3];
      bool toDraw = false;
      Color colors[3];

      Vector3f points3f[3];

      for (int j = 0; j < 3; j++) {
        points[j] = rotated[faces[i][j] - 1];
        Vector4f n = rotatedNormals[faceNormals[i][j] - 1];
        Vector3f normal3f = {n[0], n[1], n[2]};

        toDraw |= r.dirLightColor(normal3f, lights, colors[j]);

        
        points3f[j] = {points[j][0], points[j][1], points[j][2]};
      }
      
      auto n = (points3f[2] - points3f[0]).cross((points3f[2] - points3f[1]));
      n.normalize();
      
      // back-face culling
      
      if(n.dot(cameraDir) >= 0 || !enableCulling) {
        // flat shading
        // Color c;
        // if(r.dirLightColor(n, lights, c)) r.triFilled(points, c);

        // smooth shading
        r.triGradient(points, colors);

        // wireframe
        // r.tri(points, {0, 0, 0});
      }

    }
    s += 0.01;

    // Vector4f pts[3] = {{2,0,0,1}, {0,2,0,1}, {4,4,0,1}};
    // r.triGradient(pts, {255,0,0}, {0,255,0}, {0,0,255});
  } while (mfb_wait_sync(window));

  return 0;
}
