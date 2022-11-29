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

    void RotateX(float theta);
    void RotateY(float theta);
    void RotateZ(float theta);

    float Magnitude();
    void Normalize(); // Normalizes in-place
    Vector3 Normalized(); // Returns the normalized version

    Vector3 Hadamard(Vector3 right); // Element-wise product (i.e. dot product without summing)
    float Dot(Vector3 right);
    Vector3 Cross(Vector3 right);

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