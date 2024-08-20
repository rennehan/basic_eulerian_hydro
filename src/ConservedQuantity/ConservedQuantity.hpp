#include <memory>
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
    virtual void update(real *dt) = 0;
    virtual void set_initial_state(std::shared_ptr<std::vector<real>> initial_state) = 0;
};

class ConservedDensity : public ConservedQuantity
{
private:
    real density;

public:
    explicit ConservedDensity() {

    }

    void describe() const override {

    }

    void update(real *dt) const override {

    }

    void set_initial_state(std::shared_ptr<std::vector<real>> initial_state) {

    }
};

class ConservedEnergy : public ConservedQuantity
{
private:
    real energy;
public:
    explicit ConservedEnergy() {}
    
    void describe() const override {
        return;
    }

    void update(real *dt) const override {
        return;
    }

    void set_initial_state() {

    }
};

class ConservedMomentum : public ConservedQuantity
{
private:
    real velocity;
public:
    explicit ConservedMomentum() {}

    void describe() const override {
        return;
    }

    void update(real *dt) const override {
        return;
    }

    void set_initial_state() {

    }
};
#endif
