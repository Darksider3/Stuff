#include "xdg_basedirs.h"
#include <iostream>
#include "Test.h"

xdg_basedirs *XDG = xdg_basedirs::instance();

/**
 * @brief xdg_basedirs::Home()
 */
bool test_home(Li::TestCase *mua)
{
  bool returner = true;
  mua->name = "$HOME Dir";
  mua->descr = "Test wether $HOME is set correctly or not. Assuming that the user resides in /home!";
  if(!XDG->Home().starts_with("/home/"))
  {
    mua->error = 1;
    returner = false;
    mua->errorDesc = "Either there is no $HOME set, or it isn't /home!";
  }
  return returner;
}

/**
 * @brief xdg_basedirs::CacheHome()
 */
bool test_cache_home(Li::TestCase *testr)
{
  bool returner = true;
  testr->name = "$XDG_CACHE_HOME";
  testr->descr = "Test weither $XDG_CACHE_HOME is set(and in the users directory) or not";

  if(!XDG->CacheHome().starts_with(XDG->Home()))
  {
    testr->error = 1;
    testr->errorDesc = "Value of $XDG_CACHE_HOME: '" + XDG->CacheHome() + "'";
    returner = false;
  }

  return returner;
}

/**
 * @brief xdg_basedirs::DataHome()
 */
bool test_data_home(Li::TestCase *testr)
{
  bool returner = true;
  testr->name = "$XDG_DATA_HOME";
  testr->descr = "Test weither $XDG_DATA_HOME is set(and in the users directory) or not";

  if(!XDG->DataHome().starts_with(XDG->Home()))
  {
    testr->error = 1;
    testr->errorDesc = "Value of $XDG_DATA_HOME: '" + XDG->DataHome() + "'";
    returner = false;
  }

  return returner;
}

int main()
{
  xdg_basedirs *Ins = xdg_basedirs::instance();
  auto Home = Ins->Home();
  auto CacheHome = Ins->CacheHome();
  auto DataHome = Ins->DataHome();

  Li::Test *tester = Li::Test::instance();
  auto TestHome = std::make_shared<Li::TestCase>();
  auto TestCacheHome = std::make_shared<Li::TestCase>();
  auto TestDataHome = std::make_shared<Li::TestCase>();
  TestDataHome->func = *test_data_home;
  TestHome->func = *test_home;
  TestCacheHome->func = *test_cache_home;
  tester->append(TestHome.get());
  tester->append(TestCacheHome.get());
  tester->append(TestDataHome.get());
  tester->exec();
  std::cout << tester->errors();


  std::cout << "Home: " << Home << std::endl;
  std::cout << "CacheHome: " << CacheHome << std::endl;
  std::cout << "DataHome: " << DataHome << std::endl;

  return 1;
}
