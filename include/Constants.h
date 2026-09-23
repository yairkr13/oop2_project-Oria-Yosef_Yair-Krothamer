#pragma once

enum class PlayerSide { Left, Right };

// Represents the selected play configuration.
enum class GameMode { None, PlayerVsAI, PlayerVsPlayer };

namespace Config
{
    // ������ �����
    inline constexpr unsigned int WINDOW_WIDTH = 1280;
    inline constexpr unsigned int WINDOW_HEIGHT = 720;
    /*inline constexpr unsigned int WINDOW_WIDTH = 1600;
    inline constexpr unsigned int WINDOW_HEIGHT = 900;*/

    // Half/double the original 1280x720 design size - small enough to still
    // fit the bottom panel's fixed-size hand of cards (Card::WIDTH/HEIGHT
    // and Player::CARD_SPACING are NOT scaled by WINDOW_WIDTH_SCALE below,
    // so a too-narrow window can make a hand overlap itself or the other
    // player's), large enough not to be absurd. Checked at runtime, right
    // before the window is actually created - see Controller::Controller -
    // rather than static_assert here: main() already wraps Controller's
    // construction in a try/catch (see main.cpp) that prints a clean
    // message, instead of the wall of repeated compiler errors a
    // static_assert in a widely-included header like this one produces.
    inline constexpr unsigned int MIN_WINDOW_WIDTH = 1200;
    inline constexpr unsigned int MAX_WINDOW_WIDTH = 2560;
    inline constexpr unsigned int MIN_WINDOW_HEIGHT = 700;
    inline constexpr unsigned int MAX_WINDOW_HEIGHT = 1440;

    // BOTTOM_PANEL_Y removed - it was a fixed Y (600.f) that only happened
    // to match the window's original 720-tall creation size. Genuinely
    // shared between GameplayState (draws the panel) and Player (positions
    // cards on it, and needs the same boundary for click/hover checks), so
    // both now compute it themselves as windowHeight - BOTTOM_PANEL_HEIGHT
    // from the live window size, instead of a stale constant.
    inline constexpr float BOTTOM_PANEL_HEIGHT = 120.f;

    // Both tile radius and monster sprite size below are expressed relative
    // to WINDOW_WIDTH (48px/52px were tuned at the original 1280-wide
    // window) so the whole board - hexes and the monsters standing on them
    // together - scales up or down as one piece whenever WINDOW_WIDTH
    // changes, instead of a fixed-size board leaving empty margin (or
    // overflowing) around it on a differently-sized window. Still a plain
    // compile-time constant - no live resize, no runtime window-size
    // parameter anywhere.
    inline constexpr float WINDOW_WIDTH_SCALE = static_cast<float>(WINDOW_WIDTH) / 1280.f;

    inline constexpr float TILE_RADIUS = 48.f * WINDOW_WIDTH_SCALE;

    //למה הם פה? לא עדיף שהם יהיו בקלף? - כן: הועברו ל-Card.h (Card::WIDTH/HEIGHT)
    // ו-Player.h (CARD_SPACING, Player's own hand-layout choice).

    // Shared on-board visual size every monster's static sprite (and
    // Muffintop's walking sprite-sheet frames - see Monster::setWalkAnimation)
    // is scaled against - not tile size, range, or any gameplay/collision
    // value. Raised from the original 44.f for a moderately more prominent
    // monster silhouette while still fitting inside a tile.
    inline constexpr float MONSTER_BOARD_SIZE = 52.f * WINDOW_WIDTH_SCALE;
}