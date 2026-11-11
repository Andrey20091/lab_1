#pragma once
#include <stdexcept>

class Vector2D {
private:
    int x_;
    int y_;
public:
    Vector2D(int x, int y) : x_(x), y_(y) {}

    int get_x() const { return x_; }
    int get_y() const { return y_; }

    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x_ + other.x_, y_ + other.y_);
    }
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x_ - other.x_, y_ - other.y_);
    }
    bool operator==(const Vector2D& other) const {
        return x_ == other.x_ && y_ == other.y_;
    }
    bool operator!=(const Vector2D& other) const {
        return !(*this == other);  
    }
};

class Point2D {
private:
    int x_;
    int y_;
public:
    Point2D(int x, int y) : x_(x), y_(y) {}

    int get_x() const { return x_; }
    int get_y() const { return y_; }

    Point2D operator+(const Vector2D& v) const {
        return Point2D(x_ + v.get_x(), y_ + v.get_y());
    }
    Vector2D operator-(const Point2D& other) const {
        return Vector2D(x_ - other.x_, y_ - other.y_);
    }
    bool operator==(const Point2D& other) const {
        return x_ == other.x_ && y_ == other.y_;
    }
    bool operator!=(const Point2D& other) const {
        return !(*this == other);
    }
};

class Rect {
private:
    Point2D origin_; 
    int width_;
    int height_;
public:
    Rect(const Point2D& origin, int width, int height)
        : origin_(origin), width_(width), height_(height)
    {
        if (width < 0 || height < 0) {
            throw std::invalid_argument("Rect: negative size");
        }
    }

    const Point2D& get_origin() const { return origin_; }
    int get_width() const { return width_; }
    int get_height() const { return height_; }

    // Границы прямоугольника. Левая и верхняя входят в прямоугольник,
    // правая и нижняя -- первые клетки ЗА его пределами (полуоткрытые границы).
    // Пользователям Rect не нужно знать, что внутри хранится угол и размеры.
    int left() const { return origin_.get_x(); }
    int top() const { return origin_.get_y(); }
    int right() const { return origin_.get_x() + width_; }
    int bottom() const { return origin_.get_y() + height_; }

    bool contains(const Point2D& p) const {
        return p.get_x() >= left() && p.get_x() < right() &&
            p.get_y() >= top() && p.get_y() < bottom();
    }

    bool operator==(const Rect& other) const {
        return origin_ == other.origin_ &&
            width_ == other.width_ &&
            height_ == other.height_;
    }
    bool operator!=(const Rect& other) const {
        return !(*this == other);
    }
};