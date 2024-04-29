module;

#include <map>
#include <ranges>
#include <string>
#include <vector>

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

	void UpdatePlayers(std::map<std::string, PlayerInfo>& playerUpdates)
	{
		for (const auto& pInfo : playerUpdates | std::views::values) {
			UpdatePlayer(pInfo);
		}
		std::vector<std::string> idsToRemove;
		for (const auto& id : m_PlayerPool | std::views::keys) {
			if (!playerUpdates.contains(id)) idsToRemove.push_back(id);
		}
		for (const auto& id : idsToRemove) {
			m_PlayerPool.erase(id);
		}
	}

	void AddAttire(const AttireInfo& aInfo)
	{
		if (!m_AttirePool.contains(aInfo.id)) m_AttirePool.insert(std::pair(aInfo.id, Attire(aInfo.id, aInfo.pos, aInfo.type)));
	}

	void UpdateAttire(std::map<std::string, AttireInfo>& attireUpdates)
	{
		for (const auto& aInfo : attireUpdates | std::views::values) {
			AddAttire(aInfo);
		}
		std::vector<std::string> idsToRemove;
		for (const auto& id : m_AttirePool | std::views::keys) {
			if (!attireUpdates.contains(id)) idsToRemove.push_back(id);
		}
		for (const auto& id : idsToRemove) {
			m_AttirePool.erase(id);
		}
	}
};
