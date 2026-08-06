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

    inline constexpr float CARD_WIDTH = 80.f;
    inline constexpr float CARD_HEIGHT = 100.f;
    inline constexpr float CARD_SPACING = 110.f;

    inline constexpr float MONSTER_BOARD_SIZE = 44.f;
}