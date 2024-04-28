module;

#include <algorithm>
#include <chrono>
#include <map>
#include <numeric>
#include <random>
#include <ranges>

#include "structs.h"
#include "Transform.h"
#include "utils.h"

export module Player;

import Attire;
import GameObject;
import Random;

export struct PlayerInfo
{
	std::string id;
	Point2f pos;
	float health;
	float maxHealth;
	std::vector<bool> attire;
};

export class Player final : public GameObject
{
public:
	float m_Health;
	float m_MaxHealth;
	std::vector<bool> m_Attire;
	std::chrono::steady_clock::time_point m_AttackTime;
	inline static constexpr float SIZE = 20;

	Player(const std::string& id, const Point2f pos, float health = 100, float maxHealth = 100) :
		GameObject(id, GetDrawable(), pos),
		m_Health{ health },
		m_MaxHealth{ maxHealth },
		m_AttackTime{ std::chrono::steady_clock::now() }
	{
		m_Attire.resize(Attire::TYPES.size());
		for (const int t : Attire::TYPES)
		{
			m_Attire[t] = false;
		}
	}

	void Update(const PlayerInfo& pInfo)
	{
		if (pInfo.id != m_Id) return;
		m_Position = pInfo.pos;
		m_Health = pInfo.health;
		m_MaxHealth = pInfo.maxHealth;
	}

	void Draw() const override
	{
		if (!m_Visible) return;
		GameObject::Draw();
		Transform t{};
		t.Position = m_Position;
		t.ApplyTransformation();
		utils::FillEllipse(Point2f{ 0, 7.f / 6.f * SIZE }, SIZE / 3.f, SIZE / 3.f);
		for (int i = 0; i < m_Attire.size(); ++i)
		{
			if (m_Attire[i]) Attire::GetDrawable(i).Draw(true);
		}
		DrawHealth();
		t.ResetTransformation();
	}

	void DrawHealth() const
	{
		constexpr float barW = 2 * SIZE;
		const float y = 2 * SIZE;
		utils::SetColor({ 0, 1, 0, 1 });
		utils::DrawLine(-barW / 2, y, barW / 2, y, 4);
		utils::SetColor({ 1, 0, 0, 1 });
		utils::DrawLine(barW / 2 - barW * (m_MaxHealth - m_Health) / m_MaxHealth, y, barW / 2, y, 4);
	}

	void Move(const Vector2f movement, float left, float right, float top, float bottom)
	{
		const float pLeft = left + m_Size.x / 2;
		const float pRight = right - m_Size.x / 2;
		const float pTop = top - m_Size.y / 2;
		const float pBottom = bottom + m_Size.y / 2;
		constexpr float epsilon = 0.001f;
		m_Position += movement;
		if (m_Position.x < pLeft) m_Position.x = pLeft + epsilon;
		if (m_Position.x > pRight) m_Position.x = pRight - epsilon;
		if (m_Position.y < pBottom) m_Position.y = pBottom + epsilon;
		if (m_Position.y > pTop) m_Position.y = pTop + epsilon;
	}

	bool PickUp(const Attire& a)
	{
		if (!HasAttire(a.GetType()) && a.IsVisible() && IsOverlapping(a))
		{
			m_Attire[a.GetType()] = true;
			return true;
		}
		return false;
	}

	bool Drop(std::map<std::string, Attire>& pool)
	{
		for (int type = static_cast<int>(m_Attire.size()) - 1; type >= 0; --type)
		{
			if (m_Attire[type] == true)
			{
				m_Attire[type] = false;
				const auto a = Attire(Generate_UUID(), m_Position, type);
				pool.insert(std::pair<std::string, Attire>(a.GetId(), a));
				return true;
			}
		}
		return false;
	}

	void Attack(std::map<std::string, Player>& playerPool)
	{
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (std::chrono::duration<float>(now - m_AttackTime).count() <= 1.f)
		{
			return;
		}

		std::vector<int> players(playerPool.size());
		std::iota(players.begin(), players.end(), 0);
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::ranges::shuffle(players, rng);

		for (auto& p : playerPool | std::views::values)
		{
			if (p.GetId() != m_Id && !IsInSameCult(p) && IsOverlapping(p))
			{
				m_AttackTime = now;
				p.LoseHealth(20);
				return;
			}
		}
	}

	bool IsInSameCult(const Player& p) const
	{
		for (int type = 0; type < m_Attire.size(); ++type)
		{
			if (HasAttire(type) != p.HasAttire(type)) return false;
		}
		return true;
	}

	bool HasAttire(int type) const
	{
		if (type < 0 || type >= m_Attire.size()) return false;
		return m_Attire[type];
	}

	void LoseHealth(const float h)
	{
		m_Health -= h;
		if (m_Health <= 0.01)
		{
			m_Visible = false;
		}
	}

	static Drawable GetDrawable()
	{
		Point2f leftFoot = { -SIZE, -1.5 * SIZE };
		Point2f rightFoot = { SIZE, -1.5 * SIZE };
		Point2f crotch = { 0, -0.5 * SIZE };
		Point2f shoulder = { 0, 0.5 * SIZE };
		Point2f leftHand = { -SIZE, 0.5 * SIZE };
		Point2f rightHand = { SIZE, 0.5 * SIZE };
		Point2f head = { 0, 1.5 * SIZE };
		return Drawable({
			{leftFoot, crotch},
			{rightFoot, crotch},
			{crotch, shoulder},
			{leftHand, shoulder},
			{rightHand, shoulder},
			{shoulder, head}
			});
	}
};
