#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>

namespace SpriteUtils
{
	// Scales `sprite` so its texture stretches to exactly fill a
	// `windowSize`-sized area. Used by every full-screen state background.
	inline void scaleToFill(sf::Sprite& sprite, sf::Vector2u windowSize)
	{
		auto texSize = sprite.getTexture().getSize();
		sprite.setScale({
			static_cast<float>(windowSize.x) / texSize.x,
			static_cast<float>(windowSize.y) / texSize.y });
	}

	// Scales `sprite` (aspect preserved) so its width matches `targetWidth`,
	// then centers it on a `windowSize`-sized area. Used by popups that
	// should keep their native aspect ratio instead of stretching to fill
	// the window.
	inline void scaleAndCenter(sf::Sprite& sprite, sf::Vector2u windowSize, unsigned int targetWidth)
	{
		auto texSize = sprite.getTexture().getSize();
		float scale = static_cast<float>(targetWidth) / static_cast<float>(texSize.x);
		sprite.setScale({ scale, scale });

		auto bounds = sprite.getLocalBounds();
		sprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
		sprite.setPosition({ static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 2.f });
	}

	// The scale factor that shrinks/grows `size`'s LARGER dimension to
	// exactly `desiredSize`, independent of the texture's own resolution -
	// the on-board sizing convention every monster/effect texture uses (see
	// Config::MONSTER_BOARD_SIZE). Templated on the vector's own component
	// type so it takes both a whole texture's pixel size (sf::Vector2u) and
	// an already-fractional size derived from one (e.g. one frame of a
	// sprite sheet, sf::Vector2f) without a caller-side cast. Zero-guarded:
	// an empty size (0 in both dimensions) returns 1.f (unscaled) instead of
	// dividing by zero.
	template <typename T>
	float maxDimensionScale(sf::Vector2<T> size, float desiredSize)
	{
		float maxDim = static_cast<float>(std::max(size.x, size.y));
		return (maxDim > 0.f) ? (desiredSize / maxDim) : 1.f;
	}

	// The scale factor that shrinks/grows `textureSize`'s WIDTH to exactly
	// `targetWidth`, preserving aspect ratio - the convention every menu
	// button/popup that should match a layout width (rather than an
	// on-board size) uses. Zero-guarded the same way as maxDimensionScale.
	inline float widthScale(sf::Vector2u textureSize, unsigned int targetWidth)
	{
		return (textureSize.x > 0) ? (static_cast<float>(targetWidth) / static_cast<float>(textureSize.x)) : 1.f;
	}
}
