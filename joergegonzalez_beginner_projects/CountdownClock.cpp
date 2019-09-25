#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <iostream>
#include <chrono>
#include <thread>

//gcc -I/usr/include/sdl -L/usr/lib -pthread -lSDL2 -lSDL2_mixer  main.c
//#define WINDOW_WIDTH 600
//#define WINDOW_HEIGHT 600
bool quit = false;

void playSoundOnce(std::string soundFile, std::chrono::duration<float> T=std::chrono::milliseconds(100))
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
  std::this_thread::sleep_for(T);
  Mix_FreeChunk(sound);
  Mix_CloseAudio(); 
}

int EventFilterTest(void *userdata, SDL_Event *event)
{
  if(!userdata)
    return 0;
  if(event->type == SDL_KEYDOWN)
  {
    quit = true;
    return 1;
  }
  else if(event->type == SDL_QUIT)
  {
    quit = true;
    return 1;
  }
  else
  {
    return 0;
  }
}

int main(int argc, char ** argv) {
  /* Initialize SDL */
  //std::chrono::duration<float> Timer;
  std::chrono::duration<double> Timer = std::chrono::milliseconds(1000);
  if(argc > 1)
  {
    int temp = std::stoi(argv[2]);
    std::string tmpStr = std::string(argv[1]);
    if(tmpStr == "ms")
      Timer = std::chrono::milliseconds(temp);
    else if (tmpStr == "s")
      Timer = std::chrono::seconds(temp);
    else if (tmpStr == "m")
      Timer = std::chrono::minutes(temp);
    else if (tmpStr == "h")
      Timer = std::chrono::hours(temp);

  }
  SDL_Init(SDL_INIT_EVERYTHING);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) 
  { 
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError()); 
    return 1; 
  } 
  SDL_Event event;
  int counter= Timer.count();
  while(quit == false )
  {
    if(counter!=0)
    {
      std::cout << '\b' << '\r' << "\033[K" << "Time remaining: " << counter--;
      std::cout.flush();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    else 
      playSoundOnce("sound.wav");
  }
  /* Deinitialize everything */
  SDL_Quit();
  return 0;
}

