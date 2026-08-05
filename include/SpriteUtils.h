#pragma once
#include <SFML/Graphics.hpp>

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
}
