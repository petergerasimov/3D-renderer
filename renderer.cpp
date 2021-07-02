#include "renderer.hpp"

void Renderer::line(Point a, Point b, Color c)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    int dx = abs(b.x-a.x);
    int sx = a.x<b.x ? 1 : -1;
    int dy = -abs(b.y-a.y);
    int sy = a.y<b.y ? 1 : -1;
    int err = dx+dy;  /* error value e_xy */
    while(true)
    {
        setPixel({a.x, a.y}, c);
        if (a.x == b.x && a.y == b.y) break;
        int e2 = 2*err;
        if (e2 >= dy) /* e_xy+e_x > 0 */
        { 
            err += dy;
            a.x += sx;
        }
        if (e2 <= dx) /* e_xy+e_y < 0 */
        {
            err += dx;
            a.y += sy;
        }
    }
}

Matrix<float> Renderer::transMat(std::vector<float> trans)
{
    Matrix<float> toReturn;
    toReturn = {
        {1,0,0,trans[0]},
        {0,1,0,trans[1]},
        {0,0,1,trans[2]},
        {0,0,0,1}
    };
    return toReturn;

}
Matrix<float> Renderer::scaleMat(std::vector<float> scale)
{
    Matrix<float> toReturn;
    toReturn = {
        {scale[0],0,0,0},
        {0,scale[1],0,0},
        {0,0,scale[2],0},
        {0,0,0,1}
    };
    return toReturn;
}
Matrix<float> Renderer::rotXMat(float angle)
{
    Matrix<float> toReturn;
    toReturn = {
        {1,0,0,0},
        {0,cos(angle),-sin(angle),0},
        {0,sin(angle),cos(angle),0},
        {0,0,0,1}
    };
    return toReturn;
}
Matrix<float> Renderer::rotYMat(float angle)
{
    Matrix<float> toReturn;
    toReturn = {
        {cos(angle),0,sin(angle),0},
        {0,1,0,0},
        {-sin(angle),0,cos(angle),0},
        {0,0,0,1}
    };
    return toReturn;
}
Matrix<float> Renderer::rotZMat(float angle)
{
    Matrix<float> toReturn;
    toReturn = {
        {cos(angle),-sin(angle),0,0},
        {sin(angle),cos(angle),0,0},
        {0,0,1,0},
        {0,0,0,1}
    };
    return toReturn;
}
Matrix<float> Renderer::projMat()
{
    Matrix<float> toReturn;
    toReturn = {
        {atan(fovX/2),0           ,0,0},
        {0           ,atan(fovY/2),0,0},
        {0           ,0,-(zFar+zNear)/(zFar-zNear),-(2*zFar*zNear)/(zFar-zNear)},
        {0,0,-1,0}
    };
    return toReturn;
}