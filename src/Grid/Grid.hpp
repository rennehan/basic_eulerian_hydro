#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <memory>
#include <stdexcept>
#include "../main.hpp"
#include "../Cell/Cell.hpp"

#ifndef GRID_HPP
#define GRID_HPP

class Grid 
{
private:
    uint8_t dimension;
    uint32_t N_cells_1D;
    uint64_t N_cells_ND;

    // all of the Cell objects in the grid
    std::vector<std::shared_ptr<Cell>> cells;

public:
    Grid(const uint8_t input_dimension, const uint32_t input_N_cells_1D)
    {
        if (input_dimension <= 0) {
            throw std::invalid_argument("Negative/zero input dimension.");
        }

        if (input_N_cells_1D <= 0) {
            throw std::invalid_argument("Negative/zero number of cells in 1D.");
        }

        dimension = input_dimension;
        N_cells_1D = input_N_cells_1D;
        N_cells_ND = (uint64_t)pow((long double)N_cells_1D, input_dimension);

        const box_int quarter_box = (box_int)(0.25 * (real)N_CELLS_1D);

        std::vector<box_int> coordinates;
        for (uint64_t i = 0; i < N_cells_ND; i++) {
            std::vector<box_int> coordinate(DIMENSION);
            std::vector<real> velocity(DIMENSION);
            real density = 1.;
            real energy = 1.;
            real pressure = 1.;

            uint64_t scaled_index = i;

            for (uint8_t d = 0; d < DIMENSION; d++) {
                coordinate[d] = (box_int)(scaled_index % N_CELLS_1D);
                scaled_index /= N_CELLS_1D;
                velocity[d] = 0.;
            }

#if (ICS == 0)
            if (coordinate[1] <= 2 * quarter_box) {
                velocity[1] = 0.3;
                density = 1. + (double)coordinate[1] / (double)N_CELLS_1D;
            }

            if (coordinate[1] > 2 * quarter_box) {
                velocity[1] = 0.3;
                density = 2. - (double)coordinate[1] / (double)N_CELLS_1D;
            }
#else
            velocity[1] = 3. * sin(2. * M_PI * (double)coordinate[0] / (double)(N_CELLS_1D - 1));
            if (coordinate[1] <= 2 * quarter_box) {
                density = 1. + (double)coordinate[1] / (double)N_CELLS_1D;
            }

            if (coordinate[1] > 2 * quarter_box) {
                density = 2. - (double)coordinate[1] / (double)N_CELLS_1D;
            }
#endif

            real specific_kinetic_energy = 0.;
            for (uint8_t d = 0; d < DIMENSION; d++) {
                specific_kinetic_energy += 0.5 * velocity[d] * velocity[d];
            }

            // rho * e = P / (gamma - 1), E = rho * e + 0.5 * rho * u^2; E is TOTAL energy
            energy = pressure / (GAMMA-1.0) + density * specific_kinetic_energy;

            cells.push_back(std::make_shared<Cell>(coordinate, velocity, density, energy, pressure));
        }
    }

    real get_max_velocity() {
        real max_velocity = 0.;
        for (auto &cell : cells) {
            real velocity_norm = 0.;
            for (uint8_t d = 0; d < DIMENSION; d++) {
                velocity_norm += cell->get_velocity(d) * cell->get_velocity(d);
            }
            velocity_norm = sqrt(velocity_norm);
            if (velocity_norm > max_velocity) {
                max_velocity = velocity_norm;
            }
        }

        if (max_velocity < 0.1) {
            max_velocity = 0.1;
        }
        return max_velocity;
    }

    uint64_t get_N_cells_Nd() {
        return N_cells_ND;
    }

    std::shared_ptr<Cell> get_cell(const uint64_t cell_index) {
        return cells[cell_index];
    }

