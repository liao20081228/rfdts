#pragma once
#ifndef HPP_RFTS_MEM_POOL_HPP
	#define HPP_RFTS_MEM_POOL_HPP
#include"./trans_args.hpp"
#include"sem.hpp"
#include<iostream>
#include<mutex>
#include<atomic>
#include<stdexcept>
#include<cstdlib>
#include<infiniband/verbs.h>
#include<array>




namespace rfts
{

class spsc_sem_mem_pool
{
private:
	const uint64_t		__elesize;	//每个队列元素的大小
	const uint32_t		__elenum;	//元素个数
	const uint64_t		__length;	//buf长度
	unsigned char*		__addr;		//buf地址
	uint32_t		__front,
				__rear;		//队首、队尾标记
	ibv_send_wr*		__wrs;		//WR队列
	ibv_sge*		__sg_lists;	//SGE队列
	uint64_t		__wr_id;	//WR ID
	ibv_send_wr**		__ringqueue;	//存放WR地址的循环队列
	rfts::posix_sem		__count;	//剩余WR计数，并在P和C之间同步
public:
	explicit spsc_sem_mem_pool(const trans_args& transargs) noexcept;
	explicit spsc_sem_mem_pool(const trans_args* transargs) noexcept;
		 spsc_sem_mem_pool(const spsc_sem_mem_pool & ref)= delete;
		 spsc_sem_mem_pool(spsc_sem_mem_pool&& ref) noexcept;

		~spsc_sem_mem_pool(void) noexcept;

	spsc_sem_mem_pool& operator = (spsc_sem_mem_pool& ref) = delete;
	spsc_sem_mem_pool& operator = (spsc_sem_mem_pool&& ref) = delete;
	
	ibv_send_wr*	get(void) noexcept;
	ibv_send_wr*	tryget(void) noexcept;
	ibv_send_wr*	timeget(void) noexcept;
	void		put(ibv_send_wr* e) noexcept;
	void*		get_mrs_addr(void) const noexcept;
	uint64_t	get_mrs_length(void) const noexcept;
	void*		get_wrs_addr(void) const noexcept;
	uint64_t	get_wrs_length(void) const noexcept;
	void*		get_sgs_addr(void) const noexcept;
	uint64_t	get_sgs_length(void) const noexcept;
	void*		get_rq_addr(void) const noexcept;
	uint64_t	get_rq_length(void) const noexcept;
};

class spsc_atomic_mem_pool
{
private:
	const uint64_t		__elesize;	//每个队列元素的大小
	const uint32_t		__elenum;	//元素个数
	const uint32_t		__rqnum;	//队列元素个数
	const uint64_t		__length;	//buf长度
	unsigned char*		__addr;		//buf地址
	std::atomic_uint32_t	__front,
				__rear;		//队首、队尾标记
	ibv_send_wr*		__wrs;		//WR队列
	ibv_sge*		__sg_lists;	//SGE队列
	uint64_t		__wr_id;	//WR ID
	ibv_send_wr**		__ringqueue;	//存放WR地址的循环队列
public:
	explicit spsc_atomic_mem_pool(const trans_args& transargs) noexcept;
	explicit spsc_atomic_mem_pool(const trans_args* transargs) noexcept;
		 spsc_atomic_mem_pool(const spsc_atomic_mem_pool & ref)= delete;
		 spsc_atomic_mem_pool(spsc_atomic_mem_pool&& ref) noexcept;

		~spsc_atomic_mem_pool(void) noexcept;

	spsc_atomic_mem_pool& operator = (spsc_atomic_mem_pool& ref) = delete;
	spsc_atomic_mem_pool& operator = (spsc_atomic_mem_pool&& ref) = delete;
	
