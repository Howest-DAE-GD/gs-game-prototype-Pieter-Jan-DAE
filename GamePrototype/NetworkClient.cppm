module;

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

export class NetworkClient
{
	httplib::Client m_Client;

public:
	NetworkClient(const std::string& url = "http://localhost:3000") :
		m_Client{ httplib::Client(url) }
	{
	}

	NetworkClient(const std::string& host = "localhost", const int port = 3000) :
		m_Client{ httplib::Client(host, port) }
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
		auto res = m_Client.Get("/state");
		//printf(res->body.c_str());

		const std::chrono::steady_clock::time_point before = std::chrono::steady_clock::now();

		//const auto text = R"({"players":{"0d1b0798-056c-4108-9b87-8e69f8b64ff8":{"id":"0d1b0798-056c-4108-9b87-8e69f8b64ff8","pos":{"x":0,"y":0},"health":100,"maxHealth":100,"attire":{"0":false,"1":false,"2":false}}},"attire":{"2771a98d-a21f-4864-b399-5820599d6ebe":{"id":"2771a98d-a21f-4864-b399-5820599d6ebe","pos":{"x":100,"y":100},"type":0}}})";
		const auto text = res->body.c_str();

		JSON_Value* root_value = json_parse_string(text);
		const JSON_Object* root_object = json_value_get_object(root_value);

		// Parse players
		std::map<std::string, PlayerInfo> playerUpdates;

		const JSON_Object* players = json_object_get_object(root_object, "players");
		const size_t player_count = json_object_get_count(players);
		for (size_t i = 0; i < player_count; ++i) {
			const char* player_id = json_object_get_name(players, i);
			const JSON_Object* player = json_object_get_object(players, player_id);
			PlayerInfo p;
			p.id = json_object_get_string(player, "id");
			const JSON_Object* player_pos = json_object_get_object(player, "pos");
			p.pos = { static_cast<float>(json_object_get_number(player_pos, "x")), static_cast<float>(json_object_get_number(player_pos, "y")) };
			p.health = json_object_get_number(player, "health");
			p.maxHealth = json_object_get_number(player, "maxHealth");
			const JSON_Object* player_attire = json_object_get_object(player, "attire");
			for (int i = 0; i < 3; ++i) {
				p.attire.push_back(json_object_get_boolean(player_attire, std::to_string(i).c_str()));
			}
			playerUpdates.emplace(p.id, p);
		}

		// Parse attire
		std::map<std::string, AttireInfo> attireUpdates;

		const JSON_Object* attires = json_object_get_object(root_object, "attire");
		const size_t attire_count = json_object_get_count(attires);
		for (size_t i = 0; i < attire_count; ++i) {
			const char* attire_id = json_object_get_name(attires, i);
			const JSON_Object* attire = json_object_get_object(attires, attire_id);
			AttireInfo a;
			a.id = json_object_get_string(attire, "id");
			const JSON_Object* attire_pos = json_object_get_object(attire, "pos");
			a.pos = { static_cast<float>(json_object_get_number(attire_pos, "x")), static_cast<float>(json_object_get_number(attire_pos, "y")) };
			a.type = static_cast<int>(json_object_get_number(attire, "type"));
			attireUpdates.emplace(a.id, a);
		}

		json_value_free(root_value);

		std::chrono::steady_clock::time_point after = std::chrono::steady_clock::now();

		printf("Time spent parsing JSON: %.6fs\n", std::chrono::duration<float>(after - before).count());

		gameState.UpdatePlayers(playerUpdates);
		gameState.UpdateAttire(attireUpdates);
	}

	void Move(const std::string& playerId, const Vector2f& movement)
	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_number(root_object, "vector.x", movement.x);
		json_object_dotset_number(root_object, "vector.y", movement.y);
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/move", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
	}

	void Take(const std::string& playerId, const std::string& attireId)
	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_string(root_object, "id", attireId.c_str());
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/take", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
	}

	void Drop(const std::string& playerId, const int attireType)
	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_number(root_object, "type", attireType);
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/drop", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
	}

	void Attack(const std::string& playerId, const std::string& victimId)
	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);
		json_object_dotset_string(root_object, "id", victimId.c_str());
		char* serialized_string = json_serialize_to_string_pretty(root_value);
		//printf("%s\n", serialized_string);
		m_Client.Put("/player/" + playerId + "/attack", serialized_string, "application/json");
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
	}
};
