#include <vector>
#include <string>
#include <memory>
#include "../main.hpp"

#ifndef CELL_HPP
#define CELL_HPP

class Cell
{
private:
    std::shared_ptr<std::vector<box_int>> prev_coordinates;
    std::shared_ptr<std::vector<real>> prev_velocity;
    real prev_density;
    real prev_energy;

    std::shared_ptr<std::vector<box_int>> next_coordinates;
    std::shared_ptr<std::vector<real>> next_velocity;
    real next_density;
    real next_energy;

public:
    Cell(const std::vector<box_int> &init_coordinates, 
         const std::vector<real> &init_velocity, 
         const real init_density,
         const real init_energy)
    {
        prev_coordinates = std::make_shared<std::vector<box_int>>(init_coordinates);
        prev_velocity = std::make_shared<std::vector<real>>(init_velocity);
        prev_density = init_density;
        prev_energy = init_energy;

        next_coordinates = std::make_shared<std::vector<box_int>>();
        next_velocity = std::make_shared<std::vector<real>>();
        for (uint8_t d = 0; d < DIMENSION; d++) {
            next_coordinates->push_back(0);
            next_velocity->push_back(0.);
        }
        next_density = 0.;
        next_energy = 0.;
    };

    // don't bother zero-ing the next values, as they will be reset
    void evolve() {
        *prev_coordinates = *next_coordinates;
        *prev_velocity = *next_velocity;
        prev_density = next_density;
        prev_energy = next_energy;
    }

    std::shared_ptr<std::vector<box_int>> get_coordinates() const {
        return prev_coordinates;
    }

    void set_coordinates(const std::vector<box_int> &new_coordinates) {
        next_coordinates = std::make_shared<std::vector<box_int>>(new_coordinates);
    }

    box_int get_coordinates(const uint8_t dimension) const {
        return (*prev_coordinates)[dimension];
    }

    void set_coordinates(const uint8_t dimension, box_int new_coordinate) {
        (*next_coordinates)[dimension] = new_coordinate;
    }

    void set_density(const real new_density) {
        next_density = new_density;
    }

    real get_density() const {
        return prev_density;
    }

    real get_energy() const {
        return prev_energy;
    }

    void set_energy(const real new_energy) {
        next_energy = new_energy;
    }

    std::shared_ptr<std::vector<real>> get_velocity() const {
        return prev_velocity;
    }

    void set_velocity(const std::vector<real> &new_velocity) {
        next_velocity = std::make_shared<std::vector<real>>(new_velocity);
    }

    real get_velocity(const uint8_t dimension) const {
        return (*prev_velocity)[dimension];
    }

    void set_velocity(const uint8_t dimension, real new_velocity) {
        (*next_velocity)[dimension] = new_velocity;
    }
};

#endif /* GRID_HPP*/
