#ifndef __RENDERER_H__
#define __RENDERER_H__

class Renderer
{
    public:
        Renderer(int width, int height, void (*setPixel)(int, int, int), void (*clear)()) :
            width(width),
            height(height),
            setPixel(setPixel), 
            clear(clear){};
        void (*setPixel)(int, int, int);
        void (*clear)();
        void line(int x1, int y1, int x2, int y2);
    private:
        int width;
        int height;
};


#endif //__RENDERER_H__