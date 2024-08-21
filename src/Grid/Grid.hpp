#include <array>
#include <iostream>
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

        std::vector<box_int> coordinates;
        for (uint64_t i = 0; i < N_cells_ND; i++) {
            std::vector<box_int> coordinate(DIMENSION);
            std::vector<real> velocity(DIMENSION);
            real density = 1.;
            real energy = 1.;

            uint64_t scaled_index = i;

            for (uint8_t d = 0; d < DIMENSION; d++) {
                coordinate[d] = (box_int)(scaled_index % N_CELLS_1D);
                scaled_index /= N_CELLS_1D;
                velocity[d] = 1.;
            }

            cells.push_back(std::make_shared<Cell>(coordinate, velocity, density, energy));
        }
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
            for (auto coordinate : *cell->get_coordinates()) {
                std::cout << (real)coordinate / (real)N_CELLS_1D << ",";
            }
        }
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
};

#endif /* GRID_HPP */