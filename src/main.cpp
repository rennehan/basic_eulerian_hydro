#include <iostream>
#include <memory>
#include <cstddef>
#include "main.hpp"
#include "Grid/Grid.hpp"
#include "ConservedQuantity/ConservedQuantity.hpp"
int main(int argc, char **argv) {
    std::unique_ptr<Grid> grid = std::make_unique<Grid>(DIMENSION, N_CELLS_1D);

    grid->print_cells();

    auto conserved_density = std::make_unique<ConservedDensity>();
    auto conserved_energy = std::make_unique<ConservedEnergy>();
    std::vector<std::unique_ptr<ConservedMomentum>> conserved_momenta;
    for (uint8_t d = 0; d < DIMENSION; d++) {
        conserved_momenta.push_back(std::make_unique<ConservedMomentum>());
    }

    real dt = 0.5;
    real current_time = 0.;

    const uint8_t number_of_neighbors = (uint8_t)(2 * DIMENSION);
    std::vector<std::shared_ptr<Cell>> neighbor_cells(number_of_neighbors);
    std::vector<std::vector<box_int>> neighbor_coordinates(number_of_neighbors, std::vector<box_int>(DIMENSION));

    std::cout << std::endl;
    while (true) {
        for (uint64_t i = 0; i < grid->get_N_cells_Nd(); i++) {
            auto current_cell = grid->get_cell(i);

            std::cout << "Cell #" << i << "\n";
            std::cout << "(x,y) = (" << current_cell->get_coordinates(0) << ", " << current_cell->get_coordinates(1) << ")\n";

            for (uint8_t n = 0; n < number_of_neighbors; n++) {
                neighbor_coordinates[n] = *(current_cell->get_coordinates());
            }

            // d = 0 refers to the nearest cells in index space, d = 1 is next closest, etc.
            for (uint8_t d = 0; d < DIMENSION; d++) {
                neighbor_coordinates[2 * d][d] = (current_cell->get_coordinates(d) + 1) % N_CELLS_1D;
                neighbor_coordinates[2 * d + 1][d] = (current_cell->get_coordinates(d) - 1 + N_CELLS_1D) % N_CELLS_1D;
            }

            for (uint8_t d = 0; d < DIMENSION; d++) {
                neighbor_cells[2 * d] = grid->get_cell(
                    grid->coordinates_to_index(neighbor_coordinates[2 * d])
                );
                neighbor_cells[2 * d + 1] = grid->get_cell(
                    grid->coordinates_to_index(neighbor_coordinates[2 * d + 1])
                );
            }

            
            // Check to make sure the neighbors make sense
            std::cout << "\nCell #" << i << " has " << (int)number_of_neighbors << " neighbors: \n";
            for (uint8_t n = 0; n < number_of_neighbors; n++) {
                std::cout << "neighbor n = " << (int)n << ", i = " << (int)grid->coordinates_to_index(neighbor_coordinates[n]) << "\n";
                std::cout << "(x, y) = (" << neighbor_coordinates[n][0] << ", " << neighbor_coordinates[n][1] << ")\n";
            }
            std::cout << "\n";

            conserved_density->set_initial_state(current_cell);
            conserved_energy->update(&neighbor_cells, &dt);

            for (uint8_t d = 0; d < DIMENSION; d++) {
                conserved_momenta[d]->set_initial_state(current_cell);
                conserved_momenta[d]->update(&neighbor_cells, &dt);
            }
        }

        current_time += dt;
        if (current_time > MAX_TIME) {
            break;
        }
    }
}