    void print_cells() {
        std::cout << std::scientific;
        for (auto &cell : cells) {
            std::cout << "\n";
            std::cout <<"(x_0, x_1, ..., x_N) = (";
            for (auto coordinate : *cell->get_coordinates()) {
                std::cout << (real)coordinate / (real)N_CELLS_1D << ",";
            }
            std::cout << ")\n";

            std::cout << "(v_0, v_1, ..., v_N) = (";
            for (auto velocity : *cell->get_velocity()) {
                std::cout << (real)velocity << ",";
            }
            std::cout << ")\n";
            std::cout << "density = " << cell->get_density() << "\n";
            std::cout << "energy = " << cell->get_energy() << "\n";
            std::cout << "pressure = " << cell->get_pressure() << "\n\n";
        }
    }

    void save_cells(uint64_t dump_counter) {
        std::ofstream density_file("density_grid_" + std::to_string(dump_counter) + ".dat");
        // save x,y,density
        for (auto &cell : cells) {
            for (auto coordinate : *cell->get_coordinates()) {
                density_file << (real)coordinate / (real)N_CELLS_1D << ",";
            }

            density_file << cell->get_density() << "\n";
        }

        density_file.close();

        std::ofstream velocity_x_file("velocity_x_grid_" + std::to_string(dump_counter) + ".dat");
        // save x,y,density
        for (auto &cell : cells) {
            for (auto coordinate : *cell->get_coordinates()) {
                velocity_x_file << (real)coordinate / (real)N_CELLS_1D << ",";
            }

            velocity_x_file << cell->get_velocity(0) << "\n";
        }

        velocity_x_file.close();
    }

    void index_to_coordinates(const uint64_t index, std::vector<box_int> &coordinates) {
        uint64_t next = index;

        for (uint64_t d = 0; d < DIMENSION; d++) {
            if (d < (DIMENSION-1)) {
                coordinates[d] = (box_int)(next % N_CELLS_1D);
                next /= N_CELLS_1D;
            } else {
                coordinates[d] = (box_int)next;
            }
        }
    }

    uint64_t coordinates_to_index(const std::vector<box_int> &coordinates) {
        uint64_t index = 0;
        uint64_t dimension_factor = 1;

        for (uint64_t d = 0; d < DIMENSION; d++) {
            index += (uint64_t)coordinates[d] * dimension_factor;
            dimension_factor *= (uint64_t)N_CELLS_1D;
        }

        return index;
    }

    void get_neighbors(const std::shared_ptr<Cell> cell, std::vector<std::vector<box_int>> &neighbor_coordinates, 
                       std::vector<std::shared_ptr<Cell>> &neighbor_cells) {
        for (uint8_t n = 0; n < (uint8_t)(2 * DIMENSION); n++) {
            neighbor_coordinates[n] = *(cell->get_coordinates());
        }
        
        for (uint8_t d = 0; d < DIMENSION; d++) {
            neighbor_coordinates[2 * d][d] = (cell->get_coordinates(d) + 1) % N_CELLS_1D;
            neighbor_coordinates[2 * d + 1][d] = (cell->get_coordinates(d) - 1 + N_CELLS_1D) % N_CELLS_1D;
        }

        for (uint8_t d = 0; d < DIMENSION; d++) {
            neighbor_cells[2 * d] = get_cell(
                coordinates_to_index(neighbor_coordinates[2 * d])
            );
            neighbor_cells[2 * d + 1] = get_cell(
                coordinates_to_index(neighbor_coordinates[2 * d + 1])
            );
        }
    }

    void print_neighbors(const std::shared_ptr<Cell> cell) {
        const auto number_of_neighbors = (uint8_t)(2 * DIMENSION);
        std::vector<std::shared_ptr<Cell>> neighbor_cells(number_of_neighbors);
        std::vector<std::vector<box_int>> neighbor_coordinates(number_of_neighbors, std::vector<box_int>(DIMENSION));
        get_neighbors(cell, neighbor_coordinates, neighbor_cells);

        std::cout << "\nCell data";
        cell->describe();

        std::cout << "\nNeighbor data";
        for (auto &neighbor_cell : neighbor_cells) {
            neighbor_cell->describe();
        }
    }
};

#endif /* GRID_HPP */