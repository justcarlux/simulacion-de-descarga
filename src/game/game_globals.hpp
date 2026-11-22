#pragma once

#include <string>
#include <raylib.h>
#include "embedded_resources.hpp"

namespace GameFonts {
	inline const EmbeddedResource* const UBUNTU_LIGHT = FindEmbeddedResource("fonts/Ubuntu-Light.ttf");
}

namespace GameSprites {
	inline const EmbeddedResource* const DOCK = FindEmbeddedResource("sprites/dock.png");
	inline const EmbeddedResource* const BOX = FindEmbeddedResource("sprites/box.png");
	inline const EmbeddedResource* const TRUCK = FindEmbeddedResource("sprites/truck.png");
}

namespace GameGuiStyles {
	inline const EmbeddedResource* const DARK = FindEmbeddedResource("guistyles/style_dark.rgs");
}

namespace GameLayout {
	constexpr const int STATUS_BAR_HEIGHT = 32;
	constexpr const int SIMULATION_TOP = 192;
}

namespace GameColors {
	constexpr const Color BACKGROUND = { 45, 45, 45, 255 };
}