	ibv_send_wr*	get(void) noexcept;
	ibv_send_wr*	tryget(void) noexcept;
	ibv_send_wr*	timeget(void) noexcept;
	void		put(ibv_send_wr* e) noexcept;
	void*		get_mrs_addr(void) const noexcept;
	uint64_t	get_mrs_length(void) const noexcept;
	void*		get_wrs_addr(void) const noexcept;
	uint64_t	get_wrs_length(void) const noexcept;
	void*		get_sgs_addr(void) const noexcept;
	uint64_t	get_sgs_length(void) const noexcept;
	void*		get_rq_addr(void) const noexcept;
	uint64_t	get_rq_length(void) const noexcept;
};


class mpsc_sem_mem_pool
{
private:
	const uint64_t		__elesize;	//个队列元素的大小
	const uint32_t		__elenum;	//元素个数
	const uint64_t		__length;	//buf长度
	unsigned char*		__addr;		//buf地址
	uint32_t		__front;
	std::atomic_uint32_t	__rear;		//队首、队尾标记
	ibv_recv_wr*		__wrs;		//WR队列
	ibv_sge*		__sg_lists;	//SGE队列
	uint64_t		__wr_id;	//WR ID
	ibv_recv_wr**		__ringqueue;	//存放WR地址的循环队列
	std::atomic<bool>*	__rq_flag;	//循环队列标志
	rfts::posix_sem		__count;	//剩余WR计数，并在P和C之间同步
public:
	explicit mpsc_sem_mem_pool(const trans_args& transargs) noexcept;
	explicit mpsc_sem_mem_pool(const trans_args* transargs) noexcept;
		 mpsc_sem_mem_pool(const mpsc_sem_mem_pool & ref)= delete;
		 mpsc_sem_mem_pool(mpsc_sem_mem_pool&& ref) noexcept;

		~mpsc_sem_mem_pool(void) noexcept;

	mpsc_sem_mem_pool& operator = (mpsc_sem_mem_pool& ref) = delete;
	mpsc_sem_mem_pool& operator = (mpsc_sem_mem_pool&& ref) = delete;
	
	ibv_recv_wr*	get(void) noexcept;
	ibv_recv_wr*	tryget(void) noexcept;
	ibv_recv_wr*	timeget(void) noexcept;
	void		put(ibv_recv_wr* e) noexcept;
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

class mpsc_atomic_mem_pool
{
private:
	const uint64_t		__elesize;	//个队列元素的大小
	const uint32_t		__elenum;	//元素个数
	const uint32_t		__rqnum;	//队列元素个数
	const uint64_t		__length;	//buf长度
	unsigned char*		__addr;		//buf地址
	std::atomic_uint32_t	__front,
				__rear;		//队首、队尾标记
	ibv_recv_wr*		__wrs;		//WR队列
	ibv_sge*		__sg_lists;	//SGE队列
	uint64_t		__wr_id;	//WR ID
	ibv_recv_wr**		__ringqueue;	//存放WR地址的循环队列
	std::atomic<bool>*	__rq_flag;	//循环队列标志

public:
	explicit mpsc_atomic_mem_pool(const trans_args& transargs) noexcept;
	explicit mpsc_atomic_mem_pool(const trans_args* transargs) noexcept;
		 mpsc_atomic_mem_pool(const mpsc_atomic_mem_pool & ref)= delete;
		 mpsc_atomic_mem_pool(mpsc_atomic_mem_pool&& ref) noexcept;

		~mpsc_atomic_mem_pool(void) noexcept;

	mpsc_atomic_mem_pool& operator = (mpsc_atomic_mem_pool& ref) = delete;
	mpsc_atomic_mem_pool& operator = (mpsc_atomic_mem_pool&& ref) = delete;
	
	ibv_recv_wr*	get(void) noexcept;
	ibv_recv_wr*	tryget(void) noexcept;
	ibv_recv_wr*	timeget(void) noexcept;
	void		put(ibv_recv_wr* e) noexcept;
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

rfts::spsc_sem_mem_pool::spsc_sem_mem_pool(const trans_args& transargs) noexcept
	: __elesize(transargs.elesize)
	, __elenum(transargs.elenum)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_send_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_send_wr*[__elenum]())
	, __count(0, 0)
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__wrs[i].opcode = IBV_WR_SEND;
		__ringqueue[i] = &__wrs[i];
		__count.post();
		__rear += 1;
		if (__rear >= __elenum)
			__rear -= __elenum;
	}
}

