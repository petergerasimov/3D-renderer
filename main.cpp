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

  // Load png
  Image img("texture.png");
  std::cout << img.width << " " << img.height << std::endl;

  //



  std::vector<Vector4f> vertices = obj.getVertices();
  std::vector<Vector4f> vertexNormals = obj.getVertexNormals();
  std::vector<Vector2f> uvs = obj.getUVs();
  vec2di faces = obj.getFaceVertIds();
  vec2di faceNormals = obj.getFaceNormalIds();
  vec2di faceUvs = obj.getFaceTextureIds();

  // Make sure all normals are normalized
  for (Vector4f& norm : vertexNormals) {
    norm(3) = 0;
    norm.normalize();
  }

  // MAIN LOOP
  float s = 0;

  // Add directional lights
  dirLight l1({0,0,-1, 0}, {255, 255, 255}); 
  dirLight l2({0,-1,-1, 0}, {0, 0, 255});
  dirLight l3({-1,-1,0, 0}, {0, 255, 0});
  std::vector<dirLight> lights = {l1, l2, l3};

  Vector4f cameraDir = {0, 0, -1, 0};

  std::vector<Vector4f> rotated = vertices;
  std::vector<Vector4f> rotatedNormals = vertexNormals;

  const bool enableCulling = true;

  do {
    r.clear();
    r.clearZBuff();
    

    // Rotate model
    for (uint i = 0; i < vertices.size(); i++) {
      Matrix4f rot = r.rotXMat(PI) * r.rotXMat(s);

      rotated[i] = rot * vertices[i]; // Rotate first
      rotated[i] = r.transMat({0,20,200}) * rotated[i]; // Translate

      rotatedNormals[i] = rot * vertexNormals[i]; // Rotate normals
    }

    // Draw faces
    for (int i = 0, sz = faces.size(); i < sz; i++) {
      Vector4f points[3];
      Color colors[3];
      Vector2f uvPts[3];

      for (int j = 0; j < 3; j++) {
        points[j] = rotated[faces[i][j] - 1];
        colors[j] = r.dirLightColor(rotatedNormals[faceNormals[i][j] - 1], lights);        
        uvPts[j] = uvs[faceUvs[i][j] - 1];
      }
      
      Vector4f n = (points[2] - points[0]).cross3((points[2] - points[1]));
      n(3) = 0; // just making sure :DD
      n.normalize();
      
      // back-face culling
      if(n.dot(cameraDir) >= 0 || !enableCulling) {
        r.triTextured(points, colors, uvPts, img);
      }

    }
    s += 0.01;
  } while (mfb_wait_sync(window));

  return 0;
}
