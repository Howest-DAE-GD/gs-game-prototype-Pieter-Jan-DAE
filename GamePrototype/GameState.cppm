module;

#include <map>
#include <ranges>
#include <string>

export module GameState;

import Attire;
import Player;
import Random;

export class GameState
{
public:
	std::string m_PlayerId;
	std::map<std::string, Player> m_PlayerPool;
	std::map<std::string, Attire> m_AttirePool;
	bool m_GameOver = false;

	GameState()
	{
		m_PlayerId = Generate_UUID();
		m_PlayerPool.insert(std::pair<std::string, Player>(m_PlayerId, Player{ m_PlayerId, {100,100} }));
		std::string pId = Generate_UUID();
		m_PlayerPool.insert(std::pair<std::string, Player>(pId, Player{ pId, {200,200} }));
		pId = Generate_UUID();
		m_PlayerPool.insert(std::pair<std::string, Player>(pId, Player{ pId, {300,200} }));
	}

	void SetPlayerId(const std::string& id)
	{
		m_PlayerId = id;
	}

	void UpdatePlayer(const PlayerInfo& pInfo)
	{
		if (m_PlayerPool.contains(pInfo.id)) m_PlayerPool.at(pInfo.id).Update(pInfo);
		else m_PlayerPool.insert(std::pair(pInfo.id, Player(pInfo.id, pInfo.pos, pInfo.health, pInfo.maxHealth)));
	}

	void AddAttire(const AttireInfo& aInfo)
	{
		if (!m_AttirePool.contains(aInfo.id)) m_AttirePool.insert(std::pair(aInfo.id, Attire(aInfo.id, aInfo.pos, aInfo.type)));
	}

	void RemoveAttire(const std::map<std::string, AttireInfo>& aInfos)
	{
		for (const auto& [id, attire] : std::views::reverse(m_AttirePool)) {
			if (!aInfos.contains(id))
			{
				m_AttirePool.erase(id);
			}
		}
	}
};
