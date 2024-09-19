#include <memory>
#include <exception>
#include "../Cell/Cell.hpp"
#include "../Grid/Grid.hpp"

#ifndef CONSERVED_QUANTITY_HPP
#define CONSERVED_QUANTITY_HPP

class ConservedQuantity
{
public:
    ConservedQuantity() {}

    virtual void update(const std::shared_ptr<Cell> cell, const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt) = 0;
    virtual void set_initial_state(const std::shared_ptr<Cell> cell) = 0;
    virtual void set_final_state(const std::shared_ptr<Cell> cell) const = 0;
};

class ConservedCoordinates : public ConservedQuantity
{
private:
    std::vector<box_int> coordinates;

public:
    explicit ConservedCoordinates() {}

    void update(const std::shared_ptr<Cell> cell, const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt_dx) override {
        
    }

    void set_initial_state(std::shared_ptr<Cell> cell) {
        coordinates = *cell->get_coordinates();
    }

    // must be called after update
    void set_final_state(const std::shared_ptr<Cell> cell) const override {
        cell->set_coordinates(coordinates);
    }
};

class ConservedDensity : public ConservedQuantity
{
private:
    real density;

public:
    explicit ConservedDensity() {}

    void update(const std::shared_ptr<Cell> cell, const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt_dx) override {
        // (drho/dx_j) * u_j + rho * (du_j / dx_j)
        for (uint8_t d = 0; d < DIMENSION; d++) {
            const real next_neighbor_rho = neighbor_cells[2 * d]->get_density();
            const real prev_neighbor_rho = neighbor_cells[2 * d + 1]->get_density();
            const real drho = next_neighbor_rho - prev_neighbor_rho;

            const real next_neighbor_uj = neighbor_cells[2 * d]->get_velocity(d);
            const real prev_neighbor_uj = neighbor_cells[2 * d + 1]->get_velocity(d);
            const real du_j = next_neighbor_uj - prev_neighbor_uj;

            density -= dt_dx * (cell->get_velocity(d) * (next_neighbor_rho - prev_neighbor_rho) + cell->get_density() * (next_neighbor_uj - prev_neighbor_uj));
        }
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        density = cell->get_density();
    }

    void set_final_state(const std::shared_ptr<Cell> cell) const override {
#ifdef DEBUG
        if (isnan(density)) {
            std::cout << "Density is NaN.\n";
            cell->describe();
            exit(1);
        }
#endif
        cell->set_density(density);
    }
};

class ConservedEnergy : public ConservedQuantity
{
private:
    real energy;
public:
    explicit ConservedEnergy() {}


    void update(const std::shared_ptr<Cell> cell, const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt_dx) override {
        for (uint8_t d = 0; d < DIMENSION; d++) {
            // du_j/dx_j * (E + P); P = p / rho
            const real next_neighbor_uj = neighbor_cells[2 * d]->get_velocity(d);
            const real prev_neighbor_uj = neighbor_cells[2 * d + 1]->get_velocity(d);
            const real du_j = next_neighbor_uj - prev_neighbor_uj;
            energy -= dt_dx * du_j * (cell->get_energy() + cell->get_pressure());

            // u_j * dE/dx_j
            const real next_neighbor_E = neighbor_cells[2 * d]->get_energy();
            const real prev_neighbor_E = neighbor_cells[2 * d + 1]->get_energy();
            const real dE = next_neighbor_E - prev_neighbor_E;
            energy -= dt_dx * cell->get_velocity(d) * dE;

            // u_j * dP/dx_j
            const real next_neighbor_P = neighbor_cells[2 * d]->get_pressure();
            const real prev_neighbor_P = neighbor_cells[2 * d + 1]->get_pressure();
            const real dP = next_neighbor_P - prev_neighbor_P;
            energy -= dt_dx * cell->get_velocity(d) * dP;
        }
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        energy = cell->get_energy();
    }

