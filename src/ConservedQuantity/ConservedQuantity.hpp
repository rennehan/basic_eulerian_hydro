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

    virtual void update(const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt) = 0;
    virtual void set_initial_state(const std::shared_ptr<Cell> cell) = 0;
    virtual void set_final_state(const std::shared_ptr<Cell> cell) const = 0;
};

class ConservedCoordinates : public ConservedQuantity
{
private:
    std::vector<box_int> coordinates;

public:
    explicit ConservedCoordinates() {}

    void update(const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt) override {
        
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

    void update(const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt) override {
        for (auto neighbor_cell : neighbor_cells) {
            density += 1.;
        }
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        density = cell->get_density();
    }

    void set_final_state(const std::shared_ptr<Cell> cell) const override {
        cell->set_density(density);
    }
};

class ConservedEnergy : public ConservedQuantity
{
private:
    real energy;
public:
    explicit ConservedEnergy() {}


    void update(const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt) override {
        // energy is conserved
        return;
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        energy = cell->get_energy();
    }

    void set_final_state(const std::shared_ptr<Cell> cell) const override {
        cell->set_energy(energy);
    }
};

class ConservedMomentum : public ConservedQuantity
{
private:
    std::vector<real> velocity;
public:
    explicit ConservedMomentum() {}

    void update(const std::vector<std::shared_ptr<Cell>> &neighbor_cells, const real dt) override {
        return;
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        velocity = *cell->get_velocity();
    }

    void set_final_state(const std::shared_ptr<Cell> cell) const override {
        cell->set_velocity(velocity);
    }
};
#endif
