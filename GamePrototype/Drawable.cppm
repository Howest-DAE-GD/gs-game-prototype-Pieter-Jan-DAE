module;

#include <vector>
#include "Transform.h"
#include "utils.h"

export module Drawable;

export class Drawable
{
public:
	std::vector<std::vector<Point2f>> m_Shapes;
	Color4f m_Color;
	Vector2f m_Offset;
	Vector2f m_Size;

	inline static bool DRAW_BOUNDINGBOX{ true };

	Drawable(const Drawable& d) : m_Shapes{ d.m_Shapes }, m_Color{ d.m_Color }, m_Offset{ d.m_Offset }, m_Size{ d.m_Size } {}

	Drawable(const std::vector<std::vector<Point2f>>& shapes, const Color4f color = { 1.f, 1.f, 1.f, 1.f }, const Vector2f offset = { 0.f, 0.f }) :
		m_Shapes(shapes),
		m_Color(color),
		m_Offset(offset)
	{
		float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::min();
		float min_y = std::numeric_limits<float>::max();
		float max_y = std::numeric_limits<float>::min();

		for (const auto& shape : shapes)
		{
			for (const auto& p : shape)
			{
				if (p.x < min_x) min_x = p.x;
				if (p.x > max_x) max_x = p.x;
				if (p.y < min_y) min_y = p.y;
				if (p.y > max_y) max_y = p.y;
			}
		}
		m_Size = { max_x - min_x, max_y - min_y };
	}

	void Draw(const bool useOffset = false) const
	{
		Draw({ 0.f, 0.f }, useOffset);
	}

	void Draw(const Point2f pos, const bool useOffset = false) const
	{
		utils::SetColor(m_Color);
		for (std::vector<Point2f> shape : m_Shapes)
		{
			Transform t{};
			t.Position = pos + (useOffset ? m_Offset : Vector2f{ 0.f, 0.f });
			t.ApplyTransformation();
			utils::FillPolygon(shape);
			utils::DrawPolygon(shape, false, 2);
			if (DRAW_BOUNDINGBOX) DrawBoundingBox();
			t.ResetTransformation();
		}
	}

	void DrawBoundingBox() const
	{
		utils::SetColor({ 1.f, 0.f, 0.f, 1.f });
		utils::DrawRect(BoundingBox(), 1);
		utils::SetColor(m_Color);
	}

	Rectf BoundingBox() const
	{
		return { -m_Size.x / 2, -m_Size.y / 2, m_Size.x, m_Size.y };
	}
};
