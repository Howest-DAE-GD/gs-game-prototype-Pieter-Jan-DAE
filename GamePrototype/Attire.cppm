module;

#include <vector>
#include "structs.h"
#include "Vector2f.h"

export module Attire;

import GameObject;
import Random;

export class Attire final : public GameObject
{
public:

	enum AttireType
	{
		HAT,
		CANE
	};

	Vector2f m_Offset;
	AttireType m_Type;

	Attire(AttireType type, Point2f position, Vector2f offset, std::vector<std::vector<Point2f>> shapes, Color4f color) : GameObject(position, shapes, color), m_Type{ type }, m_Offset{ offset } {}
	Attire(AttireType type, Vector2f offset, std::vector<std::vector<Point2f>> shapes, Color4f color) : GameObject(shapes, color), m_Type{ type }, m_Offset{ offset } {}
	Attire(const Attire& a) : GameObject(a.m_Position, a.m_Shapes, a.m_Color), m_Type{ a.m_Type }, m_Offset{ a.m_Offset }
	{
		m_Visible = a.m_Visible;
	}
	~Attire() override = default;

	static Attire Hat()
	{
		const Point2f pos = { Generate.Float(100, 1100), Generate.Float(100, 700) };
		const Vector2f offset = { 0, 30 };
		const std::vector<std::vector<Point2f>> shapes = { {{-10,-5},{0,5},{10,-5}} };
		const Color4f color = { 1,0,0,1 };
		return Attire(HAT, pos, offset, shapes, color);
	}
	static Attire Cane()
	{
		const Point2f pos = { Generate.Float(100, 1100), Generate.Float(100, 700) };
		const Vector2f offset = { 20, 20 };
		const std::vector<std::vector<Point2f>> shapes = { {{0, -15}, {0,15}}, {{-5, 15}, {5,5}}, {{-5, 5}, {5,15}} };
		const Color4f color = { 0,0,255,255 };
		return Attire(CANE, pos, offset, shapes, color);
	}
};
