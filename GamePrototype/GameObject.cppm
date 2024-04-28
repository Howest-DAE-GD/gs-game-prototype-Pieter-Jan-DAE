module;

#include "utils.h"

export module GameObject;

import Drawable;

export class GameObject : public Drawable
{
public:
	std::string m_Id;
	Point2f m_Position;
	Vector2f m_Velocity;
	bool m_Visible = true;

	GameObject(std::string id, const Drawable& drawable, const Point2f position,
		const Vector2f velocity = { 0.f, 0.f }, const bool visible = true) :
		Drawable(drawable),
		m_Id{ std::move(id) },
		m_Position{ position },
		m_Velocity{ velocity },
		m_Visible{ visible }
	{}

	bool IsVisible() const
	{
		return m_Visible;
	}

	virtual void Draw() const
	{
		if (!m_Visible) return;
		Drawable::Draw(m_Position);
	}

	virtual void Update(const float elapsedSeconds)
	{
		m_Position += m_Velocity * elapsedSeconds;
	}

	bool IsOverlapping(const GameObject& o) const
	{
		Rectf a = o.BoundingBox();
		a.left += o.m_Position.x;
		a.bottom += o.m_Position.y;
		Rectf b = BoundingBox();
		b.left += m_Position.x;
		b.bottom += m_Position.y;
		return utils::IsOverlapping(a, b);
	}

	const std::string& GetId() const
	{
		return m_Id;
	}

};
