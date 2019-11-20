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
  std::ifstream *f = nullptr;
  BitmapFileHeader bmp_header;
  DIB_BITMAPINFOHEADER dib_header;

  explicit Headers(std::string &FN)
  {
    f = new std::ifstream(FN);
    read();
  }

  void read()
  {
    f->read(reinterpret_cast<char*>(&bmp_header), sizeof(BitmapFileHeader));
    if(bmp_header.dib_header_size != 40)
    {
      std::cout << "currently just supporting 40-byte-header-bmps...\n";
      abort();
    }
    f->read(reinterpret_cast<char*>(&dib_header), sizeof(DIB_BITMAPINFOHEADER));

    if(dib_header.num_colors == 0)
    {
      return;
    }
    // 14 + 40 => 54, 256*4 => 1024, +=> 1078
    size_t correctOffset = 14+bmp_header.dib_header_size+(dib_header.num_colors*4);
    if(correctOffset != bmp_header.Offset)
    {
      std::cout << "Offset missmatch: Header said " << bmp_header.Offset << ", but we calculated " << correctOffset << "\n";
      abort();
    }

    colortable = std::make_unique<COLOR_TABLE_ENTRY_BGRA[]>(dib_header.num_colors);
    readColorTable();
    return;
  }

  void readColorTable()
  {
    COLOR_TABLE_ENTRY_BGRA tmp;
    for(size_t i = 0; i < dib_header.num_colors; ++i){
      f->read(reinterpret_cast<char*>(&tmp), sizeof(COLOR_TABLE_ENTRY_BGRA));
      copy(tmp, colortable[i]);
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

  bool ColorTable()
  {
    if(colortable == nullptr)
      return false;
    else
      return true;
  }
  COLOR_TABLE_ENTRY_BGRA operator[](size_t b)
  {
    COLOR_TABLE_ENTRY_BGRA tmp;
    tmp.B = 0;
    tmp.G = 0;
    tmp.R = 0;

    if(b < dib_header.num_colors)
      if(colortable != nullptr)
        return colortable[b];
    return tmp;
  }
  ~Headers()
  {
    if(f->is_open())
    {
      f->close();
      delete f;
    }
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
