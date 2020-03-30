#pragma once
#ifndef HPP_RFTS_COMMU_HPP
#define HPP_RFTS_COMMU_HPP

#include"queue.hpp"
#include"wr_pool.hpp"
#define RFTS_SERVER

namespace rfts
{

template<typename T>
class commu
{
private:
	const trans_args&	__tsas;			//传输参数
	wr_pool<T>&		__wrpool;		//发送工作请求池
	queues<T*>&		__queues;		//三个队列
private:
	ibv_context*		__context;		//设备上下文
	ibv_comp_channel*	__comp_channel;		//完成通道
	ibv_cq*			__cq;			//完成队列
	ibv_pd*			__pd;			//保护域
	ibv_qp*			__qp;			//QP
	ibv_ah			__ah;			//地址句柄
	ibv_device_attr*	__device_attr;		//设备属性
	ibv_port_attr*		__port_attr;		//端口属性
private:
	std::thread		__th_send_or_recv;	//发送或接收线程
	std::thread		__th_comp;		//完成线程
	std::thread		__th_comp_check;	//完成检查线程
#ifdef RFTS_SERVER
	posix_sem		__sem;			//信号量,用于服务端同步
#endif
private:
	void __send_or_recv(void) noexcept;
	void __comp(void) noexcept;
	void __comp_check(void) noexcept;
public:
	commu(const trans_args& tsas, wr_pool<T>& wrpool, queues<T*>& ques) noexcept;
	~commu(void) noexcept;
	commu(const commu&) = delete;
	commu(commu&&) = delete;
	commu& operator = (const commu&) = delete;
	commu& operator = (commu&&) = delete;
private:
	void __open_context(void) noexcept;//根据__tsas中指定的设备名打开设备上下文
	void __query_device_attr(void) noexcept;
	void __query_port_attr(void) noexcept;
};

}


template<typename T>
rfts::commu<T>::commu(const trans_args& tsas,wr_pool<T>& wrpool, queues<T*>& ques) noexcept
	: __tsas(tsas)
	, __wrpool(wrpool)
	, __queues(ques)
{
	__open_context();
}


template<typename T>
rfts::commu<T>::~commu(void) noexcept
{

	if (__context)
	{
		if (ibv_close_device(__context))
			PEIE(rfts::commu::~commu);
	}
}



template<typename T>
void rfts::commu<T>::__query_port_attr(void) noexcept
{

}





template<typename T>
void rfts::commu<T>::__open_context(void) noexcept
{
	ibv_fork_init();
	int devnum = 0;
	ibv_device** devlist = ibv_get_device_list(&devnum);
	if (!devlist || !devnum)
		PEIE(rfts::commu::__open_context::ibv_get_device_list);
	if (!__tsas.dev_name)
	{
		ibv_context* context = ibv_open_device(devlist[0]);
		if (context)
		{
			__context = context;
			ibv_free_device_list(devlist);
			return;
		}
		else
		{
			ibv_free_device_list(devlist);
			PEIE(rfts::commu::__open_context::ibv_open_device);
		}
		
	}
	else
	{
		for (int i = 0; i < devnum; ++i)
		{
			std::string temp  = ibv_get_device_name(devlist[i]);
			if (temp == __tsas.dev_name)
			{
				ibv_context* context = ibv_open_device(devlist[i]);
				if (context)
				{
					__context = context;
					ibv_free_device_list(devlist);
					return;
				}
				else
				{
					ibv_free_device_list(devlist);
					PEIE(rfts::commu::__open_context::ibv_open_device);
				}
			}
			continue;
		}
		ibv_free_device_list(devlist);
		errno = EINVAL;
		PEIE(rfts::commu::__open_context);
	}
	
}




template<typename T>
void rfts::commu<T>::__comp(void) noexcept
{
#ifdef RFTS_SERVER


#else



#endif
}



template<typename T>
void rfts::commu<T>::__send_or_recv(void) noexcept
{
	T* bad_wr;
#ifdef RFTS_SERVER
	uint32_t pre_recv_wr = ceil(__queues.nosend_or_recving.__rqnum * __tsas.pre_recv_scale);
	pre_recv_wr = pre_recv_wr > __device_attr->max_qp_wr ? __device_attr->max_qp_wr :
		pre_recv_wr;
	for(uint32_t i = 0; i < pre_recv_wr; ++i)
	{
		T*  recv_wr= __wrpool.get();
		if (ibv_post_recv(__qp, recv_wr, &bad_wr))
			PERR(rfts::commu::__send_or_recv);
		__queues.nosend_or_recving.put(recv_wr);
	}
	while(start.load(std::memory_order_acquire))
	{
		__sem.wait();
		T*  recv_wr= __wrpool.get();
		if (ibv_post_recv(__qp, recv_wr, &bad_wr))
			PERR(rfts::commu::__send_or_recv);
		__queues.nosend_or_recving.put(recv_wr);
	}
	timespec	timeout = {0,0};
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_nsec += __tsas.sys_lat * 2 * 1000;
	while(__sem.timewait(&timeout) != ETIMEDOUT)
	{
		T*  recv_wr= __wrpool.get();
		if (ibv_post_recv(__qp, recv_wr, &bad_wr))
			PERR(rfts::commu::__send_or_recv);
		__queues.nosend_or_recving.put(recv_wr);
		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_nsec += __tsas.sys_lat * 2 * 1000;
	}
#else
	while(start.load(std::memory_order_acquire))
	{
		T* send_wr= __queues.nosend_or_recving.get();
		if(ibv_post_send(__qp, send_wr, &bad_wr))
			PERR(rfts::commu::__send_or_recv);
		__queues.sending_or_nohand.put(send_wr);
	}
	while(__queues.nosend_or_recving.get_rq_elenum())
	{
		T* send_wr= __queues.nosend_or_recving.get();
		if(ibv_post_send(__qp, send_wr, &bad_wr))
			PERR(rfts::commu::__send_or_recv);
		__queues.sending_or_nohand.put(send_wr);
	}
#endif
}


#endif /* end of include guard: HPP_RFTS_COMM_HPP */