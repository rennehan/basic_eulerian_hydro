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
    auto conserved_momenta = std::make_unique<std::vector<std::unique_ptr<ConservedMomentum>>>();
    for (uint8_t d = 0; d < DIMENSION; d++) {
        conserved_momenta->push_back(std::make_unique<ConservedMomentum>());
    }

    real dt = 0.5;
    real current_time = 0.;
    while (true) {
        for (uint8_t d = 0; d < DIMENSION; d++) {
            for (uint64_t i = 0; i < grid->get_N_cells_Nd(); i++) {
                std::shared_ptr<Cell> current_cell = grid->get_cell(i);
                real initial_density = current_cell->get_density();
                real initial_pressure = current_cell->get_pressure();
                std::shared_ptr<std::vector<real>> initial_velocity = current_cell->get_velocity();

                auto initial_state_vector = std::make_shared<std::vector<real>>();
                initial_state_vector->push_back(initial_density);
                initial_state_vector->insert(
                    initial_state_vector->end(), 
                    initial_velocity->begin(), 
                    initial_velocity->end()
                );
                initial_state_vector->push_back(initial_pressure);

                conserved_density->set_initial_state(initial_state_vector);
                (*conserved_momenta)[d]->set_initial_state(initial_state_vector);

                conserved_density->update(&dt);
                (*conserved_momenta)[d]->update(&dt);
                conserved_energy->update(&dt);

            }
        }

        current_time += dt;
        if (current_time > MAX_TIME) {
            break;
        }
    }
}