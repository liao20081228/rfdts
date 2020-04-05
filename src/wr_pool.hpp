#pragma once
#ifndef HPP_RFTS_WORK_REQUEST_POOL_HPP
#define HPP_RFTS_WORK_REQUEST_POOL_HPP

#include"sem.hpp"

#define RFTS_SERVER
#ifdef RFTS_SERVER
	#define RFTS_MPSC
#endif

namespace rfts
{

template<typename T>
class wr_pool
{
private:
	const uint64_t		__elesize;	//个队列元素的大小
	const uint32_t		__elenum;	//元素个数
	const uint32_t		__rqnum;	//队列元素个数
	const uint64_t		__length;	//buf长度
	uint8_t*		__addr;		//buf地址
	uint32_t		__front;	//队首标记
#ifdef RFTS_MPSC
	std::atomic_uint32_t	__rear;
#else
	uint32_t		__rear;		//队尾标记
#endif /* #ifdef RFTS_MPSC */

	T*			__wrs;		//WR队列
	ibv_sge*		__sg_lists;	//SGE队列
	uint64_t		__wr_id;	//WR ID
	T**			__rq;		//存放WR地址的循环队列
#ifdef RFTS_MPSC
	std::atomic<bool>*	__rq_flags;	//循环队列标志
#endif /* #ifdef RFTS_MPSC */

public:
	explicit wr_pool(const trans_args& tsas) noexcept;
	
	wr_pool(const wr_pool & ref)= delete;
	wr_pool(wr_pool&& ref) noexcept;
	wr_pool& operator = (wr_pool& ref) = delete;
	wr_pool& operator = (wr_pool&& ref) = delete;

	~wr_pool(void) noexcept;

	
	T*		get(void) noexcept;
	void		put(T* e) noexcept;
	void*		get_mrs_addr(void) const noexcept;
	uint64_t	get_mrs_length(void) const noexcept;
	void*		get_wrs_addr(void) const noexcept;
	uint64_t	get_wrs_length(void) const noexcept;
	void*		get_sgs_addr(void) const noexcept;
	uint64_t	get_sgs_length(void) const noexcept;
	void*		get_flags_addr(void) const noexcept;
	uint64_t	get_flags_length(void) const noexcept;
	void*		get_rq_addr(void) const noexcept;
	uint64_t	get_rq_length(void) const noexcept;
};

}// rfts

template<typename T>
rfts::wr_pool<T>::wr_pool(const trans_args& tsas) noexcept
	: __elesize(tsas.get_elesize())
	, __elenum(tsas.get_elenum())
	, __rqnum(__elenum + 1)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new T[__elenum]())
	, __sg_lists(new ibv_sge[__elenum]())
	, __wr_id(0)
	, __rq(new T*[__rqnum]())
#ifdef RFTS_MPSC
	, __rq_flags(new std::atomic<bool>[__rqnum]())
#endif /* #ifdef RFTS_MPSC */
{
	if(!__elenum || !__elesize)
	{
		errno = EINVAL;
		PEIE(rfts::wr_pool::wr_pool);
	}
	for(uint32_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = static_cast<uint64_t>((__addr + i * __elesize));
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		
		__rq[__rear++] = &__wrs[i];
		if (__rear >= __rqnum)
			__rear -= __rqnum;
#ifdef RFTS_MPSC
		__rq_flags[i].store(true, std::memory_order_relaxed);
#endif /* #ifdef RFTS_MPSC */
	}
	__rq[__rear] = nullptr;
#ifdef RFTS_MPSC
	__rq_flags[__rear].store(false, std::memory_order_relaxed);
#endif /* #ifdef RFTS_MPSC */
}

template<typename T>
rfts::wr_pool<T>::wr_pool(wr_pool&& ref) noexcept
	: __elesize(ref.__elesize)
	, __elenum(ref.__elenum)
	, __rqnum(ref.__rqnum)
	, __length(ref.__length)
	, __addr(ref.__addr)
	, __front(ref.__front)
#ifdef RFTS_MPSC
	, __rear(ref.__rear.load(std::memory_order_relaxed))
#else
	, __rear(ref.__rear)
