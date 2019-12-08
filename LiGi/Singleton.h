#ifndef SINGLETON_H
#define SINGLETON_H
namespace Li
{

template <typename T>
class Singleton
{
public:
  static T* instance()
  {
    if(!m_instance)
      m_instance = new T();
    return m_instance;
  }

  virtual ~Singleton()
  {
    m_instance = nullptr;
  }
private:
  static T* m_instance;

protected:
  Singleton(){}
};
template <typename T> T*Singleton<T>::m_instance = nullptr;
}
#endif // SINGLETON_H
