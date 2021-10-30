#include "obj.hpp"

#include <sstream>
#include <functional>
#include <iostream>


typedef std::stringstream sstream;

Obj::Obj(std::string path) {
    std::ifstream file(path);
    parse(file);
    file.close();
}
void Obj::parse(std::ifstream& file) {
    std::string line;
    std::function<void(sstream& ss, std::vector<Vector4f>& v)> addVertices = [&](sstream& ss, std::vector<Vector4f>& v) {
        Vector4f empty = {0,0,0,1};
        v.push_back(empty);
        for (int i = 0; !ss.eof(); i++) {
            float current;
            ss >> current;
            v.back()(i) = current;
        }
    };
    std::function<void(sstream& ss)> addFaces = [&](sstream& ss) {
        std::vector<int> empty;

        faceVertIds.push_back(empty);
        faceNormalIds.push_back(empty);
        faceTextureIds.push_back(empty);

        for (int i = 0; !ss.eof(); i++) {
            std::string current;
            ss >> current;
            std::string delim = "/";
            
            if(current.find(delim) != std::string::npos) {
                // Get vertex ids 
                float faceVert = std::stof(current.substr(0, current.find(delim)));
                faceVertIds.back().push_back(faceVert);
                // Delete vertex ids
                current.erase(0, current.find(delim) + delim.length());
                // Get texture ids
                std::string faceTextureStr = current.substr(0, current.find(delim));
                if(!faceTextureStr.empty()) {
                  faceTextureIds.back().push_back(std::stof(faceTextureStr));
                }
                // Delete texture ids
                current.erase(0, current.find(delim) + delim.length());
                // Add normal ids
                faceNormalIds.back().push_back(std::stof(current));
            }
            else {
                faceVertIds.back().push_back(std::stof(current));
            }
        }
    };
    
    while (std::getline(file, line)) {
        
        sstream ss(line);
        std::string identifier;
        ss >> identifier;
        
        if(!identifier.compare("v")) addVertices(ss, vertices);
        if(!identifier.compare("vn")) addVertices(ss, vertexNormals);
        if(!identifier.compare("f")) addFaces(ss);
        
    }
}

std::vector<Vector4f> Obj::getVertices()
{
    return vertices;
}
std::vector<Vector4f> Obj::getVertexNormals()
{
    return vertexNormals;
}
vec2di Obj::getFaceVertIds()
{
    return faceVertIds;
}
vec2di Obj::getFaceNormalIds()
{
    return faceNormalIds;
}
vec2di Obj::getFaceTextureIds()
{
    return faceTextureIds;
}
