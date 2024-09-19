#include <iostream>
#include <variant>
#include <memory>
#include <cstddef>
#include <stdexcept>
#include "main.hpp"
#include "Grid/Grid.hpp"
#include "ConservedQuantity/ConservedQuantity.hpp"

int main(int argc, char **argv) {
    std::cout << std::scientific;

    auto grid = std::make_unique<Grid>(DIMENSION, N_CELLS_1D);


    auto coordinates_conservation = std::make_unique<ConservedCoordinates>();
    auto mass_conservation = std::make_unique<ConservedDensity>();
    auto momentum_conservation = std::make_unique<ConservedMomentum>();
    auto energy_conservation = std::make_unique<ConservedEnergy>();
    
    const real dt_max = (real)DUMP_INTERVAL / 2.;
    const real dx = 1. / (real)N_CELLS_1D;
    // use 10% the calculated value, just in case
    const real CFL_buffer = 0.01;
    const real CFL_prefactor = CFL_buffer * dx;

    real dt = CFL_prefactor / grid->get_max_velocity();
    if (dt > dt_max) {
        std::cout << "Resetting dt to dt_max of " << dt_max << "\n";
        dt = dt_max;
    }

    real current_time = 0.;
    const real dt_minimum = CFL_prefactor / 100.;

    real dt_dx = dt / (2.0 * dx);

    const auto number_of_neighbors = (uint8_t)(2 * DIMENSION);
    std::vector<std::shared_ptr<Cell>> neighbor_cells(number_of_neighbors);
    std::vector<std::vector<box_int>> neighbor_coordinates(number_of_neighbors, std::vector<box_int>(DIMENSION));

    std::cout << std::endl;

    uint64_t dump_counter = 0;
    real dump_timer = 0.;
    while (true) {
        std::cout << "current time: " << current_time << "\ttimestep: " << dt << "\n";
        std::cout << "\tdensity computation\n";
        for (auto i = 0; i < grid->get_N_cells_Nd(); i++) {
            auto current_cell = grid->get_cell(i);

            grid->get_neighbors(current_cell, neighbor_coordinates, neighbor_cells);

            coordinates_conservation->set_initial_state(current_cell);
            coordinates_conservation->update(current_cell, neighbor_cells, dt_dx);
            coordinates_conservation->set_final_state(current_cell);

            mass_conservation->set_initial_state(current_cell);
            mass_conservation->update(current_cell, neighbor_cells, dt_dx);
            mass_conservation->set_final_state(current_cell);
        }

        std::cout << "\tmomentum computation\n";
        for (auto i = 0; i < grid->get_N_cells_Nd(); i++) {
            auto current_cell = grid->get_cell(i);
            grid->get_neighbors(current_cell, neighbor_coordinates, neighbor_cells);

            momentum_conservation->set_initial_state(current_cell);
            momentum_conservation->update(current_cell, neighbor_cells, dt_dx);
            momentum_conservation->set_final_state(current_cell);
        }

        std::cout << "\tenergy and pressure computation\n";
        for (auto i = 0; i < grid->get_N_cells_Nd(); i++) {
            auto current_cell = grid->get_cell(i);
            grid->get_neighbors(current_cell, neighbor_coordinates, neighbor_cells);

            energy_conservation->set_initial_state(current_cell);
            energy_conservation->update(current_cell, neighbor_cells, dt_dx);
            energy_conservation->set_final_state(current_cell);
        }

        std::cout << "\tevolve values to the next step\n";
        // after entire initial pass, we update the previous values with the next values
        for (auto i = 0; i < grid->get_N_cells_Nd(); i++) {
            grid->get_cell(i)->evolve();
        }

        current_time += dt;
        if (current_time > MAX_TIME) {
            break;
        }

        dump_timer += dt;
        if (dump_timer > DUMP_INTERVAL) {
            std::cout << "DUMP" << "\n";
            std::cout << std::to_string(dump_counter) << "\n\n";
            grid->save_cells(dump_counter);
            dump_timer = 0.;
            dump_counter++;
        }

        dt = CFL_prefactor / grid->get_max_velocity();
        if (dt > dt_max) {
            dt = dt_max;
        }

        if (dt < dt_minimum) {
            dt = dt_minimum;
        }

        dt_dx = dt * (real)N_CELLS_1D / 2.0;
    }

#ifdef DEBUG
    grid->print_cells();
#endif
}