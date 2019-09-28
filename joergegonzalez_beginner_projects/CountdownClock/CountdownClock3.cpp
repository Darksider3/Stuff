#include <iostream>
#include <bass.h>
#include <cstdlib>
#include <thread>
#include <chrono>

int main()
{
  BASS_Init(-1, 44100, 0, 0, NULL);
  BASS_SetVolume(1);
  HSAMPLE sample = BASS_SampleLoad(false, "sound.wav", 0, 0, 1, BASS_SAMPLE_FLOAT);
  HCHANNEL channel=BASS_SampleGetChannel(sample, FALSE);
  BASS_ChannelPlay(channel, FALSE);
  std::this_thread::sleep_for(std::chrono::seconds(5));
}
