#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
//gcc -I/usr/include/sdl -L/usr/lib -pthread -lSDL2 -lSDL2_mixer  main.c
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

int main(int argc, char ** argv) {
  /* Declare window and renderer */

  /* Initialize SDL */
  SDL_Init(SDL_INIT_EVERYTHING);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) 
  { 
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError()); 
    return 1; 
  } 
  int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) 
  { 
    fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError()); 
    exit(1); 
  } 
  Mix_Chunk *sound = NULL;
  sound = Mix_LoadWAV("sound.wav"); 
  if(sound == NULL) 
  { 
    fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError()); 
  } 
  int channel;
  /* Main game loop */
  while (!SDL_QuitRequested()) 
  {
    channel = Mix_PlayChannel(-1, sound, 0); 
    if(channel == -1) 
    { 
      fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError()); 
    } 
    while(Mix_Playing(channel) != 0);
  }
  Mix_FreeChunk(sound);
  Mix_CloseAudio(); 
  /* Deinitialize everything */
  SDL_Quit();
  return 0;
}

