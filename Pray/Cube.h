#pragma once
#ifndef CUBE_H
#define CUBE_H
#include <glm/glm.hpp>

//only responsible for this cube is to hold its position

//we can remove textureId as this can be inferred from the block type
//gonna remove this position


enum CubeType {
    //wont be rendered
	CubeType_Air = 0,
	CubeType_Grass,
	CubeType_Dirt,
	CubeType_Water,
	CubeType_Stone,
	CubeType_Wood,
	CubeType_Sand,
	CubeType_NumTypes,
};

class Cube {
public:
    Cube(CubeType type = CubeType::CubeType_Air, bool active = true);
    ~Cube();

    // State
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    // Type
    CubeType getType() const { return m_type; }
    void setType(CubeType type) { m_type = type; }

    // Helper to check if this cube is "empty/air"
    bool isAir() const { return m_type == CubeType_Air || !m_active; }

private:
    bool m_active;     // whether this cube should be rendered
    CubeType m_type;   // type of block (determines texture)
};

#endif