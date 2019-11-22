#ifndef BMP_H
#define BMP_H
#define DEBUG 1
#include <fstream>
#include <filesystem>
#include <iostream>
#include <byteswap.h>
#include <vector>

struct X {
  ~X() { std::cout << std::endl; }
};
#ifdef DEBUG
#define DBG (X(), std::cout << __FILE__ << ":"  << __LINE__ << ": " <<  __PRETTY_FUNCTION__ << ": ")
#endif

#define BMP_HEADER_SIZE 14 // bmps magic and standard header(before DIB) is 14 bytes


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

struct Pixel {};

struct COLOR_TABLE_ENTRY : Pixel {};

struct Pixel_BGRA : Pixel {
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t A;
};

struct Pixel_BGR24 : Pixel {
  uint8_t B;
  uint8_t G;
  uint8_t R;
};

struct COLOR_TABLE_ENTRY_BGRA : public COLOR_TABLE_ENTRY, public Pixel_BGRA {};

class Headers
{
public:
  std::unique_ptr<COLOR_TABLE_ENTRY_BGRA[]> colortable = nullptr;
  std::ifstream f;
  BitmapFileHeader bmp_header;
  DIB_BITMAPINFOHEADER dib_header;

  explicit Headers(std::string &FN);

  void Read();
  void readColorTable();
  bool ColorTable();
  COLOR_TABLE_ENTRY_BGRA operator[](size_t b);
  void writeColortable();

  ~Headers()
  {
    if(f.is_open())
    {
      f.close();
    }
  }

  void copy(COLOR_TABLE_ENTRY_BGRA from, COLOR_TABLE_ENTRY_BGRA &to)
  {
    to.B = from.B;
    to.G = from.G;
    to.R = from.R;
  }

  uint16_t BitPerPixel()
  {
    return dib_header.bits_per_pixel;
  }

  uint32_t NumColors()
  {
    return dib_header.num_colors;
  }

  uint32_t Offset()
  {
    return bmp_header.Offset;
  }
};


template<typename T = Pixel>
class GenericProcessor {
public:

};

class BW_Process : public GenericProcessor<>
{
};
class BGR_12Process : public GenericProcessor<>
{

};
class BGR_24Process : public GenericProcessor<>
{

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
};
#endif // BMP_H