rfts::spsc_sem_mem_pool::spsc_sem_mem_pool(const trans_args* transargs) noexcept
	: __elesize(transargs->elesize)
	, __elenum(transargs->elenum)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_send_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_send_wr*[__elenum]())
	, __count(0, 0)
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__wrs[i].opcode = IBV_WR_SEND;
		__ringqueue[i] = &__wrs[i];
		__count.post();
		__rear += 1;
		if (__rear >= __elenum)
			__rear -= __elenum;
	}
}

rfts::spsc_sem_mem_pool::spsc_sem_mem_pool(spsc_sem_mem_pool&& ref) noexcept
	: __elesize(ref.__elesize)
	, __elenum(ref.__elenum)
	, __length(ref.__length)
	, __addr(ref.__addr)
	, __front(ref.__front)
	, __rear(ref.__rear)
	, __wrs(ref.__wrs)
	, __sg_lists(ref.__sg_lists)
	, __wr_id(ref.__wr_id)
	, __ringqueue(ref.__ringqueue)
	, __count(0, ref.__count.getvalue())
{
	ref.__addr = nullptr;
	ref.__wrs  = nullptr;
	ref.__sg_lists = nullptr;
	ref.__ringqueue = nullptr;
}


rfts::spsc_sem_mem_pool::~spsc_sem_mem_pool(void) noexcept
{
	if (!__addr)
		return;
	while( __count.getvalue() != __elenum){};
	delete [] __ringqueue;
	__ringqueue = nullptr;
	delete [] __sg_lists;
	__sg_lists = nullptr;
	delete [] __wrs;
	__wrs = nullptr;
	delete [] __addr;
	__addr = nullptr;
}


void* rfts::spsc_sem_mem_pool::get_mrs_addr(void) const noexcept
{
	return __addr;
}


uint64_t rfts::spsc_sem_mem_pool::get_mrs_length(void) const noexcept
{
	return __length;
}

void* rfts::spsc_sem_mem_pool::get_wrs_addr(void) const noexcept
{
	return __wrs;
}


uint64_t rfts::spsc_sem_mem_pool::get_wrs_length(void) const noexcept
{
	return sizeof(ibv_send_wr) * __elenum;
}

void* rfts::spsc_sem_mem_pool::get_sgs_addr(void) const noexcept
{
	return __sg_lists;
}


uint64_t rfts::spsc_sem_mem_pool::get_sgs_length(void) const noexcept
{
	return sizeof(ibv_sge) * __elenum;
}


void* rfts::spsc_sem_mem_pool::get_rq_addr(void) const noexcept
{
	return __ringqueue;
}


uint64_t rfts::spsc_sem_mem_pool::get_rq_length(void) const noexcept
{
	return sizeof(ibv_send_wr*) * __elenum;
}


ibv_send_wr* rfts::spsc_sem_mem_pool::get(void) noexcept
{
	__count.wait();
	ibv_send_wr* temp = __ringqueue[__front];
	__ringqueue[__front++] = nullptr;
	if (__front  >= __elenum)
		__front -= __elenum;
	temp->wr_id = ++__wr_id;
	return temp;
}


ibv_send_wr* rfts::spsc_sem_mem_pool::tryget(void) noexcept
{
	uint64_t i = 0;
	for(i = 0; i < GET_TRY_NUM && EAGAIN == __count.trywait(); ++i)
	{}
	if (i >= GET_TRY_NUM)
	{
		std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		ibv_send_wr* temp = __ringqueue[__front++];
		if (__front  >= __elenum)
			__front -= __elenum;
		temp->wr_id = ++__wr_id;
		return temp;
	}
}


ibv_send_wr* rfts::spsc_sem_mem_pool::timeget(void) noexcept
{
	struct timespec timeout = {0,0};
	if (clock_gettime(CLOCK_REALTIME, &timeout))
		PERR(spsc_sem_mem_pool::timeget::clock_gettime);
	timeout.tv_nsec += GET_WAIT_TIME;
	if (ETIMEDOUT == __count.timewait(&timeout))
	{
		std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		ibv_send_wr* temp = __ringqueue[__front++];
		if (__front  >= __elenum)
			__front -= __elenum;
		temp->wr_id = ++__wr_id;
		return temp;
	}
}


void rfts::spsc_sem_mem_pool::put(ibv_send_wr* e) noexcept
{
	__ringqueue[__rear++] = e;
	if (__rear >= __elenum)
		__rear -=  __elenum;
	__count.post();
}

