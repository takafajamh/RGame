#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/Test.hpp>

int main(int argc, char** argv)
{
    spdlog::info("Starting SDL3...");

    if (SDL_Init(SDL_INIT_VIDEO) != true) 
    {
        spdlog::error("SDL_Init failed: {}", SDL_GetError());
        return -1;
    }

    spdlog::info("SDL3 Initialized!");

    // Inicjalizacja okna SDL
    SDL_Window* window = SDL_CreateWindow("SDL3 Test", 800, 600, 0);
    if (!window) 
    {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    spdlog::info("Window created!");


    ThisIsATest();


    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
