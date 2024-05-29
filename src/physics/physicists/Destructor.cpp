//
// Created by felix-roehr on 5/27/24.
//

#include "Destructor.hpp"

Destructor::Destructor(World &world) : world(world) {}

void Destructor::destruct() const {
    destructRocks();
    destructMountain();
}

void Destructor::destructRocks() const {
    for (auto const rock : this->world.getRocks()) {
        if (this->world.isOutOfScope(*rock)) {
            delete rock;
        }
    }
}

void Destructor::destructMountain() const {
    // TODO: Implement when mountain implementation is reworked
}