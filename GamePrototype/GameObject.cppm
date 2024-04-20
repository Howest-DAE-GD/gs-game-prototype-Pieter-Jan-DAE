module;

#include "structs.h"
#include "Transform.h"
#include "utils.h"
#include "Vector2f.h"

export module GameObject;

export class GameObject
{
public:
	Point2f m_Position;
	Vector2f m_Size;
	Color4f m_Color;
	Vector2f m_Velocity;
	std::vector<std::vector<Point2f>> m_Shapes;
	bool m_Visible = true;

	GameObject(Point2f position, std::vector<std::vector<Point2f>> shapes, Color4f color) : m_Position(position), m_Shapes(shapes), m_Color(color)
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

	GameObject(std::vector<std::vector<Point2f>> shapes, Color4f color) : GameObject({ 0,0 }, shapes, color) {}

	virtual ~GameObject() = default;

	virtual void Draw() const
	{
		if (!m_Visible) return;
		utils::SetColor(m_Color);
		for (std::vector<Point2f> shape : m_Shapes)
		{
			//std::vector<Point2f> offsetShape(shape.size());
			//std::transform(shape.begin(), shape.end(), offsetShape.begin(), [this](Point2f p) { return p + m_Position; });
			Transform t{};
			t.Position = m_Position;
			t.ApplyTransformation();
			utils::FillPolygon(shape);
			utils::DrawPolygon(shape, false, 2);
			t.ResetTransformation();
		}
		DrawBoundingBox();
	}

	void DrawBoundingBox() const
	{
		utils::SetColor({ 255.f, 0.f, 0.f, 255.f });
		utils::DrawRect(BoundingBox(), 1);
		utils::SetColor(m_Color);
	}

	virtual void Update(float elapsedSeconds)
	{
		m_Position += m_Velocity * elapsedSeconds;
	}

	Rectf BoundingBox(Point2f p) const
	{
		return { p.x - m_Size.x / 2, p.y - m_Size.y / 2, m_Size.x, m_Size.y };
	}

	virtual Rectf BoundingBox() const
	{
		return BoundingBox(m_Position);
	}

};
