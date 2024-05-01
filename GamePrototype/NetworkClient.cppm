module;

#include <future>

#include "httplib.h"
#include "parson.h"
#include "Vector2f.h"

export module NetworkClient;

import Attire;
import GameObject;
import GameState;
import Player;
import Random;
import Spawner;

struct StatusUpdate
{
	std::string timestamp;
	std::map<std::string, PlayerInfo> playerUpdates;
	std::map<std::string, AttireInfo> attireUpdates;
};

export class NetworkClient
{
	httplib::Client m_Client;

	std::chrono::steady_clock::time_point m_LastStatusUpdate = std::chrono::steady_clock::now();
	const float m_GetStatusInterval = 1 / 60.f;

	std::chrono::steady_clock::time_point m_LastMovementUpdate = std::chrono::steady_clock::now();
	const float m_MoveInterval = 1 / 30.f;
	Vector2f m_Movement = {};


public:
	NetworkClient(const std::string& url = "http://localhost:3000") :
		m_Client{ httplib::Client(url) }
	{
	}

	void JoinGame(GameState& gameState)
	{
		auto res = m_Client.Post("/player");
		const auto text = res->body.c_str();
		JSON_Value* root_value = json_parse_string(text);
		const JSON_Object* root_object = json_value_get_object(root_value);
		const char* id = json_object_get_string(root_object, "id");
		const auto idString = std::string(id);
		json_value_free(root_value);
		gameState.SetPlayerId(idString);
	}

	void GetState(GameState& gameState)
	{
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (std::chrono::duration<float>(now - m_LastStatusUpdate).count() > m_GetStatusInterval)
		{
			const std::chrono::system_clock::time_point sys_time = std::chrono::system_clock::now();
			const std::time_t now_t = std::chrono::system_clock::to_time_t(sys_time);
			std::tm now_tm;
			gmtime_s(&now_tm, &now_t); // NOLINT(cert-err33-c)
			std::ostringstream stream;
			stream << std::put_time(&now_tm, "%FT%TZ");
			const std::string iso8601 = stream.str();
			printf("%s GetState\n", iso8601.c_str());

			m_LastStatusUpdate = now;
			//std::packaged_task task([&]()
			//	{
			StatusUpdate update = GetStateRequest();
			if (update.timestamp > gameState.m_Timestamp)
			{
				gameState.m_Timestamp = update.timestamp;
				gameState.UpdatePlayers(update.playerUpdates);
				gameState.UpdateAttire(update.attireUpdates);
			}
			//	});
			//std::thread(std::move(task)).detach();
		}
	}

