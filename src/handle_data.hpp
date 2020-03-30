#pragma once
#ifndef HPP_RFTS_HADNLE_DATA_HPP
#define HPP_RFTS_HADNLE_DATA_HPP

#include"./trans_args.hpp"
#include"./queue.hpp"
#include"./wr_pool.hpp"
#include<thread>
#include<ctime>
#include<chrono>

namespace rfts
{
class datahandle
{
private:
	wr_pool<ibv_recv_wr>*	__wrpool;
	queue<ibv_recv_wr*>*	__nohandle;
	trans_args*		__tsas;
	uint64_t		__th_num;
	std::thread*		__threads;
private:
	void __worker(void) noexcept;
public:
	explicit datahandle(wr_pool<ibv_recv_wr>& wrpool,
			queue<ibv_recv_wr*>& nohandle, trans_args& tsas) noexcept;
	datahandle(const datahandle& ref) = delete;
	datahandle(datahandle&& ref) noexcept;

	datahandle& operator = (const datahandle& ref) = delete;
	datahandle& operator = (datahandle&& ref) = delete;

	~datahandle(void) noexcept;
};

}

rfts::datahandle::datahandle(wr_pool<ibv_recv_wr>& wrpool,
		queue<ibv_recv_wr*>& nohandle, trans_args& tsas) noexcept
	: __wrpool(&wrpool)
	, __nohandle(&nohandle)
	, __tsas(&tsas)
	, __th_num(ceil((tsas.th_wait_time + tsas.data_handle_time) / 1000000 *
			tsas.tfreq * tsas.node_num))
	, __threads(new std::thread [__th_num]())
{
	for (uint16_t i = 0; i < __th_num; ++i)
		__threads[i] = std::thread(&datahandle::__worker, this);
}


rfts::datahandle::datahandle(datahandle&& ref) noexcept
	: __wrpool(ref.__wrpool)
	, __nohandle(ref.__nohandle)
	, __tsas(ref.__tsas)
	, __th_num(ref.__th_num)
	, __threads(ref.__threads)
{
	__threads = nullptr;
}

rfts::datahandle::~datahandle(void) noexcept
{
	if (!__threads)
		return;
	for (uint64_t i = 0 ; i < __th_num; ++i)
	{
		if (__threads->joinable())
			__threads[i].join();
	}
	delete [] __threads;
}

void rfts::datahandle::__worker(void) noexcept
{
	while (true)
	{
		if (start.load(std::memory_order_acquire))
		{
			ibv_recv_wr* recv_wr = __nohandle->get();
			data* temp = reinterpret_cast<data*>(recv_wr->sg_list->addr);
			uint32_t len = recv_wr->sg_list->length / sizeof(data);
			for (uint64_t i = 0; i < len; ++i)
			{
				std::cout << "编号："	<< temp[i].number 
					<< " 时间："	<< ctime(&temp[i].time.tv_sec)
					<< "，"		<<temp[i].time.tv_nsec << "纳秒"
					<< ", 节点ID"	<< temp[i].nodeid
					<< ",传感器ID"	<< temp[i].sensorid
					<< ",类型ID"	<< temp[i].type
					<< ",值"	<< temp[i].value <<std::endl;
			}
			__wrpool->put(recv_wr);
		}
		else
		{
			if (__nohandle->get_rq_elenum())
				continue;
			else
			{
				std::this_thread::sleep_for(
					std::chrono::microseconds(
					static_cast<uint32_t>(ceil(__tsas->sys_lat))));
				if (__nohandle->get_rq_elenum())
						continue;
				else
					break;
			}
		}
	}
}

#endif /* end of include guard: HPP_RFTS_HADNLE_DATA_HPP */
