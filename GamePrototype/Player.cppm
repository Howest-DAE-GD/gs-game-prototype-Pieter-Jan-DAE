module;

#include <algorithm>
#include <chrono>
#include <random>

#include "structs.h"
#include "Transform.h"
#include "utils.h"

export module Player;

import Attire;
import GameObject;

export class Player final : public GameObject
{
public:
	int m_Id;
	float m_Health = 100;
	std::vector<Attire> m_Attire;
	std::chrono::steady_clock::time_point m_DropTime;
	std::chrono::steady_clock::time_point m_AttackTime;

	Player(const int id, const Point2f position) : GameObject(position, PlayerShape(), { 255.f, 255.f, 255.f, 255.f }),
		m_Id{ id },
		m_DropTime{ std::chrono::steady_clock::now() },
		m_AttackTime{ std::chrono::steady_clock::now() }
	{
	}

	const int& getId() const
	{
		return m_Id;
	}

	void Draw() const override
	{
		if (!m_Visible) return;
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

	void Update(const float elapsedSec) override
	{
		GameObject::Update(elapsedSec);
		for (Attire& a : m_Attire)
		{
			a.m_Position = m_Position + a.m_Offset;
		}
	}

	void PickUp(Attire& a)
	{
		if (a.m_Visible && utils::IsOverlapping(a.BoundingBox(), BoundingBox()) && !hasAttire(a))
		{
			m_Attire.push_back({ a });
			a.m_Visible = false;
		}
	}

	void Drop(std::vector<Attire>& pool)
	{
		if (m_Attire.size() > 0)
		{
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			if (std::chrono::duration<float>(now - m_DropTime).count() > 1.f)
			{
				m_DropTime = now;
				Attire a = { m_Attire.back() };
				a.m_Visible = true;
				a.m_Position = { m_Position };
				pool.push_back(a);
				m_Attire.pop_back();
			}
		}
	}

	void Attack(std::vector<Player>& playerPool)
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (std::chrono::duration<float>(now - m_AttackTime).count() <= 1.f)
		{
			return;
		}
		m_AttackTime = now;

		const int seed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
		std::vector<int> ids(playerPool.size());
		std::ranges::transform(playerPool, ids.begin(), [](Player& p) { return p.getId(); });

		std::ranges::shuffle(ids, std::default_random_engine(seed));

		for (int id : ids)
		{
			if (id != m_Id && !isInSameCult(playerPool[id]))
			{
				playerPool[id].loseHealth(20);
			}
		}
	}

	bool isInSameCult(const Player& p) const
	{
		if (p.getAttireCount() != getAttireCount()) return false;
		for (auto& playerAttire : m_Attire)
		{
			if (!hasAttire(playerAttire)) return false;
		}
		return true;
	}

	size_t getAttireCount() const
	{
		return m_Attire.size();
	}

	bool hasAttire(const Attire& a) const
	{
		for (auto& playerAttire : m_Attire)
		{
			if (a.m_Type == playerAttire.m_Type) return true;
		}
		return false;
	}

	void loseHealth(const float h)
	{
		m_Health -= h;
		if (m_Health < 0)
		{
			m_Visible = false;
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
		return {
			{leftFoot, crotch},
			{rightFoot, crotch},
			{crotch, shoulder},
			{leftHand, shoulder},
			{rightHand, shoulder},
			{shoulder, head}
		};
	}
};
