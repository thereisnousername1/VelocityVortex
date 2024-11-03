#include <iostream>
#include "Menu.h"
#include "SDL2-2.28.4\include\SDL.h"
#include "SDL2_ttf-2.22.0\include\SDL_ttf.h"
#include "SDL2_image-2.8.2\include\SDL_image.h"
#include "SDL2_mixer-2.8.0\include\SDL_mixer.h"

using namespace std;

void Menu::Init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        // printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return;
    }

    window = SDL_CreateWindow("Velocity Vortex", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Init();

    // this font cannot load number and punctuation, it is too weak
    font = TTF_OpenFont("Freedom-10eM.ttf", 28); // font and font size

    bgtexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("bg_full.jpg"));

    // bgm1 = Mix_LoadMUS("");
    bgm1 = Mix_LoadMUS("127-small_car-loop.wav");
}

void Menu::RenderText(const string& text, int x, int y) {
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Menu::StartPage() {
    bool running = true;
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);
    SDL_Surface* bgimage;

    // starting points
    int titlex = (WINDOW_WIDTH - 200) / 2, titley = WINDOW_HEIGHT / 6 * 1.5;
    int startx = (WINDOW_WIDTH - 200) / 2, starty = WINDOW_HEIGHT / 2.2;
    int exitx = (WINDOW_WIDTH - 200) / 2, exity = starty + 100;

    //Play the music
    Mix_PlayMusic(bgm1, -1);

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (state[SDL_SCANCODE_ESCAPE]) {
                // width 100px, height 50px, not precise
                running = false;
                SDL_Quit();
                exit(0);
            }
            if (state[SDL_SCANCODE_RETURN])
                running = false;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                // check cursor position
                int x, y;
                SDL_GetMouseState(&x, &y);
                // position of the start button
                if (x >= startx && x <= startx + 200 && y >= starty && y <= starty + 50) {
                // width 200px, height 50px, not precise
                    running = false;
                }
                // position of the exit button
                if (x >= exitx && x <= exitx + 100 && y >= exity && y <= exity + 50) {
                    // width 100px, height 50px, not precise
                    running = false;
                    SDL_Quit();
                    exit(0);
                }
            }
        }

        // clear the screen before start to render
        SDL_RenderClear(renderer);

        BgRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, bgtexture, nullptr, &BgRect);

        // layout
        RenderText("Velocity Vortex", titlex, titley);
        RenderText("Start Game", startx, starty);
        RenderText("Exit", exitx, exity);

        // update the screen
        SDL_RenderPresent(renderer);
    }
}

void Menu::CleanUp() {
    // Do not call clean up when the program is not done with it yet!!!
    // Or I should initialize it in the new window if TTF_Quit happened
    TTF_CloseFont(font); 
    TTF_Quit();
    Mix_FreeMusic(bgm1);

    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    // Game is not play yet, don't quit or the music don't work at all
    // SDL_Quit();
}