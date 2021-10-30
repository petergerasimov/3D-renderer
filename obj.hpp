#ifndef __OBJ_HPP__
#define __OBJ_HPP__

#include <string>
#include <vector>
#include <fstream>
#include <eigen3/Eigen/Core>

using namespace Eigen;

typedef std::vector<std::vector<int>> vec2di;

class Obj {
    private:
        std::vector<Vector4f> vertices;
        std::vector<Vector4f> vertexNormals;
        vec2di faceVertIds;
        vec2di faceNormalIds;
        vec2di faceTextureIds;
        void parse(std::ifstream& file);
    public:
        Obj(std::string path);
        std::vector<Vector4f> getVertices();
        std::vector<Vector4f> getVertexNormals();
        vec2di getFaceVertIds();
        vec2di getFaceNormalIds();
        vec2di getFaceTextureIds();
        //

};

#endif //__OBJ_HPP__