#include <iostream>
#include <string>
#include <vector>
#include "Game.h"
#include "C:\public\SDL\SDL2-2.28.4\include\SDL.h"
#include "C:\public\SDL\SDL2_ttf-2.22.0\include\SDL_ttf.h"
#include "C:\public\SDL\SDL2_image-2.8.2\include\SDL_image.h"
#include "C:\public\SDL\SDL2_mixer-2.8.0\include\SDL_mixer.h"
#include "Track.h"

using namespace std;

Track t;

Game::Game() {
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) && IMG_INIT_PNG)) {
        cerr << "Image could not initialize! IMG_Init Error: " << IMG_GetError() << endl;
        SDL_Quit();
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return;
    }

    window = SDL_CreateWindow("Velocity Vortex", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bgtexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("bg.png"));
    straightTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("Player_Straight.png"));
    leftTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("Player_Left.png"));
    rightTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("Player_Right.png"));
    oppo = SDL_CreateTextureFromSurface(renderer, IMG_Load("opponent.png"));

    currentBGM = 0;
    bgm2[0] = Mix_LoadMUS("generic-trance.wav");
    bgm2[1] = Mix_LoadMUS("127-small_car-loop.wav");
    bgm2[2] = Mix_LoadMUS("107bpm_fxbreak_loop.wav");

    TTF_Init();
    font = TTF_OpenFont("arial.ttf", 28); // font and font size
};

void Game::UserInterface(const string& text, int x, int y) {
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Debug
    // cout << "Speed: " << text.c_str() << ", surface width: " << surface->w << ", surface height: " << surface->h << endl;
}
// Destructor
Game::~Game() {
    TTF_CloseFont(font);
    TTF_Quit();
    // easier than for loop
    Mix_FreeMusic(bgm2[0]);
    Mix_FreeMusic(bgm2[1]);
    Mix_FreeMusic(bgm2[2]);

    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};

void Game::InitializePage() {
    // Render the initialization page
    bool initializing = true;

    SDL_RenderClear(renderer);

    int startx = (WINDOW_WIDTH - 200) / 2, starty = WINDOW_HEIGHT / 2.4;
    int exitx = (WINDOW_WIDTH - 200) / 2, exity = starty + 100;

    // reset everything
    t.Section = 1;
    ScoreValue = 0.0f;
    // nBgPosition = (WINDOW_WIDTH / 2) - 800;
    nCarPosition = (WINDOW_WIDTH / 2) - (TILE_SIZE / 2);
    t.TrackCurvature = 0.0f;
    t.PlayerCurvature = 0.0f;
    t.Speed = 0;
    ElapsedTime = 0;

    while (initializing) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_SCANCODE_ESCAPE) {
                // width 100px, height 50px, not precise
                initializing = false;
                SDL_Quit();
                exit(0);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                // check cursor position
                int x, y;
                SDL_GetMouseState(&x, &y);
                // position of the start button
                if (x >= startx && x <= startx + 200 && y >= starty && y <= starty + 50) {
                    initializing = false; // Start the game
                    Mix_ResumeMusic();
                    currentState = PLAYING;
                }
                // position of the exit button
                if (x >= exitx && x <= exitx + 100 && y >= exity && y <= exity + 50) {
                    // width 100px, height 50px, not precise
                    initializing = false; 
                    SDL_Quit();
                    exit(0);
                }
            }
        }

        // Clear screen and render initialization graphics/text
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);

        // Example: Render text or graphics here
        UserInterface("Retry", startx, starty);
        UserInterface("Exit", exitx, exity);

        SDL_RenderPresent(renderer);
        SDL_Delay(100); // Limit frame rate
    }
}

// Game mechanic
void Game::Run() {
    // InitializePage(); // Start with the initialization page
    Gameloop();
    while (true) {
        if (currentState == PLAYING) {
            Gameloop(); // Start the game loop
        }
        else if (currentState == INITIALIZE) {
            InitializePage(); // Render the initialization page again if needed
        }
    }
};

