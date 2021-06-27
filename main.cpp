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

  struct mfb_window *window = mfb_open_ex("Renderer", g_width, g_height, WF_RESIZABLE);
  if (!window)
    return 0;

  g_buffer = (uint32_t *)malloc(g_width * g_height * 4);
  mfb_set_resize_callback(window, resize);

  mfb_set_viewport(window, 50, 50, g_width - 50 - 50, g_height - 50 - 50);
  resize(window, g_width - 100, g_height - 100); // to resize buffer

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
      });

  // MAIN LOOP
  do
  {
    
    for (int i = 0; i < g_width ; i++)
    {
      for(int j = 0; j < g_height; j++)
      {
        // if(i == j) r.setPixel(i,j,{255,0,0});
        r.line(i,j,6,10,{255,255,0});
      }
    }
    r.clear();
  } while (mfb_wait_sync(window));

  return 0;
}
