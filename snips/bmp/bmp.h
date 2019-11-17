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
struct BGR_8
{
  uint8_t B;
  uint8_t G;
  uint8_t R;
};

struct BGRA_8 : public BGR_8
{
  uint8_t A;
};

struct BGR_4
{
  unsigned B: 4;
  unsigned G: 4;
  unsigned R: 4;
};

struct Black_White
{
  bool W;
  bool B;
};

class Impl
{
protected:
  uint8_t paddingBytes; // Each "row"(x-coordinate) must be a multiple of 4, thus max 3 bytes padding
  std::ifstream f;
  std::vector<BGR_8> DATA;
  inline bool exist(std::string &FN){return std::filesystem::exists(FN);}
public:
  BitmapFileHeader header;
  Impl(std::string&);
  void readHeader();
  bool check_header();
  void readData();

  std::shared_ptr<uint8_t[]> getDataSmartUint8RGBA();
  uint8_t *legacyUint8RGBA();
  virtual ~Impl();
};
}
#endif // BMP_H
