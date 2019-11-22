#include "bmp.h"
namespace bmp
{
Headers::Headers(std::string &FN)
{
  f = std::ifstream(FN);
  Read();
  f.close();
}

Headers::~Headers()
{
  if(f.is_open())
  {
    f.close();
  }
}

COLOR_TABLE_ENTRY_BGRA Headers::operator[](size_t b)
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

void Headers::Read()
{
  f.read(reinterpret_cast<char*>(&bmp_header), sizeof(BitmapFileHeader));
  if(bmp_header.dib_header_size != 40)
  {
    std::cout << "currently just supporting 40-byte-header-bmps...\n";
    abort();
  }
  f.read(reinterpret_cast<char*>(&dib_header), sizeof(DIB_BITMAPINFOHEADER));

  if(dib_header.num_colors == 0)
  {
    return;
  }
  // 14 + 40 => 54, 256*4 => 1024, +=> 1078
  size_t correctOffset = BMP_HEADER_SIZE+bmp_header.dib_header_size+(dib_header.num_colors*4);
  if(correctOffset != bmp_header.Offset)
  {
    std::cout << "Offset missmatch: Header said " << bmp_header.Offset << ", but we calculated " << correctOffset << "\n";
    abort();
  }

  colortable = std::make_unique<COLOR_TABLE_ENTRY_BGRA[]>(dib_header.num_colors);
  readColorTable();
  return;
}

void Headers::readColorTable()
{
  COLOR_TABLE_ENTRY_BGRA tmp;
  for(size_t i = 0; i < dib_header.num_colors; ++i){
    f.read(reinterpret_cast<char*>(&tmp), sizeof(COLOR_TABLE_ENTRY_BGRA));
    copy(tmp, colortable[i]);
  }
}

bool Headers::ColorTable()
{
  if(colortable == nullptr)
    return false;
  else
    return true;
}

void Headers::writeColortable()
{
  if(colortable == nullptr)
    return;
  std::fstream writer("./test.dump");
  for(size_t i = 0; i < dib_header.num_colors; ++i)
  {
  }

}
};
