#pragma once
//ML Detection Extension
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
// SDL and OpenGL Includes
#pragma warning(disable : 26812)
#pragma warning(disable : 4820)
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h> 
#include <SDL_mixer.h> 
#include <SDL_image.h>
#include <ctime>

#include "structs.h"
#include "SVGParser.h"

import Game;

GameSettings ParseCommandLineArguments(int argc, char** argv);
void StartHeapControl();
void DumpMemoryLeaks();

int SDL_main(int argc, char** argv)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	StartHeapControl();

	const GameSettings settings = ParseCommandLineArguments(argc, argv);
	Game* pGame{ new Game{ Window{ "The Cult - Vandenberghe, Pieter-Jan - DAEmployed", 1000 , 1000 }, settings } };
	pGame->Run();
	delete pGame;

	DumpMemoryLeaks();
	return 0;
}

GameSettings ParseCommandLineArguments(int argc, char** argv)
{
	GameSettings settings{};
	settings.url = "localhost:3000";
	for (int i = 1; i < argc - 1; i++) {
		if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--host") == 0)) {
			settings.url = std::string(argv[i + 1]);
		}
	}
	printf("Using host: %s\n", settings.url.c_str());
	return settings;
}

void StartHeapControl()
{
#if defined(DEBUG) | defined(_DEBUG)
	// Notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Report detected leaks when the program exits
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Set a breakpoint on the specified object allocation order number
	//_CrtSetBreakAlloc( 156 );
#endif
}

void DumpMemoryLeaks()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}
