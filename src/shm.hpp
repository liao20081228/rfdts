#pragma once
#ifndef HPP_RFTS_SHARED_MEMORY_HPP
	#define HPP_RFTS_SHARED_MEMORY_HPP
#include"./trans_args.hpp"

//posix shared memory
namespace rfts
{
class posix_shm
{
private:
	const std::string	__name;		//shm对应文件名
	const int		__fd;		//shm对应的fd
	void*			__addr;		//shm起始地址
	size_t			__length;	//shm长度
	std::atomic_uint64_t	__cur;		//shm当前读写位置
	int			__protect;	//shm保护模式
private:
	void __access(void* buf, size_t buf_len, size_t nbytes,
			bool reset, bool is_read) noexcept;
public:
	explicit posix_shm(const char* name, size_t size = sysconf(_SC_PAGESIZE),
			int oflag = O_RDWR | O_CREAT, mode_t mode = REGULAR_FILE_MODE,
			int prot = PROT_READ | PROT_WRITE,
			int flags = MAP_SHARED, off_t offset = 0) noexcept;
	explicit posix_shm(const std::string& name, size_t size = sysconf(_SC_PAGESIZE),
			int oflag = O_RDWR | O_CREAT, mode_t mode = REGULAR_FILE_MODE,
			int prot = PROT_READ | PROT_WRITE,
			int flags = MAP_SHARED, off_t offset = 0) noexcept;
	explicit posix_shm(const std::string* name, size_t size = sysconf(_SC_PAGESIZE),
			int oflag = O_RDWR | O_CREAT, mode_t mode = REGULAR_FILE_MODE,
			int prot = PROT_READ | PROT_WRITE,
			int flags = MAP_SHARED, off_t offset = 0) noexcept;
	posix_shm(const posix_shm& ref) = delete;
	posix_shm(posix_shm&& ref) noexcept;

	~posix_shm(void) noexcept;

	posix_shm& operator = (const posix_shm& ref) = delete;
	posix_shm& operator = (const posix_shm&& ref) = delete;
	
	void*	getaddr(void) const noexcept;
	size_t	getlength(void) const noexcept;
	void	clear(void) noexcept;
	int	sync(int flags = MS_SYNC) const noexcept;
	size_t	seek(off_t offset = 0, int whence = SEEK_SET) noexcept;
	size_t	tell(void) const noexcept;
	void	read(void* buf, size_t buf_len, size_t nbytes, bool reset = true) noexcept;
	void	write(void* buf, size_t buf_len, size_t nbytes, bool reset = true) noexcept;
};
}



rfts::posix_shm::posix_shm(const char* name, size_t size, int oflag, mode_t mode,
		int prot, int flags, off_t offset) noexcept
	: __name(name)
	, __fd(shm_open(__name.c_str(), oflag, mode))
	, __length(size)
	, __cur(0)
	, __protect(prot)
{
	if (__fd < 0)
		PEIE(posix_shm::shm_open);
	if (ftruncate(__fd, size))
	{
		close(__fd);
		PEIE(posix_shm::ftruncate);
	}
	__addr = mmap(nullptr, size, prot, flags, __fd, offset);
	if (__addr == MAP_FAILED)
	{
		close(__fd);
		PEIE(posix_shm::mmap);
	}
}

rfts::posix_shm::posix_shm(const std::string& name, size_t size, int oflag, mode_t mode
		, int prot, int flags, off_t offset) noexcept
	: __name(name)
	, __fd(shm_open(__name.c_str(), oflag, mode))
	, __length(size)
	, __cur(0)
	, __protect(prot)
{
	if (__fd < 0)
		PEIE(posix_shm::shm_open);
	if (ftruncate(__fd, size))
	{
		close(__fd);
		PEIE(posix_shm::ftruncate);
	}
	__addr = mmap(nullptr, size, prot, flags, __fd, offset);
	if (__addr == MAP_FAILED)
	{
		close(__fd);
		PEIE(posix_shm::mmap);
	}
}

rfts::posix_shm::posix_shm(const std::string* name, size_t size, int oflag, mode_t mode,
		int prot, int flags, off_t offset) noexcept
	: __name(*name)
	, __fd(shm_open(__name.c_str(), oflag, mode))
	, __length(size)
	, __cur(0)
	, __protect(prot)
{
	if (__fd < 0)
		PEIE(posix_shm::shm_open);
	if (ftruncate(__fd, size))
	{
		close(__fd);
		PEIE(posix_shm::ftruncate);
	}
	__addr = mmap(nullptr, size, prot, flags, __fd, offset);
	if (__addr == MAP_FAILED)
	{
		close(__fd);
		PEIE(posix_shm::mmap);
	}
}

