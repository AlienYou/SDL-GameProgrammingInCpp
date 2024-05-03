#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
	:mWindow(nullptr),
	mRenderer(nullptr),
	mTicksCount(0),
	mIsRunning(true)
{

}

bool Game::Initialize() {
	//Init
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)",
		100,
		100,
		1024,
		768,
		0
	);

	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	mRenderer = SDL_CreateRenderer(mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mPaddleMyPos.x = 10.0f;
	mPaddleMyPos.y = 768.0f / 2.0f;
	
	mPaddleOpponentPos.x = 1024.0f - thickness - 10;
	mPaddleOpponentPos.y = 768.0f / 2.0f;

	Ball ball1;
	ball1.pos.x = 1024.0f / 2.0f;
	ball1.pos.y = 768.0f / 2.0f;
	ball1.vel.x = -200.0f;
	ball1.vel.y = 235.0f;

	Ball ball2;
	ball2.pos.x = 1024.0F / 2.0F + 10.0f;
	ball2.pos.y = 768.0f / 2.0f;
	ball2.vel.x = -300.0f;
	ball2.vel.y = 300.0f;

	balls.push_back(ball1);
	balls.push_back(ball2);
	return true;
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	mPaddleMyDir = 0;
	if (state[SDL_SCANCODE_W]) {
		mPaddleMyDir -= 1;
	}
	else if (state[SDL_SCANCODE_S]) {
		mPaddleMyDir += 1;
	}

	mPaddleOpponentDir = 0;
	if (state[SDL_SCANCODE_I]) {
		mPaddleOpponentDir -= 1;
	}
	else if (state[SDL_SCANCODE_K]) {
		mPaddleOpponentDir += 1;
	}
}

void Game::Shutdown() {
	//First renderer
	SDL_DestroyRenderer(mRenderer);
	//Second window
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16)) {
		;
	}

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}

	mTicksCount = SDL_GetTicks();

	if (mPaddleMyDir != 0) {
		mPaddleMyPos.y += mPaddleMyDir * 300.0f * deltaTime;
		if (mPaddleMyPos.y < (paddleH / 2.0f + thickness)) {
			mPaddleMyPos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddleMyPos.y > 768.0f - paddleH / 2.0f - thickness) {
			mPaddleMyPos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	if (mPaddleOpponentDir != 0)
	{
		mPaddleOpponentPos.y += mPaddleOpponentDir * 300.0f * deltaTime;
		if (mPaddleOpponentPos.y < (paddleH / 2.0f + thickness)) {
			mPaddleOpponentPos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddleOpponentPos.y > 768.0f - paddleH / 2.0f - thickness) {
			mPaddleOpponentPos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	for (int i = 0; i < balls.size(); ++i) {
		auto& ball = balls[i];
		ball.pos.x += ball.vel.x * deltaTime;
		ball.pos.y += ball.vel.y * deltaTime;

		float diff = mPaddleMyPos.y - ball.pos.y;
		diff = diff > 0.0f ? diff : -diff;
		if (diff <= paddleH / 2.0f &&
			ball.pos.x <= 25.0f &&
			ball.pos.x >= 20.0f && //起始位置是10，表示处于方块的宽度上，但是如果仅考虑完整宽度实际上不太合理，所以应该在宽度的前一部分上才算反弹
			ball.vel.x < 0.0f) {
			ball.vel.x *= -1.0f;
		}
		else if (ball.pos.x <= 0.0f) {
			mIsRunning = false;
			SDL_Log("You lose!");
		}

		diff = mPaddleOpponentPos.y - ball.pos.y;
		diff = diff > 0.0f ? diff : -diff;
		if (diff <= paddleH / 2.0f &&
			ball.pos.x >= 1024 - thickness - 10 &&
			ball.pos.x <= 1024 - 20 &&
			ball.vel.x > 0.0f) {
			ball.vel.x *= -1.0f;
		}
		else if (ball.pos.x >= 1024.0f) {
			mIsRunning = false;
			SDL_Log("You win!");
		}
		//top
		if (ball.pos.y <= thickness && ball.vel.y < 0.0f) {
			ball.vel.y *= -1.0f;
		}//bottom
		else if (ball.pos.y >= 768 - thickness && ball.vel.y > 0.0f) {
			ball.vel.y *= -1.0f;
		}
	}
}

void Game::GenerateOutput() {
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//top
	SDL_Rect wall{ 0, 0, 1024, thickness };
	SDL_RenderFillRect(mRenderer, &wall);

	//bottom
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	//right
	//wall.x = 1024 - thickness;
	//wall.y = 0;
	//wall.w = thickness;
	//wall.h = 1024;
	//SDL_RenderFillRect(mRenderer, &wall);

	// my paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddleMyPos.x),
		static_cast<int>(mPaddleMyPos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	paddle.x = static_cast<int>(mPaddleOpponentPos.x);
	paddle.y = static_cast<int>(mPaddleOpponentPos.y - paddleH / 2);
	paddle.w = thickness; 
	paddle.h = static_cast<int>(paddleH);
	SDL_RenderFillRect(mRenderer, &paddle);

	// balls
	SDL_Rect ballRect;
	for (int i = 0; i < balls.size(); ++i) {
		auto& ball = balls[i];
		ballRect.x = static_cast<int>(ball.pos.x - thickness / 2);
		ballRect.y = static_cast<int>(ball.pos.y - thickness / 2);
		ballRect.w = thickness;
		ballRect.h = thickness;
		SDL_RenderFillRect(mRenderer, &ballRect);
	}

	//swap
	SDL_RenderPresent(mRenderer);
}