//------------

rfts::mpsc_sem_mem_pool::mpsc_sem_mem_pool(const trans_args& transargs) noexcept
	: __elesize(transargs.elesize)
	, __elenum(transargs.elenum)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_recv_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_recv_wr*[__elenum]())
	, __rq_flag(new std::atomic<bool>[__elenum]())
	, __count(0, 0)
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__ringqueue[i] = &__wrs[i];
		__rear += 1;
		if (__rear >= __elenum)
			__rear -= __elenum;
		__rq_flag[i].store(true, std::memory_order_relaxed);
		__count.post();
	}
}

rfts::mpsc_sem_mem_pool::mpsc_sem_mem_pool(const trans_args* transargs) noexcept
	: __elesize(transargs->elesize)
	, __elenum(transargs->elenum)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_recv_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_recv_wr*[__elenum]())
	, __rq_flag(new std::atomic<bool>[__elenum]())
	, __count(0, 0)
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__ringqueue[i] = &__wrs[i];
		__rear += 1;
		if (__rear >= __elenum)
			__rear -= __elenum;
		__rq_flag[i].store(true, std::memory_order_relaxed);
		__count.post();
	}
}

rfts::mpsc_sem_mem_pool::mpsc_sem_mem_pool(mpsc_sem_mem_pool&& ref) noexcept
	: __elesize(ref.__elesize)
	, __elenum(ref.__elenum)
	, __length(ref.__length)
	, __addr(ref.__addr)
	, __front(ref.__front)
	, __rear(ref.__rear.load(std::memory_order_relaxed))
	, __wrs(ref.__wrs)
	, __sg_lists(ref.__sg_lists)
	, __wr_id(ref.__wr_id)
	, __ringqueue(ref.__ringqueue)
	, __rq_flag(ref.__rq_flag)
	, __count(0, ref.__count.getvalue())
{
	ref.__addr = nullptr;
	ref.__wrs  = nullptr;
	ref.__sg_lists = nullptr;
	ref.__ringqueue = nullptr;
	ref.__rq_flag = nullptr;
}


rfts::mpsc_sem_mem_pool::~mpsc_sem_mem_pool(void) noexcept
{
	if (!__addr)
		return;
	while( __count.getvalue() != __elenum){};
	delete [] __rq_flag;
	__rq_flag = nullptr;
	delete [] __ringqueue;
	__ringqueue = nullptr;
	delete [] __sg_lists;
	__sg_lists = nullptr;
	delete [] __wrs;
	__wrs = nullptr;
	delete [] __addr;
	__addr = nullptr;
}


void* rfts::mpsc_sem_mem_pool::get_mrs_addr(void) const noexcept
{
	return __addr;
}


uint64_t rfts::mpsc_sem_mem_pool::get_mrs_length(void) const noexcept
{
	return __length;
}

void* rfts::mpsc_sem_mem_pool::get_wrs_addr(void) const noexcept
{
	return __wrs;
}


uint64_t rfts::mpsc_sem_mem_pool::get_wrs_length(void) const noexcept
{
	return sizeof(ibv_recv_wr) * __elenum;
}

void* rfts::mpsc_sem_mem_pool::get_sgs_addr(void) const noexcept
{
	return __sg_lists;
}


uint64_t rfts::mpsc_sem_mem_pool::get_sgs_length(void) const noexcept
{
	return sizeof(ibv_sge) * __elenum;
}

void* rfts::mpsc_sem_mem_pool::get_flags_addr(void) const noexcept
{
	return __rq_flag;
}


uint64_t rfts::mpsc_sem_mem_pool::get_flags_length(void) const noexcept
{
	return sizeof(std::atomic<bool>) * __elenum;
}


void* rfts::mpsc_sem_mem_pool::get_rq_addr(void) const noexcept
{
	return __ringqueue;
}


uint64_t rfts::mpsc_sem_mem_pool::get_rq_length(void) const noexcept
{
	return sizeof(ibv_recv_wr*) * __elenum;
}

