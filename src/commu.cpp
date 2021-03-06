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

private:
	ibv_context*		__context;		//设备上下文
	ibv_comp_channel*	__comp_channel;		//完成通道
	ibv_cq*			__cq;			//完成队列
	ibv_pd*			__pd;			//保护域
	ibv_qp*			__qp;			//QP
	ibv_mr*			__mr;
	ibv_ah*			__ah;			//地址句柄
	ibv_device_attr_ex*	__device_attr_ex;	//设备属性和扩展属性
	ibv_port_attr*		__port_attr;		//端口属性
private:
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
	void __query_attr(void) const noexcept;//查询设备属性和端口属性
	void __check_mtu(void) const noexcept; //检查MTU
	void __create_basic_resource(void) noexcept;//创建基本资源
	void __create_qp(void) noexcept;//创建QP
};

}


template<typename T>
inline rfts::commu<T>::commu(const trans_args& tsas,wr_pool<T>& wrpool, queues<T*>& ques) noexcept
	: __tsas(tsas)
	, __wrpool(wrpool)
	, __queues(ques)
	, __context(nullptr)
	, __comp_channel(nullptr)
	, __cq(nullptr)
	, __pd(nullptr)
	, __qp(nullptr)
	, __mr(nullptr)
	, __ah(nullptr)
	, __device_attr_ex(new ibv_device_attr_ex)
	, __port_attr(new ibv_port_attr)
{
	__open_context();
	__query_attr();
	__check_mtu();
	__create_basic_resource();
	__create_qp();
}


template<typename T>
inline rfts::commu<T>::~commu(void) noexcept
{
	if (__cq && ibv_destroy_cq(__cq))
		PEIE(rfts::commu::~commu::ibv_destroy_cq);
	if (__comp_channel && ibv_destroy_comp_channel(__comp_channel))
		PEIE(rfts::commu::~commu::ibv_destroy_comp_channel);
	if (__mr && ibv_dereg_mr(__mr))
		PEIE(rfts::commu::~commu::ibv_dereg_mr);
	if (__pd && ibv_dealloc_pd(__pd))
		PEIE(rfts::commu::~commu::ibv_dealloc_pd);
	if (__port_attr)
		delete __port_attr;
	if (__device_attr_ex)
		delete __device_attr_ex;
	if (__context && ibv_close_device(__context))
		PEIE(rfts::commu::~commu::ibv_close_device);
}

template<typename T>
inline void rfts::commu<T>::__create_qp(void) noexcept
{
	ibv_qp_init_attr qp_init_attr =
	{
		.qp_context = nullptr,
		.send_cq = __cq,
		.recv_cq = __cq,
		.srq = nullptr,
		.cap =
		{
			.max_send_wr = 256,
			.max_recv_wr = 256 ,
			.max_send_sge = 1,
			.max_recv_sge = 1,
			.max_inline_data = __port_attr->active_mtu
		},
		.qp_type = IBV_QPT_UD,
		.sq_sig_all = 0
	};
	if (!(__qp = ibv_create_qp(__pd, &qp_init_attr)))
	{
		PEI(rfts::commu::__create_qp::ibv_create_qp);
		~commu();
	}
	ibv_qp_attr qp_attr;
	memset(&qp_attr, 0, sizeof(ibv_qp_attr));
	memset(&qp_init_attr, 0, sizeof(ibv_qp_init_attr));
	ibv_quert_qp(__qp, &qp_attr, IBV_QP_CAP, &qp_init_attr);
	if (qp_init_attr.cap.max_inline_data >= __tsas.get_elenum())
	{

	}
}

