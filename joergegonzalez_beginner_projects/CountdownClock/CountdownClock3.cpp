#include <iostream>
#include <bass.h>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <cctype>
#include <vector>
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

bool isNum(std::string const &s)
{
  bool justdigits=true;
  for(char Ch: s)
  {
    if(!std::isdigit(Ch))
    {
      justdigits = false;
      return justdigits;
    }
  }

  return justdigits;
}

int main(int argc, char **argv)
{
  BASS_Init(-1, 44100, 0, 0, NULL);
  BASS_SetVolume(1);
  HSAMPLE sample = BASS_SampleLoad(false, "sound.wav", 0, 0, 1, BASS_SAMPLE_FLOAT);
  HCHANNEL channel=BASS_SampleGetChannel(sample, FALSE);
  
//@TODO up to start before BASS
  if(argc <= 3)
  {
  }
  //std::chrono::duration<double> Timer = std::chrono::milliseconds(10000);

  std::chrono::duration<double> tp = std::chrono::milliseconds(0);
  //anonym scope
  {
    std::string tmp;
    struct Timings
    {
      std::vector<int> Nums;
      std::vector<std::string> Signs;
    } TMNG;
    for(int i = 1; i != argc; ++i)
    {
      tmp = argv[i];
      if(isNum(tmp))
      {
        TMNG.Nums.emplace_back(std::stoi(tmp));
      }
      else if(tmp == "ms")
        TMNG.Signs.emplace_back(tmp);
      else if(tmp == "s")
        TMNG.Signs.emplace_back(tmp);
      else if(tmp == "ts") //tenseconds
        TMNG.Signs.emplace_back(tmp);
      else if(tmp == "m")
        TMNG.Signs.emplace_back(tmp);
      else if(tmp == "h")
        TMNG.Signs.emplace_back(tmp);
      else if(tmp == "d")
        TMNG.Signs.emplace_back(tmp);
    }
    if(TMNG.Signs.size() != TMNG.Nums.size())
    {
      std::cout << "nope, you likely forgot or added to much paramters. Inequal Numbers/Signs ratio\n";
    }
    else
    {
      for(size_t i = 0; i != TMNG.Nums.size(); ++i)
      {
        std::cout << "Num: " << TMNG.Nums[i] << "; Sign: " << TMNG.Signs[i] << ". \n";
        if(TMNG.Signs[i] == "ms")
          tp += std::chrono::milliseconds(TMNG.Nums[i]);
        else if(TMNG.Signs[i] == "s")
          tp += std::chrono::seconds(TMNG.Nums[i]);
        else if(TMNG.Signs[i] == "ts") //tenseconds
          tp += std::chrono::seconds(TMNG.Nums[i]*10);
        else if(TMNG.Signs[i] == "m")
          tp += std::chrono::minutes(TMNG.Nums[i]);
        else if(TMNG.Signs[i] == "h")
          tp += std::chrono::hours(TMNG.Nums[i]);
        else if(TMNG.Signs[i]== "d")
          tp += std::chrono::hours(TMNG.Nums[i]*24);
      }
    }
  }
  std::cout << "Milliseconds TP " << tp.count()  << "\n";
  /*typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> MTP;

  MTP want = std::chrono::time_point_cast<MTP::duration>(std::chrono::system_clock::time_point());
  want+=std::chrono::seconds(5);
  */
  auto want = std::chrono::system_clock::now()+tp;
  auto cur = std::chrono::system_clock::now();
  while(1)
  {
    cur = std::chrono::system_clock::now();
    std::chrono::duration<float> diff = cur-want;
    if(diff.count() >= 0)
    {
      BASS_ChannelPlay(channel, FALSE);
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    else
    {
      std::cout <<'\b' << '\r' << "\033[K" << "Time remaining: " << (int)std::abs(diff.count());
      std::cout.flush();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}
