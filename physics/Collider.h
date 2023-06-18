#pragma once


class Collider {
    bool is_trigger = false;
public:
    void set_trigger(bool trigger) { is_trigger = trigger; }
    bool get_trigger() const { return is_trigger; }

    virtual void compute_area_and_mass(){}
};