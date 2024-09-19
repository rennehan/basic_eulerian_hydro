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
    real prev_pressure;

    std::shared_ptr<std::vector<box_int>> next_coordinates;
    std::shared_ptr<std::vector<real>> next_velocity;
    real next_density;
    real next_energy;
    real next_pressure;

public:
    Cell(const std::vector<box_int> &init_coordinates, 
         const std::vector<real> &init_velocity, 
         const real init_density,
         const real init_energy,
         const real init_pressure)
    {
        prev_coordinates = std::make_shared<std::vector<box_int>>(init_coordinates);
        prev_velocity = std::make_shared<std::vector<real>>(init_velocity);
        prev_density = init_density;
        prev_energy = init_energy;
        prev_pressure = init_pressure;

        next_coordinates = std::make_shared<std::vector<box_int>>();
        next_velocity = std::make_shared<std::vector<real>>();
        for (uint8_t d = 0; d < DIMENSION; d++) {
            next_coordinates->push_back(init_coordinates[d]);
            next_velocity->push_back(init_velocity[d]);
        }
        next_density = init_density;
        next_energy = init_energy;
        next_pressure = init_pressure;
    };

    // don't bother zero-ing the next values, as they will be reset
    void evolve() {
        *prev_coordinates = *next_coordinates;
        *prev_velocity = *next_velocity;
        prev_density = next_density;
        prev_energy = next_energy;
        prev_pressure = next_pressure;
    }

    void describe() {
        std::cout << std::scientific;
        std::cout << "\n";
        std::cout <<"(x_0, x_1, ..., x_N) = (";
        for (auto &coordinate : *prev_coordinates) {
            std::cout << (real)coordinate / (real)N_CELLS_1D << ",";
        }
        std::cout << ")\n";

        std::cout << "(v_0, v_1, ..., v_N) = (";
        for (auto velocity : *prev_velocity) {
            std::cout << (real)velocity << ",";
        }
        std::cout << ")\n";
        std::cout << "density = " << prev_density << "\n";
        std::cout << "energy = " << prev_energy << "\n";
        std::cout << "pressure = " << prev_pressure << "\n\n";
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

    std::shared_ptr<std::vector<real>> get_velocity() const {
        return prev_velocity;
    }

    void set_velocity(const std::vector<real> &new_velocity) {
        next_velocity = std::make_shared<std::vector<real>>(new_velocity);
    }

    real get_velocity(const uint8_t dimension) const {
        return (*prev_velocity)[dimension];
    }

    real get_next_velocity(const uint8_t dimension) const {
        return (*next_velocity)[dimension];
    }

    void set_velocity(const uint8_t dimension, real new_velocity) {
        (*next_velocity)[dimension] = new_velocity;
    }

    void set_density(const real new_density) {
        next_density = new_density;
    }

    real get_density() const {
        return prev_density;
    }

    real get_next_density() const {
        return next_density;
    }
    
    real get_energy() const {
        return prev_energy;
    }

    real get_next_energy() const {
        return next_energy;
    }
    
    void set_energy(const real new_energy) {
        next_energy = new_energy;
    }

    real get_pressure() const {
        return prev_pressure;
    }

    void set_pressure(const real new_pressure) {
        next_pressure = new_pressure;
    }
};

#endif /* GRID_HPP*/