ibv_recv_wr* rfts::mpsc_sem_mem_pool::get(void) noexcept
{
	__count.wait();
	ibv_recv_wr* temp = __ringqueue[__front];
	__ringqueue[__front] = nullptr;
	__rq_flag[__front++].store(false, std::memory_order_relaxed);
	if (__front  >= __elenum)
		__front -= __elenum;
	temp->wr_id = ++__wr_id;
	return temp;
}


ibv_recv_wr* rfts::mpsc_sem_mem_pool::tryget(void) noexcept
{
	uint64_t i = 0;
	for(i = 0; i < GET_TRY_NUM && EAGAIN == __count.trywait(); ++i){}
	if (i >= GET_TRY_NUM)
	{
		std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		ibv_recv_wr* temp = __ringqueue[__front];
		__ringqueue[__front] = nullptr;
		__rq_flag[__front++].store(false, std::memory_order_relaxed);
		if (__front  >= __elenum)
			__front -= __elenum;
		temp->wr_id = ++__wr_id;
		return temp;
	}
}


ibv_recv_wr* rfts::mpsc_sem_mem_pool::timeget(void) noexcept
{
	struct timespec timeout = {0,0};
	if (clock_gettime(CLOCK_REALTIME, &timeout))
		PERR(spsc_sem_mem_pool::timeget::clock_gettime);
	timeout.tv_nsec += GET_WAIT_TIME;
	if (ETIMEDOUT == __count.timewait(&timeout))
	{
		std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		ibv_recv_wr* temp = __ringqueue[__front];
		__ringqueue[__front] = nullptr;
		__rq_flag[__front++].store(false, std::memory_order_relaxed);
		if (__front  >= __elenum)
			__front -= __elenum;
		temp->wr_id = ++__wr_id;
		return temp;
	}
}


void rfts::mpsc_sem_mem_pool::put(ibv_recv_wr* e) noexcept
{
	bool flag = false;
	while(!__rq_flag[__rear.load(std::memory_order_acquire)].compare_exchange_weak(
		flag, true, std::memory_order_acq_rel, std::memory_order_acquire))
	{
		flag = false;
	}
	uint64_t temp = __rear.load(std::memory_order_relaxed);
	__ringqueue[temp++] = e;
	if (temp >= __elenum)
		temp -=  __elenum;
	__rear.store(temp, std::memory_order_release);
	__count.post();
}










rfts::spsc_atomic_mem_pool::spsc_atomic_mem_pool(const trans_args& transargs) noexcept
	: __elesize(transargs.elesize)
	, __elenum(transargs.elenum)
	, __rqnum(__elenum + 1)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_send_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_send_wr* [__rqnum]())
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__wrs[i].opcode = IBV_WR_SEND;
		__ringqueue[i] = &__wrs[i];
		__rear += 1;
		if (__rear >= __rqnum)
			__rear -= __rqnum;
	}
}

rfts::spsc_atomic_mem_pool::spsc_atomic_mem_pool(const trans_args* transargs) noexcept
	: __elesize(transargs->elesize)
	, __elenum(transargs->elenum)
	, __rqnum(__elenum + 1)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_send_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_send_wr* [__rqnum]())
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__wrs[i].opcode = IBV_WR_SEND;
		__ringqueue[i] = &__wrs[i];
		__rear += 1;
		if (__rear >= __rqnum)
			__rear -= __rqnum;
	}
}

rfts::spsc_atomic_mem_pool::spsc_atomic_mem_pool(spsc_atomic_mem_pool&& ref) noexcept
	: __elesize(ref.__elesize)
	, __elenum(ref.__elenum)
	, __rqnum(ref.__rqnum)
	, __length(ref.__length)
	, __addr(ref.__addr)
	, __front(ref.__front.load(std::memory_order_acquire))
	, __rear(ref.__rear.load(std::memory_order_acquire))
	, __wrs(ref.__wrs)
	, __sg_lists(ref.__sg_lists)
	, __wr_id(ref.__wr_id)
	, __ringqueue(ref.__ringqueue)
{
	ref.__addr = nullptr;
	ref.__wrs  = nullptr;
	ref.__sg_lists = nullptr;
	ref.__ringqueue = nullptr;
}


