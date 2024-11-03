#pragma once
#include "SDL2-2.28.4\include\SDL.h"
#include "SDL2_image-2.8.2\include\SDL_image.h"
#include "Constants.h"

using namespace std;

class Track {

public:
    SDL_Renderer* renderer;
    bool Boosted = false;
    int Section = 0;
    int speedOffset = 0;
    float Speed = 0.0f;
    float TrackCurvature = 0.0f;
    float PlayerCurvature = 0.0f;			// Accumulation of player curvature
    float Dist = 0.0f;
    float Curvature = 0.0f;
    float TimePassed = 0.0f;
    float SpeedLimit = 1.0f;

    // this denfine the track, first one is how curvy the curve is, second one is the distance until next turn
    vector<pair<float, float>> Turn;

    Track() {

        Turn.push_back(make_pair(0.0f, 10.0f));		// Short section for start/finish line
        Turn.push_back(make_pair(0.0f, 200.0f));
        Turn.push_back(make_pair(1.0f, 200.0f));
        Turn.push_back(make_pair(0.0f, 400.0f));
        Turn.push_back(make_pair(-1.0f, 100.0f));
        Turn.push_back(make_pair(0.0f, 200.0f));
        Turn.push_back(make_pair(-1.0f, 200.0f));
        Turn.push_back(make_pair(1.0f, 200.0f));
        Turn.push_back(make_pair(0.0f, 200.0f));
        Turn.push_back(make_pair(0.2f, 500.0f));
        Turn.push_back(make_pair(0.0f, 200.0f));

    }

    void setElapsedTime(float Time) {
        TimePassed = Time;
    }

    void Draw() {

        // Turning happened
        float TurnOffset = 0;
        Section = 0;

        while (Section < Turn.size() && TurnOffset <= Dist) // Track loop
        {
            TurnOffset += Turn[Section].second;
            Section++;

            // endless track
            if (Section == Turn.size())
                Section = 1;
        }

        // Track perspective, from 2D to pseudo 3D
        float TargetCurvature = Turn[Section - 1].first;

        float TrackCurveDiff = (TargetCurvature - Curvature) * TimePassed * Speed;

        Curvature += TrackCurveDiff * 0.005;

        TrackCurvature += Curvature * TimePassed * Speed;

        for (int y = 0; y < WINDOW_HEIGHT / 2; y++) {   // Track itself
            // Calculate perspective and road segments for the current row
            float Perspective = (float)y / (WINDOW_HEIGHT / 2.0f);
            float MidPoint = 0.5f + Curvature * powf((1.0f - Perspective), 3);
            float RoadWidth = 0.1f + Perspective * 0.8f; // Min 10% Max 90%
            RoadWidth *= 0.5f; // Adjust to half width
            float ClipWidth = RoadWidth * 0.15f;

            int Lborder = (MidPoint - RoadWidth - ClipWidth) * WINDOW_WIDTH;
            int LClip = (MidPoint - RoadWidth) * WINDOW_WIDTH;
            int Rborder = (MidPoint + RoadWidth + ClipWidth) * WINDOW_WIDTH;
            int RClip = (MidPoint + RoadWidth) * WINDOW_WIDTH;
            int Row = WINDOW_HEIGHT / 2 + y;

            // int nClipColour = sinf(80.0f * powf(1.0f - Perspective, 2) + Dist) > 0.0f ? FG_RED : FG_WHITE;

            // int nRoadColour = (nTrackSection - 1) == 0 ? FG_WHITE : FG_GREY;

            // Draw segments based on calculated widths
            // Left border
            if (Lborder > 0) {
                // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                // green or darker green
                SDL_SetRenderDrawColor(renderer, 0,
                    (sinf(20.0f * powf(1.0f - Perspective, 3) + Dist * 0.1f) > 0.0f) ? 255 : 100,
                    0, 255);
                SDL_RenderDrawLine(renderer, 0, Row, Lborder, Row);
            }
            // Left clip
            if (LClip > Lborder) {
                // red or white tile
                SDL_SetRenderDrawColor(renderer, 255,
                    sinf(80.0f * powf(1.0f - Perspective, 2) + Dist) > 0.0f ? 0 : 255,
                    sinf(80.0f * powf(1.0f - Perspective, 2) + Dist) > 0.0f ? 0 : 255,
                    255);
                SDL_RenderDrawLine(renderer, Lborder, Row, LClip, Row);
            }
            // Road
            if (RClip > LClip) {
                // (TrackSection - 1) == 0 ? FG_WHITE : FG_GREY;
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawLine(renderer, LClip, Row, RClip, Row);
            }
            // Right clip
            if (Rborder > RClip) {
                // red or white tile
                SDL_SetRenderDrawColor(renderer, 255,
                    sinf(80.0f * powf(1.0f - Perspective, 2) + Dist) > 0.0f ? 0 : 255,
                    sinf(80.0f * powf(1.0f - Perspective, 2) + Dist) > 0.0f ? 0 : 255,
                    255);
                SDL_RenderDrawLine(renderer, RClip, Row, Rborder, Row);
            }
            // Right border
            if (Rborder < WINDOW_WIDTH) {
                // green or darker green
                SDL_SetRenderDrawColor(renderer, 0,
                    (sinf(20.0f * powf(1.0f - Perspective, 3) + Dist * 0.1f) > 0.0f) ? 255 : 100,
                    0, 255);
                SDL_RenderDrawLine(renderer, Rborder, Row, WINDOW_WIDTH, Row);
            }
        }

        SDL_RenderPresent(renderer); // Present the renderer after all drawing is done
    }

    void Move() {
        if (Speed < 0.0f)
            Speed = 0.0f;
        if (Speed > SpeedLimit)
            Speed = SpeedLimit;
        Dist += (10.0f * Speed) * TimePassed;
    }

    void Boost() {
        if (Boosted == false)
        {
            speedOffset = 1;
            Boosted = true;
            SpeedLimit = 1.5f;
        }
        else
        {
            speedOffset = 0;
            Boosted = false;
            SpeedLimit = 1;
        }
    }
};