    void set_final_state(const std::shared_ptr<Cell> cell) const override {
#ifdef DEBUG
        if (isnan(energy)) {
            std::cout << "Energy is NaN.\n";
            cell->describe();
            exit(1);
        }
#endif
        cell->set_energy(energy);
        real next_specific_kinetic_energy = 0.;
        for (uint8_t d = 0; d < DIMENSION; d++) {
            next_specific_kinetic_energy += 0.5 * cell->get_next_velocity(d) * cell->get_next_velocity(d);
        }

        // next_pressure is P = (gamma - 1) * rho * e = (gamma - 1) * (E - 0.5 * rho * u^2)
        const real next_pressure = (GAMMA-1.0) * (cell->get_next_energy() - cell->get_next_density() * next_specific_kinetic_energy);
#ifdef DEBUG
        if (next_pressure <= 0.) {
            std::cout << "Pressure is zero or negative.\n";
            cell->describe();
            exit(1);
        }
#endif
        cell->set_pressure(next_pressure);
    }
};

class ConservedMomentum : public ConservedQuantity
{
private:
    std::vector<real> momentum;
public:
    explicit ConservedMomentum() {}

    void update(const std::shared_ptr<Cell> cell, const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt_dx) override {
        // (dt / dx)) * ( (drho/dx_j * u_i * u_j) + (rho * u_j * du_i/dx_j) + (rho * u_i * du_j/dx_j))
        for (uint8_t component = 0; component < DIMENSION; component++) {

            // d signifies the derivative direction, here labelled j in the comments. i indicates direction of equation
            for (uint8_t d = 0; d < DIMENSION; d++) {

                // drho/dx_j
                const real next_neighbor_rho = neighbor_cells[2 * d]->get_density();
                const real prev_neighbor_rho = neighbor_cells[2 * d + 1]->get_density();
                const real drho = next_neighbor_rho - prev_neighbor_rho;

                // drho/dx_j * u_i * u_j
                momentum[component] -= dt_dx * drho * cell->get_velocity(component) * cell->get_velocity(d);

                // du_i/dx_j
                const real next_neighbor_ui = neighbor_cells[2 * d]->get_velocity(component);
                const real prev_neighbor_ui = neighbor_cells[2 * d + 1]->get_velocity(component);
                const real du_i = next_neighbor_ui - prev_neighbor_ui;

                // rho * u_j * du_i/dx_j
                momentum[component] -= dt_dx * cell->get_density() * cell->get_velocity(d) * du_i;

                // du_j / dx_j
                const real next_neighbor_uj = neighbor_cells[2 * d]->get_velocity(d);
                const real prev_neighbor_uj = neighbor_cells[2 * d + 1]->get_velocity(d);
                const real du_j = next_neighbor_uj - prev_neighbor_uj;

                // rho * u_i * du_j / dx_j
                momentum[component] -= dt_dx * cell->get_density() * cell->get_velocity(component) * du_j;

                // add on the divergence term
                if (d == component) {
                    // dP / dx_j
                    const real next_neighbor_P = neighbor_cells[2 * d]->get_pressure();
                    const real prev_neighbor_P = neighbor_cells[2 * d + 1]->get_pressure();
                    const real dP = next_neighbor_P - prev_neighbor_P;

                    // dP / dx_j delta_ij
                    momentum[component] -= dt_dx * dP;
                }
            }
        }
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        momentum = *cell->get_velocity();
        for (uint8_t d = 0; d < DIMENSION; d++) {
            momentum[d] *= cell->get_density();
        }
    }

    void set_final_state(const std::shared_ptr<Cell> cell) const override {
        for (uint8_t d = 0; d < DIMENSION; d++) {
#ifdef DEBUG
            if (isnan(momentum[d])) {
                std::cout << "Momentum in " << (int)d << " dimension is NaN.\n";
                cell->describe();
                exit(1);
            }
#endif
            cell->set_velocity(d, momentum[d] / cell->get_next_density());
        }
    }
};
#endif
