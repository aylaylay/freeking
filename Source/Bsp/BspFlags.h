#pragma once

#include <stdint.h>

namespace Freeking
{
	enum class BspSurfaceFlags : uint32_t
	{
		Light = 0x1,
		Slick = 0x2,
		Sky = 0x4,
		Warp = 0x8,
		Trans33 = 0x10,
		Trans66 = 0x20,
		Flowing = 0x40,
		NoDraw = 0x80,
		Hint = 0x100,
		Skip = 0x200,
		Specular = 0x400,
		Diffuse = 0x800,
		Masked = 0x1000,
		Mirror = 0x2000,
		Window33 = 0x4000,
		Window66 = 0x8000,
		_10000 = 0x10000,
		_20000 = 0x20000,
		_40000 = 0x40000,
		Water = 0x80000,
		Concrete = 0x100000,
		Fabric = 0x200000,
		Gravel = 0x400000,
		Metal = 0x800000,
		MetalLite = 0x1000000,
		Tin = 0x2000000,
		Wood = 0x4000000,
		ReflectFake = 0x8000000,
		ReflectLight = 0x10000000,
		_20000000 = 0x20000000,
		_40000000 = 0x40000000,
		_80000000 = 0x80000000,
	};

	enum class BspContentFlags : uint32_t
	{
		CONTENTS_SOLID = 1,
		CONTENTS_WINDOW = 2,
		CONTENTS_AUX = 4,
		CONTENTS_LAVA = 8,
		CONTENTS_SLIME = 16,
		CONTENTS_WATER = 32,
		CONTENTS_MIST = 64,
		LAST_VISIBLE_CONTENTS = 64,
		CONTENTS_AREAPORTAL = 0x8000,
		CONTENTS_PLAYERCLIP = 0x10000,
		CONTENTS_MONSTERCLIP = 0x20000,
		CONTENTS_CURRENT_0 = 0x40000,
		CONTENTS_CURRENT_90 = 0x80000,
		CONTENTS_CURRENT_180 = 0x100000,
		CONTENTS_CURRENT_270 = 0x200000,
		CONTENTS_CURRENT_UP = 0x400000,
		CONTENTS_CURRENT_DOWN = 0x800000,
		CONTENTS_ORIGIN = 0x1000000,
		CONTENTS_MONSTER = 0x2000000,
		CONTENTS_DEADMONSTER = 0x4000000,
		CONTENTS_DETAIL = 0x8000000,
		CONTENTS_TRANSLUCENT = 0x10000000,
		CONTENTS_LADDER = 0x20000000,
		SURF_LIGHT = 0x1,
		SURF_SLICK = 0x2,
		SURF_SKY = 0x4,
		SURF_WARP = 0x8,
		SURF_TRANS33 = 0x10,
		SURF_TRANS66 = 0x20,
		SURF_FLOWING = 0x40,
		SURF_NODRAW = 0x80,
		MASK_ALL = (-1),
		MASK_SOLID = (CONTENTS_SOLID | CONTENTS_WINDOW),
		MASK_PLAYERSOLID = (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER),
		MASK_DEADSOLID = (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW),
		MASK_MONSTERSOLID = (CONTENTS_SOLID | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER),
		MASK_WATER = (CONTENTS_WATER | CONTENTS_LAVA | CONTENTS_SLIME),
		MASK_OPAQUE = (CONTENTS_SOLID | CONTENTS_SLIME | CONTENTS_LAVA),
		MASK_SHO = (CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEADMONSTER),
		MASK_CURRENT = (CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN)
	};
}
