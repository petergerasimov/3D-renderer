#include <iostream>
#include "linalg.hpp"

int main()
{
    std::vector<int> a;
    a = {1,0,3};
    Matrix<int> b;
    b = {
        {1,0,3},
        {5,1,1},
        {3,3,7}
    };
    b = b * a;
    b.print();
    return EXIT_SUCCESS;
}