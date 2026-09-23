#pragma once

// Generic hex-grid geometry for this project's "doubled" coordinate system:
// q advances by +-2 between same-row neighbors, or +-1 alongside a +-1 row
// change for a diagonal neighbor. Knows nothing about Board/Tile/gameplay -
// purely the six-neighbor hex grid's own geometry, shared by anything that
// needs adjacency or direction on it (reachability BFS, Blue's knockback).
namespace HexGrid
{
    // One neighbor step, as a (dq, dr) offset.
    struct Offset
    {
        int dq;
        int dr;
    };

    // The six neighbor directions - the one place this project's hex adjacency is defined.
    inline constexpr Offset kNeighborOffsets[6] = {
        {-2,  0}, {+2,  0},
        {-1, -1}, {+1, -1},
        {-1, +1}, {+1, +1}
    };

    // The neighbor step that best continues from (fromQ,fromRow) toward
    // (toQ,toRow) - exact when aligned to one of the six axes, closest match
    // otherwise. Useful for a knockback, pull, or dash toward/away from a target.
    Offset stepToward(int fromQ, int fromRow, int toQ, int toRow);

    // Hex-grid distance (steps) between two tiles.
    int distance(int fromQ, int fromRow, int toQ, int toRow);
}
