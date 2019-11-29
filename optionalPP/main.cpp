#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>

struct OptionFlag
{
  std::string FlagName;
  std::string LongName;
  std::string description;
  static bool cmpr(OptionFlag *f, OptionFlag *w)
  {
    return (f->FlagName.compare(w->FlagName) < 0);
  }
};

struct BoolFlag : public OptionFlag
{
  bool value;

  static bool cmpr(BoolFlag *f, BoolFlag *w)
  {
    return OptionFlag::cmpr(f, w);
  }
  void set_opposite()
  {
    value = !value;
  }
};

struct StringFlag : public OptionFlag
{
  std::string value;
  static bool cmpr(BoolFlag *f, BoolFlag *w)
  {
    return OptionFlag::cmpr(f, w);
  }
  void set_opposite()
  {
    value.clear();
  }

};

class optionalPP
{
public:
  std::vector<BoolFlag*> m_options;
  char **m_argv;
  int m_len;

  optionalPP(char **argv, int len) : m_argv{argv+1}, m_len{len-1}
  {}

  void add_flag(std::string name, bool standard, std::string desc)
  {
    BoolFlag tmp;
    tmp.FlagName = name;
    tmp.value = standard;
    tmp.description = desc;
    m_options.emplace_back(new BoolFlag(tmp));
  }
  void add_flag(std::string name, std::string Longname, bool standard, std::string desc)
  {
    BoolFlag tmp;
    tmp.FlagName = name;
    tmp.LongName = Longname;
    tmp.value = standard;
    tmp.description = desc;
    m_options.emplace_back(new BoolFlag(tmp));
  }

  // processes argv
  bool process()
  {
    sort();
    for(int i = 0; i != m_len; ++i)
    {
      for(size_t j = 0; j != m_options.size(); ++j)
      {
        if(m_options[j]->FlagName.compare(m_argv[i]) == 0)
        {
          m_options[j]->set_opposite();
        }
        else if(!m_options[j]->LongName.empty())
        {
          if(m_options[j]->LongName.compare(m_argv[i]) == 0)
            m_options[j]->set_opposite();
        }
      }
    }
    return true;
  }

  // FIXME: decltype(T)
  bool isOn(std::string name)
  {
    for(auto ele: m_options)
    {
      if(name.compare(ele->FlagName) == 0)
      {
        return ele->value;
      }
    }
    return false;
  }
  /*bool flag_is_on(std::string flagname)
  {

    std::cout << std::find(m_options.begin(), m_options.end(), flagname, cmprStr) << " <----- \n";
    return true;
  }*/

  void set_opposite(size_t at)
  {
    m_options.at(at)->value = !m_options.at(at)->value;
  }

  void sort()
  {
    std::sort(m_options.begin(), m_options.end(), m_options.at(0)->cmpr);
  }
  static bool cmpr(OptionFlag *f, OptionFlag *w) // this one get's actually used by std::sort
  {
    return (f->FlagName.compare(w->FlagName) < 0);
  }
  virtual ~optionalPP()
  {
    for(auto el: m_options)
    {
      delete el;
    }
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
  avb.add_flag("-h", "--help", false, "Say hello");
  avb.add_flag("-H", "--Hell", false, "Say hello");
  avb.process();

  if(avb.isOn("-h"))
    std::cout << "IM ON HURRAY\n";
  if(avb.isOn("-H"))
  {
    std::cout << "THE HELL MAY DOMINATE YOU \n";
  }
  std::cout.flush();
}
