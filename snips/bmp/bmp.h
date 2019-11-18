#ifndef BMP_H
#define BMP_H
#include <fstream>
#include <filesystem>
#include <iostream>
#include <byteswap.h>
#include <vector>

struct X {
  ~X() { std::cout << std::endl; }
};

#define DBG (X(), std::cout << __FILE__ << ":"  << __LINE__ << ": " <<  __PRETTY_FUNCTION__ << ": ")

namespace bmp {
//packing needed because we cant allow alignment at Size
#pragma pack(push, 1)
//__attribute__((packed))
struct BitmapFileHeader
{
  uint16_t Type; // Magic identifier, big-Endian 424d, little-endian 4d42
  uint32_t Size; // File size in Bytes
  uint16_t Reserved1; // unused(spec)
  uint16_t Reserved2; // unused(spec)
  uint32_t Offset; // offset to image data in bytes from beginning of file(54 bytes v5)
  uint32_t dib_header_size; // DIB Header in bytes. NT 3.1 => 40, V2->52, V3->56, V4=>108, V5 => 124
};

struct DIB_BITMAPINFOHEADER
{
  int32_t width_px; // width of the image
  int32_t height_px; // height of the image
  uint16_t num_planes; // number of color planes
  uint16_t bits_per_pixel; // bits per pixel
  uint32_t compression; // compression method
  uint32_t image_size_byte; // image size in bytes
  int32_t x_resolution_ppm; // PPM => Pixels per meter
  int32_t y_resolution_ppm; // PPM => Pixels per meter
  uint32_t num_colors; // number of colors;
  uint32_t important_colors; // important colors
};


#pragma pack(pop)
struct GenericPixel
{};
struct Pixel_BGR24 : GenericPixel
{
  uint8_t B;
  uint8_t G;
  uint8_t R;
};

struct Pixel_BGRA32 : GenericPixel
{
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t A;
};

struct Pixel_BGR12 : GenericPixel
{
  unsigned B: 4;
  unsigned G: 4;
  unsigned R: 4;
};

struct Pixel_BGR4_Indexed : GenericPixel
{
  unsigned B: 1;
  unsigned G: 1;
  unsigned R: 1;
  unsigned trashbyte: 1; // get's set to 0
};

struct Pixel_BlackWhite : GenericPixel
{
  bool W;
  bool B;
};

class ReadHeader
{

};

template<typename T = GenericPixel>
class GenericProcessor {
public:
  std::ifstream f;
  inline bool exist(std::string &FN){return std::filesystem::exists(FN);}
  std::vector<T> DATA;

  BitmapFileHeader header;
  DIB_BITMAPINFOHEADER dib;

  GenericProcessor(std::string &FN)
  {
    if(!exist(FN))
    {
      std::cout << "The file " << FN <<" doesn't exist.\n";
      abort();
    }
    if(std::filesystem::file_size(FN) < 54)
    {
      std::cout << "The file " << FN << " is not a valid BMP file.\n";
      abort();
    }
    f.open(FN, std::ios::binary);
    init();
  }

  void init();

  void readBMPHeader()
  {
    f.read(reinterpret_cast<char*>(&header), sizeof(BitmapFileHeader));
    f.read(reinterpret_cast<char*>(&dib), sizeof(DIB_BITMAPINFOHEADER));
  }
  void readDIBHeader();
};

class BW_Process : public GenericProcessor<Pixel_BlackWhite>
{
  struct COLORARRAY
  {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
  };
public:
  COLORARRAY *colorArray;
  virtual void init()
  {
    colorArray = new COLORARRAY[dib.num_colors];

    if(dib.compression != 0)
    {
      std::cout << "BI_RGB-Compression isnt currently supported.\n";
      abort();
    }
  }

  void readColorTable()
  {
    COLORARRAY tmp;
    f.seekg(14+header.dib_header_size);
    for(long long int i = 0; i != dib.num_colors; ++i)
    {
      f.read(reinterpret_cast<char*>(&tmp), sizeof(COLORARRAY));
      colorArray[i].R = tmp.R;
      colorArray[i].G = tmp.G;
      colorArray[i].B = tmp.B;
      colorArray[i].A = tmp.A;
    }
  }
  void readData()
  {/*
    COLORARRAY tmp;
    uint8_t trash;

    f.seekg(header.Offset);
  */}
  virtual ~BW_Process()
  {
    delete[] colorArray;
  }
};
class Impl
{
protected:
  uint8_t paddingBytes; // Each "row"(x-coordinate) must be a multiple of 4, thus max 3 bytes padding
  std::ifstream f;
  inline bool exist(std::string &FN){return std::filesystem::exists(FN);}
public:
  std::vector<Pixel_BGR24> DATA;
  BitmapFileHeader header;
  DIB_BITMAPINFOHEADER dib;
  Impl(std::string&);
  void readHeaders();
  bool check_header();
  void readData();

  std::shared_ptr<uint8_t[]> getDataSmartUint8RGBA();
  uint8_t *legacyUint8RGBA();
  virtual ~Impl();
};

class BGR_12Process : public GenericProcessor<Pixel_BGR12>
{

};
class BGR_24Process : public GenericProcessor<Pixel_BGR24>
{

};

};
#endif // BMP_H
