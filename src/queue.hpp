#pragma once
#ifndef HPP_RFTS_QUEUE_HPP
	#define HPP_RFTS_QUEUE_HPP

#include"./sem.hpp"

#ifdef RFTS_SERVER
	#define RFTS_SPMC
#endif /* #ifdef RFTS_SERER */

namespace rfts
{

template<typename T>//T是指针
class queue
{
public:
	const uint32_t		__rqnum;	//队列容量
private:
	T*			__rq;		//队列地址
#ifdef RFTS_SPMC
	std::atomic_uint32_t	__front;
#else
	uint32_t		__front;
#endif
	uint32_t		__rear;
#ifdef RFTS_SPMC
	std::atomic<bool>*	__rq_flags;	//原子标记
#endif
	rfts::posix_sem		__count;
public:
	explicit queue(const trans_args& tsas) noexcept;
	~queue(void) noexcept;

	queue(const queue&) = delete;
	queue(queue&&) noexcept;
	queue& operator=(const queue&) = delete;
	queue& operator=(queue&&) = delete;

	void put(T e) noexcept;
	T get(void) noexcept;
	uint32_t get_rq_elenum(void) const noexcept;
	void* get_rq_addr(void) const noexcept;
	uint64_t get_rq_length(void) const noexcept;

#ifdef RFTS_SPMC
	void* get_flags_addr(void) const noexcept;
	uint64_t get_flags_length(void) const noexcept;
#endif /* #ifdef RFTS_SPMC */
};

template<typename T>
struct queues
{
	queue<T>	nosend_or_recving;
	queue<T>	sending_or_nohand;
	queue<ibv_wc*>	sended_or_recved;
	queues(trans_args& tsas)
		: nosend_or_recving(queue<T>(tsas))
		, sending_or_nohand(queue<T>(tsas))
		, sended_or_recved(queue<ibv_wc*>(tsas))
	{
	}
};
}



template<typename T>
rfts::queue<T>::queue(const trans_args& tsas) noexcept
	: __rqnum(ceil(tsas.tfreq * tsas.sys_lat / 1000000 * tsas.node_num *
			tsas.ele_num_scale))
	, __rq(new T[__rqnum]())
	, __front(0)
	, __rear(0)
#ifdef RFTS_SPMC
	, __rq_flags(new std::atomic<bool>[__rqnum]())
#endif /* #ifdef RFTS_SPMC */
	, __count(0, 0)
{
	if (!__rqnum)
	{
		errno = EINVAL;
		PEIE(rfts::queue::queue);
	}
	for (uint32_t i = 0; i < __rqnum; ++i)
	{
		__rq[i] = nullptr;
#ifdef RFTS_SPMC
		__rq_flags[i].store(false, std::memory_order_relaxed);
#endif /* #ifdef RFTS_SPMC */
	}
}

template<typename T>
rfts::queue<T>::queue(queue&& ref) noexcept
	: __rqnum(ref.__rqnum)
	, __rq(ref.__rq)
	, __front(ref.__front)
	, __rear(ref.__rear)
#ifdef RFTS_SPMC
	, __rq_flags(ref.__rq_flags)
#endif /* #ifdef RFTS_SPMC */
	, __count(0, ref.__count.getvalue())
{
	ref.__rq = nullptr;
#ifdef RFTS_SPMC
	ref.__rq_flags = nullptr;
#endif /* #ifdef RFTS_SPMC */
}

template<typename T>
rfts::queue<T>::~queue(void) noexcept
{
	if (!__rq)
		return;
	while(__count.getvalue() != 0){};
	delete [] __rq;
	__rq = nullptr;
#ifdef RFTS_SPMC
	delete [] __rq_flags;
	__rq_flags = nullptr;
#endif /* #ifdef RFTS_SPMC */
}


template<typename T>
void rfts::queue<T>::put(T e) noexcept
{
	__rq[__rear] = e;
#ifdef RFTS_SPMC
	__rq_flags[__rear].store(true, std::memory_order_release);
#endif /* #ifdef RFTS_SPMC */
	if (++__rear >= __rqnum)
		__rear-=__rqnum;
	__count.post();
}

template<typename T>
T rfts::queue<T>::get(void) noexcept
{
	__count.wait();
#ifdef RFTS_SPMC
	bool  flag = true;
	while(!__rq_flags[__front.load(std::memory_order_acquire)].compare_exchange_weak(
		flag, false, std::memory_order_acq_rel, std::memory_order_acquire))
		flag = true;
	uint32_t front = __front.load(std::memory_order_relaxed);
	T temp = __rq[front];
	__rq[front++] = nullptr;
	if ( front >= __rqnum)
		front -=  __rqnum;
	__front.store(front, std::memory_order_release);
#else
	T temp = __rq[__front];
	__rq[__front++] = nullptr;
	if (__front >= __rqnum)
		__front-=__rqnum;
#endif
	return temp;
}


template<typename T>
void* rfts::queue<T>::get_rq_addr(void) const noexcept
{
	return __rq;
}

template<typename T>
uint64_t rfts::queue<T>::get_rq_length(void) const noexcept
{
	return sizeof(T) * __rqnum;
}

template<typename T>
uint32_t rfts::queue<T>::get_rq_elenum(void) const noexcept
{
	return __count.getvalue();
}

#ifdef RFTS_SPMC
template<typename T>
void* rfts::queue<T>::get_flags_addr(void) const noexcept
{
	return __rq_flags;
}

template<typename T>
uint64_t rfts::queue<T>::get_flags_length(void) const noexcept
{
	return sizeof(std::atomic<bool>) * __rqnum;
}

#endif /* #ifdef RFTS_SPMC */


#endif /* end of include guard: HPP_RFTS_QUEUE_HPP */