rfts::posix_shm::posix_shm(posix_shm&& ref) noexcept
	: __name(ref.__name)
	, __fd(ref.__fd)
	, __addr(ref.__addr)
	, __length(ref.__length)
	, __cur(ref.__cur.load(std::memory_order_acquire))
	, __protect(ref.__protect)
{
	ref.__addr = nullptr;
}


rfts::posix_shm::~posix_shm(void) noexcept
{
	if (!__addr)
		return;
	if(munmap(__addr, __length))
		PEIE(posix_shm::munmap);
	close(__fd);
	shm_unlink(__name.c_str());
}


void* rfts::posix_shm::getaddr(void) const noexcept
{
	return __addr;
}

size_t rfts::posix_shm::getlength(void) const noexcept
{
	return  __length;
}
void rfts::posix_shm::clear(void) noexcept
{
	memset(__addr, 0, __length);
	__cur.store(0, std::memory_order_release);
}


int rfts::posix_shm::sync(int flags) const noexcept
{
	int ret = msync(__addr, __length, flags);
	if (ret && errno == EINVAL)
		PEIE(posix_shm::sync);
	return ret;
}

size_t rfts::posix_shm::seek(off_t offset, int whence) noexcept
{
	uint64_t temp = 0;
	switch(whence)
	{
		case SEEK_SET:
			if (offset < 0 || offset >= static_cast<off_t>(__length))
			{
				errno = EINVAL;
				PEIE(posix_shm::seek);
			}
			__cur.store(offset, std::memory_order_release);
			return __cur.load(std::memory_order_acquire);
		case SEEK_CUR:
			temp = __cur.load(std::memory_order_acquire);
			do
			{
				if((static_cast<off_t>(temp) + offset)
					>= static_cast<off_t>(__length)
					|| (temp + offset) < 0)
				{
					errno = EINVAL;
					PEIE(posix_shm::mseek);
				}
			}while (!__cur.compare_exchange_weak(temp, temp + offset,
							std::memory_order_acq_rel,
							std::memory_order_acquire));
			__cur.store(__length - 1  + offset, std::memory_order_release);
			return __cur.load(std::memory_order_acquire);
		case SEEK_END:
			if (offset > 0 || offset <= static_cast<off_t>(-__length))
			{
				errno = EINVAL;
				PEIE(posix_shm::seek);
			}
			__cur.store(__length - 1  + offset, std::memory_order_release);
			return __cur.load(std::memory_order_acquire);
		default:
			errno = EINVAL;
			PEIE(posix_shm::seek);
			break;
	}
}

size_t rfts::posix_shm::tell(void) const noexcept
{
	return __cur.load(std::memory_order_acquire);
}

void rfts::posix_shm::__access(void* buf, size_t buf_len, size_t nbytes, bool reset,
		bool is_read) noexcept
{
	if ((is_read && !(__protect & PROT_READ)) || (!is_read && !(__protect & PROT_WRITE)))
	{
		errno = EPERM;
		if (is_read)
			PEIE(posix_shm::read);
		PEIE(posix_shm::write);
	}

	if (!buf || buf_len < nbytes)
	{
		errno =  EINVAL;
		if (is_read)
			PEIE(posix_shm::read);
		PEIE(posix_shm::write);
	}
	if (!nbytes)
		return;
	if (is_read)
		memset(buf, 0, buf_len);
	if (reset)
		seek(0, SEEK_SET);
	uint64_t temp = __cur.load(std::memory_order_acquire);
	do
	{
		if (temp + nbytes > __length)
		{
			errno =  EINVAL;
			if (is_read)
				PEIE(posix_shm::read);
			PEIE(posix_shm::write);
		}
		if (is_read)
			memcpy(buf, static_cast<unsigned char*>(__addr) + __cur, nbytes);
		else
			memcpy(static_cast<unsigned char*>(__addr) + temp, buf, nbytes);
	}while (!__cur.compare_exchange_weak(temp, temp +nbytes, std::memory_order_acq_rel,
			std::memory_order_acquire));
}

void rfts::posix_shm::read(void* buf, size_t buf_len, size_t nbytes, bool reset) noexcept
{
	__access(buf, buf_len, nbytes, reset, true);
}

void rfts::posix_shm::write(void* buf, size_t buf_len, size_t nbytes, bool reset) noexcept
{
	__access(buf, buf_len, nbytes, reset, false);
}

#endif /* end of include guard: HPP_RFTS_SHARED_MEMORY_HPP */
