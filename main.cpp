#include <MiniFB.h>
#include "renderer.hpp"
// #include "linalg.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <string.h>

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

  g_buffer = (uint32_t *)malloc(g_width * g_height * 4);
  mfb_set_resize_callback(window, resize);

  mfb_set_viewport(window, 0, 0, g_width, g_height);
  resize(window, g_width, g_height); // to resize buffer

  mfb_update_state state;

  static Renderer r(
      g_width,
      g_height,
      //set pixel
      [](Point p, Color color) 
      {
        //maybe make it "draw" things off screen
        if(p.y >= 0 && p.y < (int)g_height && p.x >= 0 && p.x < (int)g_width)
        {
          g_buffer[p.y * g_width + p.x] = color.i;
        }
      },
      //clear screen
      [&]() 
      {
        state = mfb_update_ex(window, g_buffer, g_width, g_height);
        if (state != STATE_OK)
        {
          window = 0x0;
          exit(0);
        }
        memset(g_buffer, 0, g_width*g_height*sizeof(*g_buffer));
      }
  );
  
  

  // CUBE

  std::vector<std::vector<float>> points = {
    { 1, 1, 1, 1},
    { 1,-1, 1, 1},
    {-1,-1, 1, 1},
    {-1, 1, 1, 1},
    { 1, 1,-1, 1},
    { 1,-1,-1, 1},
    {-1,-1,-1, 1},
    {-1, 1,-1, 1}
  };

  // MAIN LOOP

  int hw = g_width/2;
  int hh = g_height/2;
  float s = 0;

  Matrix<float> projected;

  do
  {
    r.clear();
    std::vector<Point> pts(8);
    for(uint i = 0; i < points.size(); i++)
    {
      projected = ( r.rotXMat(s) * ( r.rotYMat(s) * r.rotZMat(s) ) ) * points[i];
      //[View To Projection]x[World To View]x[Model to World]=[ModelViewProjectionMatrix].
     
      projected = r.projMat() * projected;
      float sf = 100.0f;
      projected = r.scaleMat({sf,sf,sf}) * projected;
      float z = projected[2][0];
      //fix this bs with https://en.wikipedia.org/wiki/3D_projection
      int x = (int)(projected[0][0]/(z/sf)) + hw;
      int y = (int)(projected[1][0]/(z/sf)) + hh;
      // int x = (int)(projected[0][0]) + hw;
      // int y = (int)(projected[1][0]) + hh;
      r.setPixel({x,y},{255,255,255});
      pts[i] = {x,y};
    }
    //aids
    for(int i = 0; i < 4; i++)
    {
      r.line(pts[i],pts[i+4],{255,255,255});
    }
    for(int i = 0; i < 3; i++)
    {
      r.line(pts[i],pts[i+1],{255,255,255});
    }
    r.line(pts[3],pts[0],{255,255,255});
    for(int i = 4; i < 7; i++)
    {
      r.line(pts[i],pts[i+1],{255,255,255});
    }
    r.line(pts[7],pts[4],{255,255,255});
    //temp testing bs will remove
    s+=0.01;
  } while (mfb_wait_sync(window));

  return 0;
}
