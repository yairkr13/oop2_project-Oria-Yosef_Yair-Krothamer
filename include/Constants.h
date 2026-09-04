#pragma once

enum class PlayerSide { Left, Right };

// Represents the selected play configuration.
enum class GameMode { None, PlayerVsAI, PlayerVsPlayer };

namespace Config
{
    // ������ �����
    inline constexpr unsigned int WINDOW_WIDTH = 1280;
    inline constexpr unsigned int WINDOW_HEIGHT = 720;

    // ������ ����� ������ (��� �� �����)
    inline constexpr float BOTTOM_PANEL_Y = 600.f;
    inline constexpr float BOTTOM_PANEL_HEIGHT = 120.f;
    inline constexpr float CARD_START_Y = 610.f;

    inline constexpr float TILE_RADIUS = 48.f;

    //למה הם פה? לא עדיף שהם יהיו בקלף?
    inline constexpr float CARD_WIDTH = 80.f;
    inline constexpr float CARD_HEIGHT = 100.f;
    inline constexpr float CARD_SPACING = 110.f;

    // Shared on-board visual size every monster's static sprite (and
    // Muffintop's walking sprite-sheet frames - see Monster::setWalkAnimation)
    // is scaled against - not tile size, range, or any gameplay/collision
    // value. Raised from the original 44.f for a moderately more prominent
    // monster silhouette while still fitting inside a tile (TILE_RADIUS
    // above is unchanged).
    inline constexpr float MONSTER_BOARD_SIZE = 52.f;
}