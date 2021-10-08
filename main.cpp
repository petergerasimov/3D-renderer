#include <MiniFB.h>
#include "renderer.hpp"
#include "obj.hpp"
// #include "linalg.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <string.h>
#include <iostream>

static uint32_t g_width = 800;
static uint32_t g_height = 600;
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
        x += g_width/2;
        y += g_height/2;
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
  Obj obj("./teapot.obj");

  vec2df vTemp = obj.getVertices();
  vec2di faces = obj.getFaces();
  std::vector<Vector4f> vertices;
  for(int i = 0; i < vTemp.size(); i++) {
    Vector4f toPush(vTemp[i][0], vTemp[i][1], vTemp[i][2], vTemp[i][3]);
    vertices.push_back(toPush);
  }

  // MAIN LOOP
  int hw = g_width / 2;
  int hh = g_height / 2;
  float s = 0;

  
  std::vector<Vector4f> rotated = vertices;

  do {
    r.clear();
    for (uint i = 0; i < vertices.size(); i++) {
      Matrix<float, 4, 1> rotatedMat;
      rotatedMat = ( r.rotXMat(s) * r.rotYMat(s) * r.rotZMat(s) ) * vertices[i];
      float sf = 100.0f;
      rotatedMat = r.scaleMat({ sf, sf, sf }) * rotatedMat;
      rotatedMat = r.transMat({3,3,3}) * rotatedMat;
      
      rotated[i] = { rotatedMat(0, 0), rotatedMat(1, 0), rotatedMat(2, 0), 1 };
    }
    //aids
    for (const auto& face : faces) {
      Vector4f points[3];
      for (int i = 0; i < 3; i++) {
        points[i] = { rotated[face[i] - 1][0], rotated[face[i] - 1][1], rotated[face[i] - 1][2], 1 };
      }
      r.tri(points, { 255, 255, 255 });
    }
    s+=0.05;
  } while (mfb_wait_sync(window));

  return 0;
}
