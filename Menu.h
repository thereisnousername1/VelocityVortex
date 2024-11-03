#pragma once
#include <iostream>
#include "SDL2-2.28.4\include\SDL.h"
#include "SDL2_ttf-2.22.0\include\SDL_ttf.h"
#include "SDL2_image-2.8.2\include\SDL_image.h"
#include "SDL2_mixer-2.8.0\include\SDL_mixer.h"
#include "Constants.h"

using namespace std;

class Menu {
public:
    void Init();
    void RenderText(const string& text, int x, int y);
    void StartPage();
    void CleanUp();
    SDL_Window* window;
    SDL_Renderer* renderer;

private:
    TTF_Font* font;
    SDL_Texture* bgtexture;
    SDL_Rect BgRect;
    //The music that will be played
    Mix_Music* bgm1;

};