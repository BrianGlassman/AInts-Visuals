#pragma once

#include <initializer_list>
#include <vector>

// Ref for using initializer list: https://en.cppreference.com/w/cpp/utility/initializer_list
// Ref for [] overloading: https://www.geeksforgeeks.org/overloading-subscript-or-array-index-operator-in-c/

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3();
    Vector3(std::initializer_list<float> coords);
    Vector3(float coords[]);

    std::vector<float> asStdVec();

    // Operators
    float& operator[](int index);
private:
};

class Vector3Int {
public:
    int x;
    int y;
    int z;

    std::vector<int> asStdVec();
private:
};