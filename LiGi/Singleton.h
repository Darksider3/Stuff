#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
namespace Li {

template<typename T>
class Singleton {
public:
	static T* instance()
	{
		if (!m_instance)
			m_instance = std::make_shared<T>();
		return m_instance.get();
	}

	virtual ~Singleton()
	{
		m_instance = nullptr;
	}

private:
	static std::shared_ptr<T> m_instance;

protected:
	// CRTP
	friend T;
	Singleton() noexcept { }
	Singleton(const Singleton&) noexcept { }
	Singleton(Singleton&&) noexcept { }
	Singleton& operator=(Singleton const&) noexcept;
	Singleton& operator=(Singleton) noexcept;
	Singleton& operator=(Singleton&&) noexcept;
};
template<typename T>
std::shared_ptr<T> Singleton<T>::m_instance = nullptr;
}
#endif // SINGLETON_H
