#include <vector>
#include <string>
#include <memory>
#include "../main.hpp"

#ifndef CELL_HPP
#define CELL_HPP

class Cell
{
private:
    std::shared_ptr<std::vector<box_int>> coordinates;
    std::shared_ptr<std::vector<real>> velocity;
    real density;

public:
    Cell(const std::shared_ptr<std::vector<box_int>> init_coordinates, 
         const std::shared_ptr<std::vector<real>> init_velocity, 
         const real init_density)
    {
        coordinates = std::move(init_coordinates);
        velocity = std::move(init_velocity);
        density = init_density;
    };

    std::shared_ptr<std::vector<box_int>> get_coordinates() const {
        return coordinates;
    }

    real get_density() const {
        return density;
    }

    std::shared_ptr<std::vector<real>> get_velocity() const {
        return velocity;
    }

    real get_velocity(uint8_t index) {
        return (*velocity)[index];
    }
};

#endif /* GRID_HPP*/