void Game::Gameloop() {
    bool running = true;

    string tempSpeed;
    string Score;
    ScoreValue = 0.0f;

    int randomNum = rand() % 2; // This will give 0 or 1

    Opponents = { WINDOW_WIDTH / 2, (WINDOW_HEIGHT / 2) + TILE_SIZE, TILE_SIZE, TILE_SIZE };

    Mix_PlayMusic(bgm2[0], -1);

    while (running) {

        SDL_Delay(32);
        // cout << ElapsedTime;
        t.setElapsedTime(ElapsedTime);

        Input();

        // SDL_RenderClear(renderer);

        t.renderer = renderer;
        tempSpeed = "Current Speed: " + to_string(static_cast<int>(fabs(t.Speed * 150)));
        Score = "Score: " + to_string(static_cast<int>(ScoreValue));
        t.Draw();

        HandleCollisions();

        CarPosition = t.PlayerCurvature - t.TrackCurvature;

        if (nCarPosition >= 0 && nCarPosition <= WINDOW_WIDTH) {    // make sure it is on screen
            nCarPosition = (WINDOW_WIDTH / 2) + ((int)WINDOW_WIDTH * CarPosition) / 2.0f - 20;
            nBgPosition = (WINDOW_WIDTH / 2) + ((int)WINDOW_WIDTH * CarPosition) / 2.0f - 800;
        }
        else {  // reset car position and speed
            nCarPosition = (WINDOW_WIDTH / 2) - (TILE_SIZE / 2);
            // t.TrackCurvature = 0.0f;
            t.PlayerCurvature = 0.0f;
            t.Speed = 0;
            ElapsedTime = 0;
        }

        // CarRect = { (WINDOW_WIDTH / 2) - 20, WINDOW_HEIGHT - 100 , 40, 40 }; // Position and size of the image
        CarRect = { nCarPosition, WINDOW_HEIGHT - 100, TILE_SIZE, TILE_SIZE };
        BgRect = { nBgPosition, 0, 1600, 300 };
        SDL_RenderCopy(renderer, player, nullptr, &CarRect);
        SDL_RenderCopy(renderer, bgtexture, nullptr, &BgRect);

        // Movement logic for opponents
        // Check the player's speed
        if (t.Speed == 0) {
            if (Opponents.y < WINDOW_HEIGHT / 2) {
                Opponents.y = WINDOW_HEIGHT + 1000;
            }
            else
                // Move opponent up and vanish when the player stops
                Opponents.y -= 2; // Adjust this value for how quickly they move up
        }
        else if (t.Speed >= 1.0f) {
            // When the player accelerates, opponents approach the bottom
            // Adjust position based on track curvature and boundary conditions
            if (Opponents.x < WINDOW_WIDTH / 3 || Opponents.x >(WINDOW_WIDTH / 3) * 2) {
                // Turn towards center if near the left or right edge
                Opponents.x += (t.Curvature * 25.0f) * (randomNum == 0 ? 1 : -1);
            }
            else {
                // Move smoothly based on curvature
                Opponents.x += t.Curvature * 25.0f;
            }

            Opponents.y += OPPO_MOVE_SPEED; // Adjust the multiplier for speed of descent
            // when the tile reached the bottom, reset to half of the screen
            if (Opponents.y > WINDOW_HEIGHT) {
                Opponents.x = (WINDOW_WIDTH / 2) - (TILE_SIZE / 2);
                Opponents.y = WINDOW_HEIGHT / 2;
            }
        }

        SDL_RenderCopy(renderer, oppo, nullptr, &Opponents);

        // UI layout
        UserInterface(tempSpeed, 10, 10);
        UserInterface(Score, 10, 48);
        UserInterface("Press w to accelarate", 520, 10);
        UserInterface("Press a and d to steer", 510, 48);
        UserInterface("Press LShift to boost", 525, 86);
        UserInterface("Press r to reset position", 490, 124);
        UserInterface("Press < and > to choose your bgm!", 350, 162);

        SDL_RenderPresent(renderer);
    }
}

void Game::HandleCollisions() {
    // Collision check
    if (SDL_HasIntersection(&CarRect, &Opponents)) {
        // Handle collision
        // cout << "Collision detected with opponent !" << endl;
        Opponents.y = WINDOW_HEIGHT / 2;
        Mix_PauseMusic();
        currentState = INITIALIZE; // Change state to initialize
        InitializePage(); // Call the initialization page
        return; // Exit the collision handling
    }
}

void Game::PlayMusic() {
    // Ensure music is playing if initialized
    Mix_HaltMusic();
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(bgm2[currentBGM], -1);
    }
}

// Control
void Game::Input() {
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&event)) {
        // Terminate the game
        if (state[SDL_SCANCODE_ESCAPE]) {
            SDL_Quit();
            exit(0);
        }
    }

    // SDL_SCANCODE_COMMA = 54
    // SDL_SCANCODE_PERIOD = 55

    if (state[SDL_SCANCODE_COMMA]) {
        // Next soundtrack
        currentBGM = (currentBGM + 1) % 3; // Loop through tracks
        PlayMusic();
    }

    if (state[SDL_SCANCODE_PERIOD]) {
        // Previous soundtrack
        currentBGM = (currentBGM - 1 + 3) % 3; // Loop through tracks
        PlayMusic();
    }

    if (state[SDL_SCANCODE_W]) {
        ElapsedTime += 0.001;
        t.Speed += (1.0f + t.speedOffset) * (0.5f * ElapsedTime); // Increase speed
        if (t.Speed < 0.0f)
            t.Speed = 0.0f;
        if (t.Speed > t.SpeedLimit) {
            t.Speed = t.SpeedLimit;
            // Score increase faster in boosted state
            if (t.SpeedLimit == 2)
                ScoreValue++;
            ScoreValue += 1;
        }
        t.Move();   // calling it to speed up
    }
    else {
        t.Speed -= 0.5f * ElapsedTime; // Decrease speed
        t.Move();   // calling it to slow down
        if (t.Speed == 0.0f)
            ElapsedTime = 0.0f;
    }

    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        if(t.Speed > 0)
            t.PlayerCurvature -= 0.25f * (1.0f - t.Speed / 2.0f);
        player = leftTexture;
    }
    else if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        if (t.Speed > 0)
            t.PlayerCurvature += 0.25f * (1.0f - t.Speed / 2.0f);
        player = rightTexture;
    }
    else {
        player = straightTexture;
    }

    if (state[SDL_SCANCODE_R]) {
        nCarPosition = (WINDOW_WIDTH / 2) - (TILE_SIZE / 2);
        t.TrackCurvature = 0.0f;
        t.PlayerCurvature = 0.0f;
        t.Speed = 0;
        ElapsedTime = 0;
    }

    // Check if the car is off track
    if (fabs(CarPosition) >= 0.8f) {
        t.Speed -= 2.0f * ElapsedTime; // Slow down if off track
    }
    // shift gear
    if (state[SDL_SCANCODE_LSHIFT]) {
        t.Boost();
    }
}