rfts::spsc_atomic_mem_pool::~spsc_atomic_mem_pool(void) noexcept
{
	if (!__addr)
		return;
	int32_t num;
	do
	{
		num = __rear.load(std::memory_order_acquire) -
			__front.load(std::memory_order_relaxed);
		if( num < 0)
			num += __rqnum;
		if(static_cast<uint32_t>(num) == __elenum)
			break;
	}while(true);
	delete [] __ringqueue;
	__ringqueue = nullptr;
	delete [] __sg_lists;
	__sg_lists = nullptr;
	delete [] __wrs;
	__wrs = nullptr;
	delete [] __addr;
	__addr = nullptr;
}


void* rfts::spsc_atomic_mem_pool::get_mrs_addr(void) const noexcept
{
	return __addr;
}


uint64_t rfts::spsc_atomic_mem_pool::get_mrs_length(void) const noexcept
{
	return __length;
}

void* rfts::spsc_atomic_mem_pool::get_wrs_addr(void) const noexcept
{
	return __wrs;
}


uint64_t rfts::spsc_atomic_mem_pool::get_wrs_length(void) const noexcept
{
	return sizeof(ibv_send_wr) * __elenum;
}

void* rfts::spsc_atomic_mem_pool::get_sgs_addr(void) const noexcept
{
	return __sg_lists;
}


uint64_t rfts::spsc_atomic_mem_pool::get_sgs_length(void) const noexcept
{
	return sizeof(ibv_sge) * __elenum;
}


void* rfts::spsc_atomic_mem_pool::get_rq_addr(void) const noexcept
{
	return __ringqueue;
}


uint64_t rfts::spsc_atomic_mem_pool::get_rq_length(void) const noexcept
{
	return sizeof(ibv_send_wr*) * __rqnum;
}


ibv_send_wr* rfts::spsc_atomic_mem_pool::get(void) noexcept
{
	uint32_t front = __front.load(std::memory_order_relaxed),
		 rear  = __rear.load(std::memory_order_acquire);
	while(front == rear)
		 rear  = __rear.load(std::memory_order_acquire);
	ibv_send_wr* temp = __ringqueue[front];
	__ringqueue[front++] = nullptr;
	if (front >= __rqnum)
		front -= __rqnum;
	__front.store(front, std::memory_order_release);
	temp->wr_id = ++__wr_id;
	return temp;
}


ibv_send_wr* rfts::spsc_atomic_mem_pool::tryget(void) noexcept
{
	uint64_t i = 0;
	uint32_t front = __front.load(std::memory_order_relaxed),
		 rear  = __rear.load(std::memory_order_acquire);
	for(i = 0; i < GET_TRY_NUM && front == rear; ++i)
		 rear  = __rear.load(std::memory_order_acquire);
	if (i >= GET_TRY_NUM)
	{
		std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		ibv_send_wr* temp = __ringqueue[front];
		__ringqueue[front++] = nullptr;
		if (front  >= __rqnum)
			front -= __rqnum;
		__front.store(front, std::memory_order_release);
		temp->wr_id = ++__wr_id;
		return temp;
	}
}


ibv_send_wr* rfts::spsc_atomic_mem_pool::timeget(void) noexcept
{
	struct timespec oldtime = {0, 0}, newtime = {0, 0};
	clock_gettime(CLOCK_REALTIME, &oldtime);
	uint64_t front = __front.load(std::memory_order_relaxed);
	uint64_t rear = __rear.load(std::memory_order_acquire);
	while(front == rear)
	{
		clock_gettime(CLOCK_REALTIME, &newtime);
		if ((newtime.tv_sec - oldtime.tv_sec) * 1000000000 +
			newtime.tv_nsec - oldtime.tv_nsec > GET_WAIT_TIME)
		{
			std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
			exit(EXIT_FAILURE);
		}
		rear = __rear.load(std::memory_order_acquire);
	}
	ibv_send_wr* temp = __ringqueue[front];
	__ringqueue[front++] = nullptr;
	if (front  >= __rqnum)
		front -= __rqnum;
	__front.store(front, std::memory_order_release);
	temp->wr_id = ++__wr_id;
	return temp;
}


void rfts::spsc_atomic_mem_pool::put(ibv_send_wr* e) noexcept
{
	uint64_t rear =  __rear.load(std::memory_order_relaxed);
	__ringqueue[rear++] = e;
	if (rear >= __rqnum)
		rear -=  __rqnum;
	__rear.store(rear, std::memory_order_release);
}