template<typename T>
inline void rfts::commu<T>::__create_basic_resource(void) noexcept
{
	if (!(__pd = ibv_alloc_pd(__context)))
	{
		PEI(rfts::commu::__create_basic_resource::ibv_alloc_pd);
		~commu();
	}
	if (!(__mr = ibv_reg_mr(__pd, __wrpool.get_mrs_addr, __wrpool.get_mrs_length(),
			IBV_ACCESS_LOCAL_WRITE)))
	{
		PEI(rfts::commu::__create_basic_resource::ibv_reg_mr);
		~commu();
	}
	if (!(__comp_channel = ibv_create_comp_channel(__context)))
	{
		PEI(rfts::commu::__create_basic_resource::ibv_create_comp_channel);
		~commu();
	}
	if (!(__cq = ibv_create_cq(__context, __tsas.get_elenum() <
		__device_attr_ex->orig_attr.max_cqe ? __tsas.get_elenum() :
		__device_attr_ex->orig_attr.max_cqe, nullptr, __comp_channel, 0)))
	{
		PEI(rfts::commu::__create_basic_resource::ibv_create_cq);
		~commu();
	}
}

template<typename T>
inline void rfts::commu<T>::__check_mtu(void) const noexcept
{
	if (__port_attr->active_mtu < __tsas.get_elesize())
	{
		errno = ENOSYS;
		PEI(rfts::commu::__check_mtu);
		~commu();
	}
}

template<typename T>
inline void rfts::commu<T>::__query_attr(void) const noexcept
{
	memset(__device_attr_ex, 0, sizeof(ibv_device_attr_ex));
	if (ibv_query_device_ex(__context, nullptr, __device_attr_ex))
	{
		PEI(rfts::commu::__query_attr::ibv_query_device_ex);
		~commu();
	}
	memset(__port_attr, 0, sizeof(ibv_port_attr));
	if (__tsas.ib_port >= 1 && __tsas.ib_port <= __device_attr_ex->orig_attr.phys_port_cnt)
	{
		if (ibv_query_port(__context, __tsas.ib_port, __port_attr))
		{
			PEI(rfts::commu::__query_attr::ibv_query_port);
			~commu();
		}
	}
	else
	{
			errno = EINVAL;
			PEI(rfts::commu::__query_attr);
			~commu();
	}
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
	uint32_t pre_recv_wr = ceil(__queues.nosend_or_recving.__rqnum * __tsas.pre_rr_scale);
	pre_recv_wr = pre_recv_wr > __device_attr_ex->orig_attr.max_qp_wr ?
		__device_attr_ex->orig_attr.max_qp_wr : pre_recv_wr;
	for(uint32_t i = 0; i < pre_recv_wr; ++i)
	{
		T*  recv_wr= __wrpool.get();
		if (ibv_post_recv(__qp, recv_wr, &bad_wr))
			PEIE(rfts::commu::__send_or_recv);
		__queues.nosend_or_recving.put(recv_wr);
	}
	while(start.load(std::memory_order_acquire))
	{
		__sem.wait();
		T*  recv_wr= __wrpool.get();
		if (ibv_post_recv(__qp, recv_wr, &bad_wr))
			PEIE(rfts::commu::__send_or_recv);
		__queues.nosend_or_recving.put(recv_wr);
	}
	timespec	timeout = {0,0};
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_nsec += __tsas.sys_lat * 2 * 1000;
	while(__sem.timewait(&timeout) != ETIMEDOUT)
	{
		T*  recv_wr= __wrpool.get();
		if (ibv_post_recv(__qp, recv_wr, &bad_wr))
			PEIE(rfts::commu::__send_or_recv);
		__queues.nosend_or_recving.put(recv_wr);
		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_nsec += __tsas.sys_lat * 2 * 1000;
	}
#else
	while(start.load(std::memory_order_acquire))
	{
		T* send_wr= __queues.nosend_or_recving.get();
		if(ibv_post_send(__qp, send_wr, &bad_wr))
			PEIE(rfts::commu::__send_or_recv);
		__queues.sending_or_nohand.put(send_wr);
	}
	while(__queues.nosend_or_recving.get_rq_elenum())
	{
		T* send_wr= __queues.nosend_or_recving.get();
		if(ibv_post_send(__qp, send_wr, &bad_wr))
			PEIE(rfts::commu::__send_or_recv);
		__queues.sending_or_nohand.put(send_wr);
	}
#endif
}


#endif /* end of include guard: HPP_RFTS_COMM_HPP */
