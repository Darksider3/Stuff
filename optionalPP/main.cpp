#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>

struct OptionFlag
{
  std::string FlagName;
  std::string LongName;
  bool standard;
  std::string description;
};

class optionalPP
{
public:
  std::vector<OptionFlag> m_options;
  char **m_argv;
  int m_len;

  optionalPP(char **argv, int len) : m_argv{argv+1}, m_len{len-1}
  {}

  void add_flag(std::string name, bool standard, std::string desc)
  {
    OptionFlag tmp;
    tmp.FlagName = name;
    tmp.standard = standard;
    tmp.description = desc;
    m_options.emplace_back(tmp);
  }
  void add_flag(std::string name, std::string Longname, bool standard, std::string desc)
  {
    OptionFlag tmp;
    tmp.FlagName = name;
    tmp.LongName = Longname;
    tmp.standard = standard;
    tmp.description = desc;
    m_options.emplace_back(tmp);
  }
  // processes argv
  bool process()
  {
    sort();
    for(int i = 0; i != m_len; ++i)
    {
      for(size_t j = 0; j != m_options.size(); ++j)
      {
        if(m_options[j].FlagName.compare(m_argv[i]) == 0)
        {
          std::cout << "HEEERE \n";
          set_opposite(j);
        }
      }
    }
    return true;
  }
  /*bool flag_is_on(std::string flagname)
  {

    std::cout << std::find(m_options.begin(), m_options.end(), flagname, cmprStr) << " <----- \n";
    return true;
  }*/

  void set_opposite(size_t at)
  {
    m_options.at(at).standard = !m_options.at(at).standard;
  }

  void sort()
  {
    std::sort(m_options.begin(), m_options.end(), cmpr);
  }
  static bool cmpr(OptionFlag f, OptionFlag w) // this one get's actually used by std::sort
  {
    return (f.FlagName.compare(w.FlagName) < 0);
  }
  virtual ~optionalPP()
  {

  }
};

std::stringstream print_intake(int argc, char **argv)
{
  std::stringstream ss;
  for(int i = argc; argc > 0; --argc)
  {
    ss << argv[i];
  }
  return ss;
}
int main(int argc, char **argv)
{
  optionalPP avb(argv, argc);
  avb.add_flag("-h", "--help",true, "Say hello");
  avb.process();
  std::cout.flush();
}