//------------

rfts::mpsc_atomic_mem_pool::mpsc_atomic_mem_pool(const trans_args& transargs) noexcept
	: __elesize(transargs.elesize)
	, __elenum(transargs.elenum)
	, __rqnum(__elenum + 1)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_recv_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_recv_wr*[__elenum]())
	, __rq_flag(new std::atomic<bool>[__elenum]())
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__ringqueue[i] = &__wrs[i];
		__rear += 1;
		if (__rear >= __rqnum)
			__rear -= __rqnum;
		__rq_flag[i].store(true, std::memory_order_relaxed);
	}
}

rfts::mpsc_atomic_mem_pool::mpsc_atomic_mem_pool(const trans_args* transargs) noexcept
	: __elesize(transargs->elesize)
	, __elenum(transargs->elenum)
	, __rqnum(__elenum + 1)
	, __length(__elesize * __elenum)
	, __addr(new unsigned char[__length]())
	, __front(0)
	, __rear(0)
	, __wrs(new ibv_recv_wr[__elenum]())
	, __sg_lists(new ibv_sge [__elenum]())
	, __wr_id(0)
	, __ringqueue(new ibv_recv_wr*[__elenum]())
	, __rq_flag(new std::atomic<bool>[__elenum]())
{
	for(uint64_t i = 0; i < __elenum; ++i)
	{
		__sg_lists[i].addr = (uint64_t)(__addr + i * __elesize);
		__sg_lists[i].length = __elesize;
		__wrs[i].next = nullptr,
		__wrs[i].sg_list = &__sg_lists[i],
		__wrs[i].num_sge = 1,
		__ringqueue[i] = &__wrs[i];
		__rear += 1;
		if (__rear >= __rqnum)
			__rear -= __rqnum;
		__rq_flag[i].store(true, std::memory_order_relaxed);
	}
}

rfts::mpsc_atomic_mem_pool::mpsc_atomic_mem_pool(mpsc_atomic_mem_pool&& ref) noexcept
	: __elesize(ref.__elesize)
	, __elenum(ref.__elenum)
	, __rqnum(ref.__rqnum)
	, __length(ref.__length)
	, __addr(ref.__addr)
	, __front(ref.__front.load(std::memory_order_relaxed))
	, __rear(ref.__rear.load(std::memory_order_relaxed))
	, __wrs(ref.__wrs)
	, __sg_lists(ref.__sg_lists)
	, __wr_id(ref.__wr_id)
	, __ringqueue(ref.__ringqueue)
	, __rq_flag(ref.__rq_flag)
{
	ref.__addr = nullptr;
	ref.__wrs  = nullptr;
	ref.__sg_lists = nullptr;
	ref.__ringqueue = nullptr;
	ref.__rq_flag = nullptr;
}


rfts::mpsc_atomic_mem_pool::~mpsc_atomic_mem_pool(void) noexcept
{
	if (!__addr)
		return;
	int32_t num = 0;
	do
	{
		num = __rear.load(std::memory_order_acquire) -
			__front.load(std::memory_order_relaxed);
		if( num < 0)
			num += __rqnum;
		if(static_cast<uint64_t>(num) == __elenum)
			break;
	}while(true);

	delete [] __rq_flag;
	__rq_flag = nullptr;
	delete [] __ringqueue;
	__ringqueue = nullptr;
	delete [] __sg_lists;
	__sg_lists = nullptr;
	delete [] __wrs;
	__wrs = nullptr;
	delete [] __addr;
	__addr = nullptr;
}


void* rfts::mpsc_atomic_mem_pool::get_mrs_addr(void) const noexcept
{
	return __addr;
}


uint64_t rfts::mpsc_atomic_mem_pool::get_mrs_length(void) const noexcept
{
	return __length;
}

void* rfts::mpsc_atomic_mem_pool::get_wrs_addr(void) const noexcept
{
	return __wrs;
}


uint64_t rfts::mpsc_atomic_mem_pool::get_wrs_length(void) const noexcept
{
	return sizeof(ibv_recv_wr) * __elenum;
}

