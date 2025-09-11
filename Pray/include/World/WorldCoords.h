
#ifndef WorldCoords_H
#define WorldCoords_H

// Key for identifying chunks by grid position
struct ChunkCoords {
    int x, z;
    bool operator==(const ChunkCoords& other) const {
        return x == other.x && z == other.z;
    }
};


#endif