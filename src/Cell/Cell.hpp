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
    real energy;

public:
    Cell(const std::vector<box_int> &init_coordinates, 
         const std::vector<real> &init_velocity, 
         const real init_density,
         const real init_energy)
    {
        coordinates = std::make_shared<std::vector<box_int>>(init_coordinates);
        velocity = std::make_shared<std::vector<real>>(init_velocity);
        density = init_density;
        energy = init_energy;
    };

    std::shared_ptr<std::vector<box_int>> get_coordinates() const {
        return coordinates;
    }

    box_int get_coordinates(uint8_t dimension) const {
        return (*coordinates)[dimension];
    }

    real get_density() const {
        return density;
    }

    real get_energy() const {
        return energy;
    }

    std::shared_ptr<std::vector<real>> get_velocity() const {
        return velocity;
    }

    real get_velocity(uint8_t index) const {
        return (*velocity)[index];
    }
};

#endif /* GRID_HPP*/
