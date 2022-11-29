#include "CSCIx229.h"

#include "Vector.hpp"

Vector3::Vector3()
{
    x = 0; y = 0; z = 0;
}

Vector3::Vector3(std::initializer_list<float> coords)
{
    auto it = coords.begin();
    x = *it;
    it++;
    y = *it;
    it++;
    z = *it;
}

Vector3::Vector3(float coords[])
{
    x = coords[0];
    y = coords[1];
    z = coords[2];
}

float& Vector3::operator[](int index)
{
    switch(index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default:
            Fatal(999, "Invalid index %d\n", index);
            return x; // Have to return something or compiler grumbles
    }
}

std::vector<float> Vector3::asStdVec()
{
    return std::vector<float> ({x, y, z});
}

void Vector3::RotateX(float theta)
{
    float oldX = x, oldY = y, oldZ = z;
    x =  oldX;
    y =  Cos(theta)*oldY - Sin(theta)*oldZ;
    z =  Sin(theta)*oldY + Cos(theta)*oldZ;
}
void Vector3::RotateY(float theta)
{
    float oldX = x, oldY = y, oldZ = z;
    x =  Cos(theta)*oldX + Sin(theta)*oldZ;
    y =  oldY;
    z = -Sin(theta)*oldX + Cos(theta)*oldZ;
}
void Vector3::RotateZ(float theta)
{
    float oldX = x, oldY = y, oldZ = z;
    x =  Cos(theta)*oldX - Sin(theta)*oldY;
    y =  Sin(theta)*oldX + Cos(theta)*oldY;
    z =  oldZ;
}

float Vector3::Magnitude()
{
    return sqrt(x*x + y*y + z*z);
}

void Vector3::Normalize()
{
    float mag = Magnitude();
    x /= mag;
    y /= mag;
    z /= mag;
}

Vector3 Vector3::Normalized()
{
    Vector3 ans({x, y, z});
    ans.Normalize();
    return ans;
}

Vector3 Vector3::Hadamard(Vector3 right)
{
    Vector3 ans;
    ans.x = x * right.x;
    ans.y = y * right.y;
    ans.z = z * right.z;
    return ans;
}
float Vector3::Dot(Vector3 right)
{
    auto hadamard = Hadamard(right);
    return hadamard.x + hadamard.y + hadamard.z;
}

Vector3 Vector3::Cross(Vector3 right)
{
    Vector3 ans;
    ans.x = y * right.z - z * right.y;
    ans.y = -(x * right.z - z * right.x);
    ans.z = x * right.y - y * right.x;
    return ans;
}

std::vector<int> Vector3Int::asStdVec()
{
    return std::vector<int> ({x, y, z});
}