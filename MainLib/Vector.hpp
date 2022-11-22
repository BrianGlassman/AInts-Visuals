#pragma once

#include <vector>

// Ref for [] overloading: https://www.geeksforgeeks.org/overloading-subscript-or-array-index-operator-in-c/

class Vector3 {
public:
    float x;
    float y;
    float z;


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