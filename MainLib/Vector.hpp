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

    // Operators
    bool operator==(const Vector3Int& vec) const;
private:
};

// Ref https://en.cppreference.com/w/cpp/utility/hash
// custom specialization of std::hash can be injected in namespace std
template<>
struct std::hash<Vector3Int>
{
    std::size_t operator()(Vector3Int const& vec) const noexcept
    {
        // Use sXXsYYsZZ as hash, where "s" is 1 for positive and 0 for negative or 0
        // (assumes that no coordinate > 99)
        std::size_t hash = (vec.x > 0)*1e8 + vec.x*1e6 + 
               (vec.y > 0)*1e5 + vec.y*1e3 + 
               (vec.z > 0)*1e2 + vec.z*1e0;
        return hash;
    }
};