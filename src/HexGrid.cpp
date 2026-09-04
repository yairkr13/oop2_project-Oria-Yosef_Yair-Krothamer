#include "HexGrid.h"
#include <limits>

namespace
{
    struct Cube { int x, y, z; };

    // This grid's (q, row) -> cube coordinate transform - the standard way
    // to reason about hex direction/distance. A pure linear transform of
    // (q, row) (no constant term), so the cube DIRECTION of a given (dq, dr)
    // offset - toCube(dq, dr) - is the same regardless of which tile it's
    // taken from; stepToward below relies on exactly that to derive each of
    // kNeighborOffsets' own cube directions once, instead of hand-writing a
    // second, separate direction table that would have to be kept in sync
    // with kNeighborOffsets by eye.
    Cube toCube(int q, int row)
    {
        int x = (q - row) / 2;
        int z = row;
        return { x, -x - z, z };
    }
}

namespace HexGrid
{
    Offset stepToward(int fromQ, int fromRow, int toQ, int toRow)
    {
        Cube from = toCube(fromQ, fromRow);
        Cube to = toCube(toQ, toRow);
        int dx = to.x - from.x, dy = to.y - from.y, dz = to.z - from.z;

        int bestIndex = 0;
        int bestDot = std::numeric_limits<int>::min();
        for (int i = 0; i < 6; ++i)
        {
            Cube dir = toCube(kNeighborOffsets[i].dq, kNeighborOffsets[i].dr);
            int dot = dx * dir.x + dy * dir.y + dz * dir.z;
            if (dot > bestDot) { bestDot = dot; bestIndex = i; }
        }

        return kNeighborOffsets[bestIndex];
    }
}
