#include <MiniFB.h>
#include "renderer.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

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
      [](int x, int y, Color color)
      {
        //maybe make it "draw" things off screen
        g_buffer[y*g_width + x] = MFB_RGB(color.red, color.green, color.blue);
      },
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
  
  // MAIN LOOP

  uint8_t s = 0;

  do
  {
    r.clear();
    for (int i = 0; i < g_width ; i++)
    {
      for(int j = 0; j < g_height; j++)
      {
        // if(i == j) r.setPixel(i,j,{255,0,0});
        uint8_t red = sin(0.3*s + 0) * 255;
        uint8_t grn = sin(0.3*s + 2) * 255;
        uint8_t blu = sin(0.3*s + 4) * 255;
        r.setPixel(i,j,{red,grn,blu});
      }
    }
    s++;
  } while (mfb_wait_sync(window));

  return 0;
}
