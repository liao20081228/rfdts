#pragma once
#ifndef HPP_RFTS_SEMAPHORE_HPP
	#define HPP_RFTS_SEMAPHORE_HPP

#include"./trans_args.hpp"
namespace rfts{
class posix_sem
{
private:
	sem_t*			__sem;
	const std::string	__name;
public:
	explicit posix_sem(int pshared = 0,  unsigned int value = 0) noexcept;
	explicit posix_sem(const char* name, int oflag, mode_t mode,
			unsigned int value = 0) noexcept;
	explicit posix_sem(const std::string& name, int oflag, mode_t mode,
			unsigned int value = 0) noexcept;
	explicit posix_sem(const std::string* name, int oflag, mode_t mode,
			unsigned int value = 0) noexcept;
	posix_sem(const posix_sem& ref) = delete;
	posix_sem(posix_sem&& ref) noexcept;
	
	~posix_sem(void) noexcept;

	posix_sem& operator = (posix_sem& ref) = delete;
	posix_sem& operator = (posix_sem&& ref) = delete;
	
	void post(void) const noexcept ;
	void wait(void) const noexcept;
	int  trywait(void) const noexcept;
	int  timewait(const struct timespec* abs_timeout) const noexcept;
	uint32_t  getvalue(void) const noexcept;
};

}


rfts::posix_sem::posix_sem(int pshared, unsigned int value) noexcept
	: __sem(new sem_t)
	, __name("")
{
	if (sem_init(__sem, pshared, value))
	{
		delete __sem;
		PEIE(posix_sem::sem_init);
	}
}

rfts::posix_sem::posix_sem(const char* name, int oflag, mode_t mode,
			unsigned int value) noexcept
	: __sem(sem_open(name, oflag, mode, value))
	, __name(name)
{
	if (__sem == SEM_FAILED)
		PEIE(posix_sem::sem_open);
}

rfts::posix_sem::posix_sem(const std::string& name, int oflag, mode_t mode,
			unsigned int value) noexcept
	: __sem(sem_open(name.c_str(), oflag, mode, value))
	, __name(name)
{
	if (__sem == SEM_FAILED)
		PEIE(posix_sem::sem_open);
}

rfts::posix_sem::posix_sem(const std::string* name, int oflag, mode_t mode, 
		unsigned int value) noexcept
	: __sem(sem_open(name->c_str(), oflag, mode, value))
	, __name(*name)
{
	if (__sem == SEM_FAILED)
		PEIE(posix_sem::sem_open);
}

rfts::posix_sem::posix_sem(posix_sem&& ref) noexcept
	:__sem(ref.__sem)
	, __name(ref.__name)
{
	ref.__sem = nullptr;
}

rfts::posix_sem::~posix_sem(void) noexcept
{
	if (__sem)
	{
		if (__name.size())
		{
			sem_close(__sem);
			sem_unlink(__name.c_str());
			return;
		}
		delete __sem;
		sem_destroy(__sem);
	}
}


void rfts::posix_sem::post(void) const noexcept
{
	if (sem_post(__sem))
		PEIE(posix_sem::sem_post);
}

void rfts::posix_sem::wait(void) const noexcept
{
	if (sem_wait(__sem))
		PEIE(posix_sem::sem_wait);
}


int rfts::posix_sem::trywait(void) const noexcept
{
	if (sem_trywait(__sem))
	{
		if (errno != EAGAIN)
		{
			PEIE(posix_sem::sem_trywait);
		}
		else
			return EAGAIN;
	}
	return 0;
}

int rfts::posix_sem::timewait(const struct timespec* abs_timeout) const noexcept
{
	if (sem_timedwait(__sem,abs_timeout))
	{
		if (errno != ETIMEDOUT)
		{
			PEIE(posix_sem::sem_trywait);
		}
		else
			return ETIMEDOUT;
	}
	return 0;
}

inline unsigned int rfts::posix_sem::getvalue(void) const noexcept
{
	int n  = 0;
	sem_getvalue(__sem, &n);
	return n;
}

#endif /* end of include guard: HPP_RFTS_SEMAPHORE_HPP */
