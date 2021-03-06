#include <iostream>
#include <csignal>
#include <ao/ao.h>
#include <sndfile.h>
#include <unistd.h>
#include <cerrno>
#define BUFFER_SIZE 4096

bool run = true;
struct Timer
{
  size_t time;
  enum type {MILLISECONDS, SECONDS, MINUTES, HOURS};
  std::string FileToPlay;

  explicit Timer(std::string n)
  {
    FileToPlay = n;
  }
  explicit Timer()
  {
    FileToPlay = "./sound.wav";
  }
  static void clean(ao_device *device, SNDFILE *file)
  {
    ao_close(device);
    sf_close(file);
    ao_shutdown();
  }
  int play()
  {
    ao_device *device;
    ao_sample_format format;
    SF_INFO sfinfo;

    int default_driver;

    short *buffer;
    SNDFILE *file = sf_open(FileToPlay.c_str(), SFM_READ, &sfinfo);
    //printf("Samples: %d\n", sfinfo.frames);
    //printf("Sample rate: %d\n", sfinfo.samplerate);
    //printf("Channels: %d\n", sfinfo.channels);
    ao_initialize();
    default_driver = ao_default_driver_id();
    switch(sfinfo.format & SF_FORMAT_SUBMASK)
    {
     case SF_FORMAT_PCM_16:
        format.bits = 16;
        break;
      case  SF_FORMAT_PCM_24:
        format.bits = 24;
        break;
      case SF_FORMAT_PCM_32:
        format.bits = 32;
        break;
      case SF_FORMAT_PCM_S8:
        format.bits = 8;
        break;
      case SF_FORMAT_PCM_U8:
        format.bits = 8;
      default:
        format.bits = 16;
        break;
    }
    format.channels = sfinfo.channels;
    format.rate = sfinfo.samplerate;
    format.byte_format = AO_FMT_NATIVE;
    device = ao_open_live(default_driver, &format, NULL);
    if(device == NULL)
    {
      std::string err="Device id: "+std::to_string(ao_default_driver_id())+"\n";
      err+="Format.channels: "+std::to_string(format.channels) +"\n";
      err+="Format.rate: "+std::to_string(format.rate)+"\n";
      err+="Format.byte_format: "+std::to_string(format.byte_format)+"\n";
      err+="Format.bits: "+std::to_string(format.bits)+"\n";
      switch(errno)
      {
        case AO_ENODRIVER:
          err+="No driver corresponds to driver_id\n";
          break;
        case AO_ENOTLIVE:
          err+="Driver has no live output device\n";
          break;
        case AO_EBADOPTION:
          err+="Invalid value for valid option key\n";
          break;
        case AO_EOPENDEVICE:
          err+="Cannot open the device, probably due to permissions\n";
          break;
        case AO_EFAIL:
          err+="Some other source of failure\n";
          break;
        default:
          break;
      }
      fprintf(stderr, err.c_str());
      fprintf(stderr, "Error opening device... \n");
      return 1;
    }

    buffer = new short[BUFFER_SIZE];
    while(1)
    {
      int read = sf_read_short(file, buffer, BUFFER_SIZE);
      fprintf(stderr, "Read: %i\n", read);
      if(ao_play(device, (char *) buffer, (uint_32) (read * sizeof(short))))// == 0)
      {
        printf("ao_play: failed.\n");
        clean(device, file);
        break;
      }
      if(run==false)
      {
        clean(device, file);
        break;
      }
    }
    return 0;
  }
};

void sigHandler(int num)
{
  if(num!=0)
    run = false;
  run = false;
}


int main()
{
  std::signal(SIGINT, sigHandler);
  Timer Player;
  while(run)
  {
    Player.play();
    sleep(5);
  }
}
