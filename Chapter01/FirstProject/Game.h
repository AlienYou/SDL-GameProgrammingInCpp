#pragma once

#include "SDL/SDL.h"
#include <vector>

struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 pos;
	Vector2 vel;
};

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;

	int mPaddleMyDir;
	int mPaddleOpponentDir;
	Vector2 mPaddleMyPos;
	Vector2 mPaddleOpponentPos;
	std::vector<Ball> balls;
};
