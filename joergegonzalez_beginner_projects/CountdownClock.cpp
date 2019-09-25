#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <iostream>
#include <chrono>
#include <thread>

//gcc -I/usr/include/sdl -L/usr/lib -pthread -lSDL2 -lSDL2_mixer  main.c
//#define WINDOW_WIDTH 600
//#define WINDOW_HEIGHT 600


void playSoundOnce(std::string soundFile)
{
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 4096;
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
  {
    fprintf(stderr, "Unable to init audio: %s\n", Mix_GetError());
    return;
  }
  Mix_Chunk *sound = NULL;
  sound = Mix_LoadWAV(soundFile.c_str());
  if(sound == NULL)
  {
    fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
    return;
  }
  int channel;
  /* Main game loop */
  channel = Mix_PlayChannel(-1, sound, 0); 
  if(channel == -1) 
  { 
    fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError()); 
  } 
  while(Mix_Playing(channel) != 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  Mix_FreeChunk(sound);
  Mix_CloseAudio(); 
}


int main(int argc, char ** argv) {
  /* Initialize SDL */
  SDL_Init(SDL_INIT_EVERYTHING);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) 
  { 
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError()); 
    return 1; 
  } 
  SDL_Event event;
  bool quit = false;
  while(quit == false )
  {
    std::cout << "in loop \n";
    if(SDL_WaitEvent(&event) != 0)
    {
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
      if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
      {
        quit = true;
      }
    }
    playSoundOnce("sound.wav");
    std::cout << "play!\n";
  }
  playSoundOnce("sound.wav");
  /* Deinitialize everything */
  SDL_Quit();
  return 0;
}

