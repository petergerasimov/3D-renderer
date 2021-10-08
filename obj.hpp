#ifndef __OBJ_HPP__
#define __OBJ_HPP__

#include <string>
#include <vector>
#include <fstream>

typedef std::vector<std::vector<float>> vec2df;
typedef std::vector<std::vector<int>> vec2di;

class Obj {
    private:
        vec2df vertices;
        vec2di faces;
        void parse(std::ifstream& file);
    public:
        Obj(std::string path);
        vec2df getVertices();
        vec2di getFaces();
        //

};

#endif //__OBJ_HPP__