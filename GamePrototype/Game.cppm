module;

#include <iostream>
#include <ranges>
#include <SDL_opengl.h>
#include <vector>
#include "structs.h"
#include "BaseGame.h"
#include "Vector2f.h"

export module Game;

import Attire;
import GameObject;
import GameState;
import NetworkClient;
import Player;
import Random;
import Spawner;

export struct GameSettings
{
	std::string url = "http://localhost:3000";
};

export class Game : public BaseGame
{
	float BASE_SPEED = 200.f;
	GameState m_GameState;
	NetworkClient m_NetworkClient;

public:
	explicit Game(const Window& window, const GameSettings& settings) : BaseGame{ window },
		m_NetworkClient{ settings.url }
	{
		Initialize();
	}

	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game() override
	{
		Cleanup();
	}

	void Update(float elapsedSec) override
	{
		m_NetworkClient.GetState(m_GameState);
		for (auto& p : m_GameState.m_PlayerPool | std::views::values)
		{
			if (p.m_Id == m_GameState.m_PlayerId)
			{
				p.m_Color = { 0.f, 1.f, 0.f, 1.f };
			}
		}

		if (m_GameState.m_GameOver) return;

		if (!m_GameState.m_PlayerPool.contains(m_GameState.m_PlayerId))
		{
			printf("Trying to update own player [id: %s] but could not find in player list.", m_GameState.m_PlayerId.c_str());
			return;
		}
		// Check keyboard state
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);

		Vector2f movement{};
		if (pStates[SDL_SCANCODE_RIGHT] || pStates[SDL_SCANCODE_D])
		{
			//std::cout << "Right arrow key is down\n";
			movement.x += BASE_SPEED * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_LEFT] || pStates[SDL_SCANCODE_A])
		{
			//std::cout << "Left arrow key is down\n";
			movement.x -= BASE_SPEED * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_UP] || pStates[SDL_SCANCODE_W])
		{
			//std::cout << "Up arrow key is down\n";
			movement.y += BASE_SPEED * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_DOWN] || pStates[SDL_SCANCODE_S])
		{
			//std::cout << "Down arrow key is down\n";
			movement.y -= BASE_SPEED * elapsedSec;
		}
		m_NetworkClient.Move(m_GameState.m_PlayerId, movement);
	}

	void Draw() const override
	{
		ClearBackground();
		for (auto& player : m_GameState.m_PlayerPool | std::views::values) player.Draw();
		for (auto& a : m_GameState.m_AttirePool | std::views::values) a.Draw();
	}

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override
	{
		if (m_GameState.m_GameOver) return;

		auto& you = m_GameState.m_PlayerPool.at(m_GameState.m_PlayerId);
		if (e.keysym.scancode == SDL_SCANCODE_Q)
		{
			for (int type = static_cast<int>(you.m_Attire.size()) - 1; type >= 0; --type)
			{
				if (you.m_Attire[type] == true)
				{
					m_NetworkClient.Drop(m_GameState.m_PlayerId, type);
					return;
				}
			}
		}
		if (e.keysym.scancode == SDL_SCANCODE_E)
		{
			for (auto& [id, attire] : m_GameState.m_AttirePool)
			{
				if (you.Take(attire))
				{
					m_NetworkClient.Take(m_GameState.m_PlayerId, id);
					return;
				}
			}
		}
		if (e.keysym.scancode == SDL_SCANCODE_SPACE)
		{
			if (const std::string victimId = you.Attack(m_GameState.m_PlayerPool); !victimId.empty())
			{
				m_NetworkClient.Attack(m_GameState.m_PlayerId, victimId);
			}
		}
	}

	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override
	{
		//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
		//switch ( e.keysym.sym )
		//{
		//case SDLK_LEFT:
		//	//std::cout << "Left arrow key released\n";
		//	break;
		//case SDLK_RIGHT:
		//	//std::cout << "`Right arrow key released\n";
		//	break;
		//case SDLK_1:
		//case SDLK_KP_1:
		//	//std::cout << "Key 1 released\n";
		//	break;
		//}
	}

	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override
	{
		//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	}

	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override
	{
		//std::cout << "MOUSEBUTTONDOWN event: ";
		//switch ( e.button )
		//{
		//case SDL_BUTTON_LEFT:
		//	std::cout << " left button " << std::endl;
		//	break;
		//case SDL_BUTTON_RIGHT:
		//	std::cout << " right button " << std::endl;
		//	break;
		//case SDL_BUTTON_MIDDLE:
		//	std::cout << " middle button " << std::endl;
		//	break;
		//}
	}

	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override
	{
		//std::cout << "MOUSEBUTTONUP event: ";
		//switch ( e.button )
		//{
		//case SDL_BUTTON_LEFT:
		//	std::cout << " left button " << std::endl;
		//	break;
		//case SDL_BUTTON_RIGHT:
		//	std::cout << " right button " << std::endl;
		//	break;
		//case SDL_BUTTON_MIDDLE:
		//	std::cout << " middle button " << std::endl;
		//	break;
		//}
	}

private:
	// FUNCTIONS
	void Initialize()
	{
		m_NetworkClient.JoinGame(m_GameState);
	}

	void Cleanup()
	{
	}

	void ClearBackground() const
	{
		glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
};