	StatusUpdate GetStateRequest()
	{
		auto res = m_Client.Get("/state");

		//const std::chrono::steady_clock::time_point before = std::chrono::steady_clock::now();
		const auto text = res->body.c_str();

		JSON_Value* root_value = json_parse_string(text);
		const JSON_Object* root_object = json_value_get_object(root_value);

		// Parse timestamp
		const auto timestamp = std::string(json_object_get_string(root_object, "timestamp"));

		// Parse players
		std::map<std::string, PlayerInfo> playerUpdates;
		const JSON_Object* players = json_object_get_object(root_object, "players");
		const size_t player_count = json_object_get_count(players);
		for (size_t i = 0; i < player_count; ++i)
		{
			const char* player_id = json_object_get_name(players, i);
			const JSON_Object* player = json_object_get_object(players, player_id);
			PlayerInfo p;
			p.id = json_object_get_string(player, "id");
			const JSON_Object* player_pos = json_object_get_object(player, "pos");
			p.pos = {
				static_cast<float>(json_object_get_number(player_pos, "x")),
				static_cast<float>(json_object_get_number(player_pos, "y"))
			};
			p.health = static_cast<float>(json_object_get_number(player, "health"));
			p.maxHealth = static_cast<float>(json_object_get_number(player, "maxHealth"));
			const JSON_Object* player_attire = json_object_get_object(player, "attire");
			for (int j = 0; j < Attire::TYPES.size(); ++j)
			{
				p.attire.push_back(json_object_get_boolean(player_attire, std::to_string(j).c_str()));
			}
			playerUpdates.emplace(p.id, p);
		}

		// Parse attire
		std::map<std::string, AttireInfo> attireUpdates;
		const JSON_Object* attires = json_object_get_object(root_object, "attire");
		const size_t attire_count = json_object_get_count(attires);
		for (size_t i = 0; i < attire_count; ++i)
		{
			const char* attire_id = json_object_get_name(attires, i);
			const JSON_Object* attire = json_object_get_object(attires, attire_id);
			AttireInfo a;
			a.id = json_object_get_string(attire, "id");
			const JSON_Object* attire_pos = json_object_get_object(attire, "pos");
			a.pos = {
				static_cast<float>(json_object_get_number(attire_pos, "x")),
				static_cast<float>(json_object_get_number(attire_pos, "y"))
			};
			a.type = static_cast<int>(json_object_get_number(attire, "type"));
			attireUpdates.emplace(a.id, a);
		}

		json_value_free(root_value);
		//std::chrono::steady_clock::time_point after = std::chrono::steady_clock::now();
		//printf("Time spent parsing JSON: %.6fs\n", std::chrono::duration<float>(after - before).count());

		StatusUpdate update{};
		update.timestamp = timestamp;
		update.playerUpdates = playerUpdates;
		update.attireUpdates = attireUpdates;
		return update;
	}

	void Move(const std::string& playerId, const Vector2f& movement)
	{
		m_Movement += movement;
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (std::chrono::duration<float>(now - m_LastMovementUpdate).count() > m_MoveInterval && m_Movement != Vector2f{})
		{
			const std::chrono::system_clock::time_point sys_time = std::chrono::system_clock::now();
			const std::time_t now_t = std::chrono::system_clock::to_time_t(sys_time);
			std::tm now_tm;
			gmtime_s(&now_tm, &now_t); // NOLINT(cert-err33-c)
			std::ostringstream stream;
			stream << std::put_time(&now_tm, "%FT%TZ");
			const std::string iso8601 = stream.str();
			printf("%s Moving x: %f, y: %f\n", iso8601.c_str(), m_Movement.x, m_Movement.y);

			m_LastMovementUpdate = now;
			const Vector2f movementUpdate = m_Movement;

			//std::packaged_task task([this, playerId, movementUpdate]()
			//	{
			JSON_Value* root_value = json_value_init_object();
			JSON_Object* root_object = json_value_get_object(root_value);
			json_object_dotset_number(root_object, "vector.x", movementUpdate.x);
			json_object_dotset_number(root_object, "vector.y", movementUpdate.y);
			char* serialized_string = json_serialize_to_string_pretty(root_value);
			//printf("%s\n", serialized_string);
			m_Client.Put("/player/" + playerId + "/move", serialized_string, "application/json");
			json_free_serialized_string(serialized_string);
			json_value_free(root_value);
			//	});
			//std::thread(std::move(task)).detach();
			m_Movement = {};
		}
	}

	void Take(const std::string& playerId, const std::string& attireId)
	{
		//std::packaged_task task([this, playerId, attireId]()
		//	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_string(root_object, "id", attireId.c_str());
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/take", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
		//	});
		//std::thread(std::move(task)).detach();
	}

	void Drop(const std::string& playerId, const int attireType)
	{
		//std::packaged_task task([this, playerId, attireType]()
		//	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_number(root_object, "type", attireType);
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/drop", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
		//	});
		//std::thread(std::move(task)).detach();
	}

	void Attack(const std::string& playerId, const std::string& victimId)
	{
		//std::packaged_task task([this, playerId, victimId]()
		//	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_string(root_object, "id", victimId.c_str());
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/attack", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
		//	});
		//std::thread(std::move(task)).detach();
	}
};
