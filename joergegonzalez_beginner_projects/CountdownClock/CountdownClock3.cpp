#include <iostream>
#include <bass.h>
#include <cstdlib>
#include <thread>
#include <chrono>
class BASSer
{
private:
  HSAMPLE sample;
  HCHANNEL channel;
public:
  BASSer(int freq, float volume, std::string filename)
  {
    BASS_Init(-1, freq, 0, 0, NULL);
    BASS_SetVolume(volume);
    sample = BASS_SampleLoad(false, filename.c_str(), 0, 0, 1, BASS_SAMPLE_FLOAT);
    channel = BASS_SampleGetChannel(sample, FALSE);
  }

  bool volume(float Vol)
  {
    if(BASS_SetVolume(Vol))
      return true;
    return false;
  }

  void Play()
  {
    BASS_ChannelPlay(channel, FALSE);
  }

  bool Stop()
  {
    return BASS_Stop();
  }

  bool Pause()
  {
    return BASS_Pause();
  }

  bool Resume()
  {
    return BASS_Start();
  }

  bool isStarted()
  {
    return BASS_IsStarted();
  }
  ~BASSer()
  {
    BASS_SampleFree(sample);
    BASS_Free();
  }
};
int main()
{
  BASS_Init(-1, 44100, 0, 0, NULL);
  BASS_SetVolume(1);
  HSAMPLE sample = BASS_SampleLoad(false, "sound.wav", 0, 0, 1, BASS_SAMPLE_FLOAT);
  HCHANNEL channel=BASS_SampleGetChannel(sample, FALSE);
  while(1)
  {
    BASS_ChannelPlay(channel, FALSE);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
