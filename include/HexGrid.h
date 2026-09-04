#pragma once

// Generic hex-grid geometry for this project's "doubled" coordinate system:
// q always advances by +-2 between two same-row neighbors, or +-1 alongside
// a +-1 row change for a diagonal neighbor (q - row is always even - see
// Board::screenToTile for the same convention). Knows nothing about Board,
// Tile, Monster, attacks, knockback, or any other gameplay concept - purely
// the geometry of this six-neighbor hex grid itself, so anything that needs
// to reason about adjacency or direction on it can share this instead of
// re-deriving its own copy (today: Board/BoardPathfinder's reachability BFS,
// and Blue's knockback).
namespace HexGrid
{
    // One neighbor step, as a (dq, dr) offset in this grid's own (q, row)
    // coordinates.
    struct Offset
    {
        int dq;
        int dr;
    };

    // The six neighbor directions - the one and only place this project's
    // hex adjacency is defined. Anything that needs to enumerate a tile's
    // neighbors, or reason about hex directions at all (see stepToward
    // below), uses this instead of its own copy.
    inline constexpr Offset kNeighborOffsets[6] = {
        {-2,  0}, {+2,  0},
        {-1, -1}, {+1, -1},
        {-1, +1}, {+1, +1}
    };

    // The single one of the six neighbor steps above that best continues
    // outward from (fromQ,fromRow) toward (toQ,toRow) - exact when the two
    // are aligned along one of the six axes, the closest matching direction
    // otherwise (picked by maximizing the dot product in cube coordinates,
    // the standard way to compare directions on a hex grid; the cube
    // conversion itself is an internal implementation detail - callers only
    // ever deal in (q, row)). Useful for anything that needs "which way is
    // roughly toward/away from that other tile" without needing the full
    // reachable-tiles/path machinery - e.g. a knockback, a pull, or a dash
    // toward/away from a target.
    Offset stepToward(int fromQ, int fromRow, int toQ, int toRow);
}
