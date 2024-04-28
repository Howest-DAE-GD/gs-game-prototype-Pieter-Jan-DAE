module;

#include <array>
#include <vector>
#include "Vector2f.h"

export module Attire;

import Drawable;
import GameObject;

export struct AttireInfo
{
	std::string id;
	Point2f pos;
	int type;
};

export class Attire : public GameObject
{
	int m_Type;

public:
	Attire(const std::string id, const Point2f pos, const int type)
		: GameObject(id, GetDrawable(type), pos), m_Type{ type }
	{}

	int GetType() const
	{
		return m_Type;
	}

	enum Type
	{
		HAT_T,
		CANE_T
	};

	static inline const Drawable& HAT = Drawable({ {{-10,-5},{0,5},{10,-5}} }, { 1,0,0,1 }, { 0, 30 });
	static inline const Drawable& CANE = Drawable({ {{0, -15}, {0,15}}, {{-5, 15}, {5,5}}, {{-5, 5}, {5,15}} }, { 0,0,255,255 }, { 20, 20 });

	static inline constexpr std::array<Type, 2> TYPES = { HAT_T, CANE_T };
	static inline const std::array<Drawable, 2> DRAWABLES = { HAT, CANE };

	static Drawable GetDrawable(const int i)
	{
		return DRAWABLES[i];
	}
};
