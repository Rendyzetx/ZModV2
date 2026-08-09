#pragma once

struct Vector2i {
    int x;
    int y;

    bool operator==(const Vector2i& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2i& other) const {
        return !(*this == other);
    }
};

struct Vector2 {
    float x, y;
};

struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

struct Vector4 {
    float x, y, z, w;
};

struct Matrix4x4 {
    float m[4][4];
};

struct Bounds {
    Vector3 m_Center;
    Vector3 m_Extent;
};

