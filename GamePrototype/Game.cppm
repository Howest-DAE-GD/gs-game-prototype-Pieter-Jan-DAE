module;

#include <iostream>
#include <SDL_opengl.h>
#include <vector>
#include "structs.h"
#include "BaseGame.h"

export module Game;

import Attire;
import GameObject;
import Player;
import Random;
import Spawner;

export class Game : public BaseGame
{
	std::vector<GameObject> m_PlayerPool{};
	std::vector<Attire> m_AttirePool{};
	Player m_Player{ { 100.f, 100.f } };
	Spawner m_Spawner;

public:
	explicit Game(const Window& window) : BaseGame{ window }
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
		// Check keyboard state
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);
		if (pStates[SDL_SCANCODE_RIGHT] || pStates[SDL_SCANCODE_D])
		{
			//std::cout << "Right arrow key is down\n";
			m_Player.m_Position.x += 100.f * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_LEFT] || pStates[SDL_SCANCODE_A])
		{
			//std::cout << "Left and up arrow keys are down\n";
			m_Player.m_Position.x -= 100.f * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_UP] || pStates[SDL_SCANCODE_W])
		{
			//std::cout << "Right arrow key is down\n";
			m_Player.m_Position.y += 100.f * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_DOWN] || pStates[SDL_SCANCODE_S])
		{
			//std::cout << "Left and up arrow keys are down\n";
			m_Player.m_Position.y -= 100.f * elapsedSec;
		}

		for (auto& obj : m_AttirePool) m_Player.PickUp(obj);

		m_Player.Update(elapsedSec);
		m_Spawner.Spawn(m_AttirePool);
	}

	void Draw() const override
	{
		ClearBackground();
		m_Player.Draw();
		for (auto& a : m_AttirePool) {
			a.Draw();
		}
	}

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override
	{
		//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
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
		m_Spawner = {};
		//m_Player.m_Shape = std::vector<Point2f>{ {-10, -10}, {0, 10}, {10, -10} };
		//m_Player.PickUp(hat);
		//m_Player.PickUp(cane);

		//m_PlayerPool.resize(10, GameObject(Point2f(100, 100)));
		//m_AttirePool.push_back(Attire::Hat());

		//for (int i = 0; i < m_AttirePool.size(); ++i)
		//{
		//}
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
