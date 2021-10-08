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
    std::function<void(sstream& ss)> addVertices = [&](sstream& ss) {
        std::vector<float> empty;
        vertices.push_back(empty);
        for (int i = 0; !ss.eof(); i++) {
            float current;
            ss >> current;
            vertices.back().push_back(current);
            
        }
        if( vertices.back().size() == 3 )
        {
            vertices.back().push_back(1.0f);
        }
    };
    std::function<void(sstream& ss)> addFaces = [&](sstream& ss) {
        std::vector<int> empty;
        faces.push_back(empty);
        for (int i = 0; !ss.eof(); i++) {
            float current;
            ss >> current;
            faces.back().push_back(current);
        }
    };
    
    while (std::getline(file, line)) {
        
        sstream ss(line);
        std::string identifier;
        ss >> identifier;
        
        if(!identifier.compare("v")) addVertices(ss);
        if(!identifier.compare("f")) addFaces(ss);
        
    }
}

vec2df Obj::getVertices()
{
    return vertices;
}
vec2di Obj::getFaces()
{
    return faces;
}