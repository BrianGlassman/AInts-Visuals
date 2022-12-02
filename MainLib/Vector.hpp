#pragma once

#include <initializer_list>
#include <vector>

// Ref for using initializer list: https://en.cppreference.com/w/cpp/utility/initializer_list
// Ref for [] overloading: https://www.geeksforgeeks.org/overloading-subscript-or-array-index-operator-in-c/

class Vector3;
class Vector3Int;

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3();
    Vector3(std::initializer_list<float> coords);
    Vector3(float coords[]);
    Vector3(const Vector3Int intVec);

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

    Vector3Int();
    Vector3Int(std::initializer_list<int> coords);
    Vector3Int(int coords[]);
    Vector3Int(const Vector3 floatVec);

    std::vector<int> asStdVec() const;
    Vector3Int Reversed() const { return {-x, -y, -z}; }
    std::size_t Hash() const
    {
        // Use sXXsYYsZZ as hash, where "s" is 2 for positive and 1 for negative or 0
        // (assumes that no coordinate > 99)
        return 100100100 +
               (x > 0)*1e8 + abs(x)*1e6 + 
               (y > 0)*1e5 + abs(y)*1e3 + 
               (z > 0)*1e2 + abs(z)*1e0;
    }

    static const Vector3Int  Forward; static const int  hashForward = 100100201;
    static const Vector3Int Backward; static const int hashBackward = 100100101;
    static const Vector3Int       Up; static const int       hashUp = 100201100;
    static const Vector3Int     Down; static const int     hashDown = 100101100;
    static const Vector3Int    Right; static const int    hashRight = 201100100;
    static const Vector3Int     Left; static const int     hashLeft = 101100100;

    // Operators
    bool operator==(const Vector3Int& vec) const;
    Vector3Int operator+(const Vector3Int& rhs) const;
    Vector3Int operator-(const Vector3Int& rhs) const;
private:
};

// Ref https://en.cppreference.com/w/cpp/utility/hash
// custom specialization of std::hash can be injected in namespace std
template<>
struct std::hash<Vector3Int>
{
    std::size_t operator()(Vector3Int const& vec) const noexcept
    {
        return vec.Hash();
    }
};