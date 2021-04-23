#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include <SFML/System/Vector2.hpp>

struct Bounds2D
{
    Bounds2D(float x_min = 0.f, float x_max = 0.f, float y_min = 0.f, float y_max = 0.f)
            : min_x(x_min), max_x(x_max), min_y(y_min), max_y(y_max) {}
    float min_x, max_x;
    float min_y, max_y;
    
    inline sf::Vector2f center() const
    {
        return sf::Vector2f((min_x+max_x)/2.f,(min_y+max_y)/2.f);
    }
    inline sf::Vector2f size() const
    {
        return sf::Vector2f(max_x-min_x,max_y-min_y);
    }
};

#endif //BOUNDS_HPP