#pragma once

enum class PlayerSide { Left, Right };

// Represents the selected play configuration.
enum class GameMode { None, PlayerVsAI, PlayerVsPlayer, PlayerVsRemote };

namespace Config
{
    inline constexpr unsigned int WINDOW_WIDTH = 1280;
    inline constexpr unsigned int WINDOW_HEIGHT = 720;

    // Valid range for WINDOW_WIDTH/HEIGHT, checked at runtime in
    // Controller::Controller (not static_assert) for a clean failure message.
    inline constexpr unsigned int MIN_WINDOW_WIDTH = 1200;
    inline constexpr unsigned int MAX_WINDOW_WIDTH = 2560;
    inline constexpr unsigned int MIN_WINDOW_HEIGHT = 700;
    inline constexpr unsigned int MAX_WINDOW_HEIGHT = 1440;

    // Shared bottom-panel height; GameplayState and Player both derive its
    // Y from the live window height instead of a fixed constant.
    inline constexpr float BOTTOM_PANEL_HEIGHT = 120.f;

    // Scales the board (tile radius, monster sprite size) relative to
    // WINDOW_WIDTH so it grows/shrinks as one piece with the window.
    inline constexpr float WINDOW_WIDTH_SCALE = static_cast<float>(WINDOW_WIDTH) / 1280.f;

    inline constexpr float TILE_RADIUS = 48.f * WINDOW_WIDTH_SCALE;

    // On-board sprite size every monster (and Muffintop's walk frames) is
    // scaled against - not tile size or any gameplay value.
    inline constexpr float MONSTER_BOARD_SIZE = 52.f * WINDOW_WIDTH_SCALE;
}