#endif /* #ifdef RFTS_MPSC */
	, __wrs(ref.__wrs)
	, __sg_lists(ref.__sg_lists)
	, __wr_id(ref.__wr_id)
	, __rq(ref.__rq)
#ifdef RFTS_MPSC
	, __rq_flags(ref.__rq_flags)
#endif /* #ifdef RFTS_MPSC */
{
	ref.__addr = nullptr;
	ref.__wrs  = nullptr;
	ref.__sg_lists = nullptr;
	ref.__rq = nullptr;
#ifdef RFTS_MPSC
	ref.__rq_flags = nullptr;
#endif /* #ifdef RFTS_MPSC */
}


template<typename T>
rfts::wr_pool<T>::~wr_pool(void) noexcept
{
	if (!__addr)
		return;
	int32_t num = 0;
	while(true)
	{
		num = __rear - __front;
		if (num < 0)
			num += __rqnum;
		if (num == __elenum)
			break;
	}
#ifdef RFTS_MPSC
	delete [] __rq_flags;
	__rq_flags = nullptr;
#endif /* #ifdef RFTS_MPSC */
	delete [] __rq;
	__rq = nullptr;
	delete [] __sg_lists;
	__sg_lists = nullptr;
	delete [] __wrs;
	__wrs = nullptr;
	delete [] __addr;
	__addr = nullptr;
}


template<typename T>
inline void* rfts::wr_pool<T>::get_mrs_addr(void) const noexcept
{
	return __addr;
}


template<typename T>
inline uint64_t rfts::wr_pool<T>::get_mrs_length(void) const noexcept
{
	return __length;
}


template<typename T>
inline void* rfts::wr_pool<T>::get_wrs_addr(void) const noexcept
{
	return __wrs;
}


template<typename T>
inline uint64_t rfts::wr_pool<T>::get_wrs_length(void) const noexcept
{
	return sizeof(T) * __elenum;
}

template<typename T>
inline void* rfts::wr_pool<T>::get_sgs_addr(void) const noexcept
{
	return __sg_lists;
}


template<typename T>
inline uint64_t rfts::wr_pool<T>::get_sgs_length(void) const noexcept
{
	return sizeof(ibv_sge) * __elenum;
}

#ifdef RFTS_MPSC
template<typename T>
inline void* rfts::wr_pool<T>::get_flags_addr(void) const noexcept
{
	return __rq_flags;
}


template<typename T>
inline uint64_t rfts::wr_pool<T>::get_flags_length(void) const noexcept
{
	return sizeof(std::atomic<bool>) * __rqnum;
}
#endif

template<typename T>
inline void* rfts::wr_pool<T>::get_rq_addr(void) const noexcept
{
	return __rq;
}


template<typename T>
inline uint64_t rfts::wr_pool<T>::get_rq_length(void) const noexcept
{
	return sizeof(T*) * __rqnum;
}

template<typename T>
T* rfts::wr_pool<T>::get(void) noexcept
{
	T* temp = __rq[__front];
	__rq[__front] = nullptr;
#ifdef RFTS_MPSC
	__rq_flags[__front].store(false, std::memory_order_relaxed);
#endif /* #ifdef RFTS_MPSC */
	if (++__front  >= __rqnum)
		__front -= __rqnum;
	temp->wr_id = ++__wr_id;
	return temp;
}


template<typename T>
void rfts::wr_pool<T>::put(T* e) noexcept
{
#ifdef RFTS_MPSC
	bool flag = false;
	while(!__rq_flags[__rear.load(std::memory_order_acquire)].compare_exchange_weak(
		flag, true, std::memory_order_acq_rel, std::memory_order_acquire))
		flag = false;
	uint64_t rear = __rear.load(std::memory_order_relaxed);
	__rq[rear++] = e;
	if (rear >= __rqnum)
		rear -=  __rqnum;
	__rear.store(rear, std::memory_order_release);
#else
	__rq[__rear++] = e;
	if (__rear >= __rqnum)
		__rear -=  __rqnum;
#endif
}


#endif /* end of include guard: HPP_RFTS_WORK_REQUEST_POOL_HPP */
