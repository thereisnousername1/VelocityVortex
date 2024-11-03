#pragma once
#include <iostream>
#include <string>
#include "SDL2-2.28.4\include\SDL.h"
#include "SDL2_ttf-2.22.0\include\SDL_ttf.h"
#include "SDL2_image-2.8.2\include\SDL_image.h"
#include "SDL2_mixer-2.8.0\include\SDL_mixer.h"
#include "Constants.h"

using namespace std;

// Define game states
enum GameState {
	INITIALIZE,
	PLAYING,
	GAME_OVER
};

class Game {
public:
	Game();
	void UserInterface(const string& text, int x, int y);
	~Game();
	void RenderTrack();
	void drawCar(SDL_Renderer* renderer, int x, int y);
	void Run();
	void Gameloop();
	void HandleCollisions();
	void InitializePage();
	void PlayMusic();

private:

	GameState currentState;
	float ElapsedTime = 0.0f;
	void Input();
	SDL_Window* window;
	SDL_Renderer* renderer;

	float CarPosition = 0.0f;
	SDL_Texture* player;
	SDL_Texture* straightTexture;
	SDL_Texture* leftTexture;
	SDL_Texture* rightTexture;
	SDL_Texture* oppo;
	SDL_Rect CarRect;
	SDL_Rect Opponents;

	SDL_Texture* bgtexture;
	SDL_Rect BgRect;
	int nCarPosition = 0;
	int nBgPosition = 0;

	float ScoreValue;
	TTF_Font* font;

	//The music that will be played
	Mix_Music* bgm2[3];
	int currentBGM;
};
