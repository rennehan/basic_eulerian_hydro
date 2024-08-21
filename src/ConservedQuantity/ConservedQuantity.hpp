#include <memory>
#include "../Cell/Cell.hpp"
#include "../Grid/Grid.hpp"

#ifndef CONSERVED_QUANTITY_HPP
#define CONSERVED_QUANTITY_HPP

class ConservedQuantity
{
public:
    ConservedQuantity()
    {

    }

    virtual void describe() const = 0;
    virtual void update(std::vector<std::shared_ptr<Cell>> *neighbor_cells, real *dt) = 0;
    virtual void set_initial_state(std::shared_ptr<Cell> cell) = 0;
};

class ConservedDensity : public ConservedQuantity
{
private:
    real density;
    std::shared_ptr<std::vector<real>> initial_state;

public:
    explicit ConservedDensity() {

    }

    void describe() const override {

    }

    void update(std::vector<std::shared_ptr<Cell>> *neighbor_cells, real *dt) override {

    }

    void set_initial_state(std::shared_ptr<Cell> cell) {
        density = cell->get_density();
    }
};

class ConservedEnergy : public ConservedQuantity
{
private:
    real energy;
public:
    explicit ConservedEnergy() {}
    
    void describe() const override {
 
    }

    void update(std::vector<std::shared_ptr<Cell>> *neighbor_cells, real *dt) override {
        
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        energy = cell->get_energy();
    }
};

class ConservedMomentum : public ConservedQuantity
{
private:
    std::vector<real> velocity;
public:
    explicit ConservedMomentum() {}

    void describe() const override {
        return;
    }

    void update(std::vector<std::shared_ptr<Cell>> *neighbor_cells, real *dt) override {
        return;
    }

    void set_initial_state(std::shared_ptr<Cell> cell) override {
        velocity = *cell->get_velocity();
    }
};
#endif