void* rfts::mpsc_atomic_mem_pool::get_sgs_addr(void) const noexcept
{
	return __sg_lists;
}


uint64_t rfts::mpsc_atomic_mem_pool::get_sgs_length(void) const noexcept
{
	return sizeof(ibv_sge) * __elenum;
}

void* rfts::mpsc_atomic_mem_pool::get_flags_addr(void) const noexcept
{
	return __rq_flag;
}


uint64_t rfts::mpsc_atomic_mem_pool::get_flags_length(void) const noexcept
{
	return sizeof(std::atomic<bool>) * __rqnum;
}


void* rfts::mpsc_atomic_mem_pool::get_rq_addr(void) const noexcept
{
	return __ringqueue;
}


uint64_t rfts::mpsc_atomic_mem_pool::get_rq_length(void) const noexcept
{
	return sizeof(ibv_recv_wr*) * __rqnum;
}

ibv_recv_wr* rfts::mpsc_atomic_mem_pool::get(void) noexcept
{
	uint32_t front = __front.load(std::memory_order_relaxed),
		 rear = __rear.load(std::memory_order_acquire);
	while(front == rear)
		 rear = __rear.load(std::memory_order_acquire);
	ibv_recv_wr* temp = __ringqueue[front];
	__ringqueue[front] = nullptr;
	__rq_flag[front++].store(false, std::memory_order_relaxed);
	if (front  >= __rqnum)
		front -= __rqnum;
	temp->wr_id = ++__wr_id;
	__front.store(front, std::memory_order_release);
	return temp;
}


ibv_recv_wr* rfts::mpsc_atomic_mem_pool::tryget(void) noexcept
{
	uint64_t i = 0;
	uint32_t front = __front.load(std::memory_order_relaxed),
		 rear = __rear.load(std::memory_order_acquire);
	for(i = 0; i < GET_TRY_NUM && front == rear; ++i){}
		 rear  = __rear.load(std::memory_order_acquire);
	if (i >= GET_TRY_NUM)
	{
		std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		ibv_recv_wr* temp = __ringqueue[front];
		__ringqueue[front] = nullptr;
		__rq_flag[front++].store(false, std::memory_order_relaxed);
		if (front  >= __rqnum)
			front -= __rqnum;
		temp->wr_id = ++__wr_id;
		__front.store(front, std::memory_order_release);
		return temp;
	}
}


ibv_recv_wr* rfts::mpsc_atomic_mem_pool::timeget(void) noexcept
{
	struct timespec oldtime = {0, 0}, newtime = {0, 0};
	clock_gettime(CLOCK_REALTIME, &oldtime);
	uint64_t front = __front.load(std::memory_order_relaxed);
	uint64_t rear = __rear.load(std::memory_order_acquire);
	while(front == rear)
	{
		clock_gettime(CLOCK_REALTIME, &newtime);
		if ((newtime.tv_sec - oldtime.tv_sec) * 1000000000 +
			newtime.tv_nsec - oldtime.tv_nsec > GET_WAIT_TIME)
		{
			std::cout << "MEM_POOL_CAPACITY_SCALE too small" << std::endl;
			exit(EXIT_FAILURE);
		}
		rear = __rear.load(std::memory_order_acquire);
	}
	ibv_recv_wr* temp = __ringqueue[front];
	__ringqueue[front] = nullptr;
	__rq_flag[front++].store(false, std::memory_order_relaxed);
	if (front  >= __rqnum)
		front -= __rqnum;
	temp->wr_id = ++__wr_id;
	__front.store(front, std::memory_order_release);
	return temp;
}

void rfts::mpsc_atomic_mem_pool::put(ibv_recv_wr* e) noexcept
{
	bool flag = false;
	while(!__rq_flag[__rear.load(std::memory_order_acquire)].compare_exchange_weak(
		flag, true, std::memory_order_acq_rel, std::memory_order_acquire))
		flag = false;
	uint64_t rear = __rear.load(std::memory_order_relaxed);
	__ringqueue[rear++] = e;
	if (rear >= __rqnum)
		rear -=  __rqnum;
	__rear.store(rear, std::memory_order_release);
}


#endif /* end of include guard: HPP_RFTS_MEM_POOL_HPP */

