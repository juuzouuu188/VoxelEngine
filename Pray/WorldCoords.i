#line 1 "C:\\Users\\Owner\\source\\repos\\Pray\\Pray\\include\\World\\WorldCoords.h"





struct ChunkCoords {
    int x, z;
    bool operator==(const ChunkCoords& other) const {
        return x == other.x && z == other.z;
    }
};


#line 15 "C:\\Users\\Owner\\source\\repos\\Pray\\Pray\\include\\World\\WorldCoords.h"
