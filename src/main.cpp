#include <iostream>
#include <variant>
#include <memory>
#include <cstddef>
#include "main.hpp"
#include "Grid/Grid.hpp"
#include "ConservedQuantity/ConservedQuantity.hpp"

int main(int argc, char **argv) {
    auto grid = std::make_unique<Grid>(DIMENSION, N_CELLS_1D);

    using ConservationLaw = std::variant<ConservedCoordinates, ConservedDensity, ConservedEnergy, ConservedMomentum>;

    std::vector<std::unique_ptr<ConservationLaw>> conservation_laws;
    conservation_laws.emplace_back(std::make_unique<ConservationLaw>(ConservedCoordinates()));
    conservation_laws.emplace_back(std::make_unique<ConservationLaw>(ConservedDensity()));
    conservation_laws.emplace_back(std::make_unique<ConservationLaw>(ConservedEnergy()));
    conservation_laws.emplace_back(std::make_unique<ConservationLaw>(ConservedMomentum()));

    real dt = 0.5;
    real current_time = 0.;

    const auto number_of_neighbors = (uint8_t)(2 * DIMENSION);
    std::vector<std::shared_ptr<Cell>> neighbor_cells(number_of_neighbors);
    std::vector<std::vector<box_int>> neighbor_coordinates(number_of_neighbors, std::vector<box_int>(DIMENSION));

    std::cout << std::endl;
    while (true) {
        for (auto i = 0; i < grid->get_N_cells_Nd(); i++) {
            auto current_cell = grid->get_cell(i);

            for (auto n = 0; n < number_of_neighbors; n++) {
                neighbor_coordinates[n] = *(current_cell->get_coordinates());
            }

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

            for (auto &conservation_law : conservation_laws) {
                std::visit(
                    [&current_cell, &neighbor_cells, dt](auto &law) { 
                        law.set_initial_state(current_cell);
                        law.update(neighbor_cells, dt);
                        law.set_final_state(current_cell);
                    }, 
                    *conservation_law
                );
            }
        }

        // after entire initial pass, we update the previous values with the next values
        for (auto i = 0; i < grid->get_N_cells_Nd(); i++) {
            grid->get_cell(i)->evolve();
        }

        current_time += dt;
        if (current_time > MAX_TIME) {
            break;
        }
    }

    grid->print_cells();
}