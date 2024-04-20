module;

#include "structs.h"
#include "Transform.h"
#include "utils.h"

export module Player;

import Attire;
import GameObject;

export class Player : public GameObject
{
public:
	std::vector<Attire> m_Attire;

	Player(Point2f position) : GameObject(position, PlayerShape(), { 255.f, 255.f, 255.f, 255.f }) {}

	void Draw() const
	{
		GameObject::Draw();
		Transform t{};
		t.Position = m_Position;
		t.ApplyTransformation();
		utils::FillEllipse(Point2f{ 0, 7.f / 6.f * S }, S / 3.f, S / 3.f);
		t.ResetTransformation();
		for (Attire a : m_Attire)
		{
			a.Draw();
		}
	}

	void Update(float elapsedSec)
	{
		GameObject::Update(elapsedSec);
		for (Attire& a : m_Attire)
		{
			a.m_Position = m_Position + a.m_Offset;
		}
	}

	void PickUp(Attire& a)
	{
		if (a.m_Visible && utils::IsOverlapping(a.BoundingBox(), BoundingBox()))
		{
			m_Attire.push_back({ a });
			a.m_Visible = false;
		}
	}

	inline static constexpr float S = 20;

	static std::vector<std::vector<Point2f>> PlayerShape()
	{
		Point2f leftFoot = { -S, -1.5 * S };
		Point2f rightFoot = { S, -1.5 * S };
		Point2f crotch = { 0, -0.5 * S };
		Point2f shoulder = { 0, 0.5 * S };
		Point2f leftHand = { -S, 0.5 * S };
		Point2f rightHand = { S, 0.5 * S };
		Point2f head = { 0, 1.5 * S };
		return { { leftFoot, crotch}, {rightFoot, crotch}, {crotch, shoulder}, {leftHand, shoulder}, {rightHand, shoulder}, {shoulder, head} };
	}
};
