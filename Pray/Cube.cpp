#include "Cube.h"

Cube::Cube(CubeType type, bool active)
    : m_type(type), m_active(active) {
}

Cube::~Cube() {
    // Nothing to clean up (no dynamic memory)
}