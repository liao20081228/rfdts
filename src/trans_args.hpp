#pragma once
#ifndef HPP_RFTS_TRANS_ARGS_HPP
#define HPP_RFTS_TRANS_ARGS_HPP

#include<infiniband/verbs.h>
#include<semaphore.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<ctime>
#include<cmath>
#include<cerrno>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<iostream>
#include<thread>
#include<atomic>
#include<vector>
#include<array>

#ifndef RFTS_PRINT_ERROR_INFO_AND_EXIT_WITH_ERRNO
	#define _PEIE(a) {perror(#a);exit(errno);}
	#define PEIE(c) _PEIE(c() failed)
#endif

#ifndef RFTS_PRINT_ERROR_INFO
	#define _PEI(a) {perror(#a);}
	#define PEI(c) _PEI(c() failed)
#endif

#ifndef RFTS_REGULAR_FILE_MODE
	#define REGULAR_FILE_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
#endif

namespace rfts
{

extern std::atomic<bool> start;

struct data//数据结构
{
	uint16_t		nodeid;		//节点ID
	uint8_t			sensorid;	//传感器ID
	uint8_t			type;		//数据类型,束流位置？长度？
	struct timespec		time;		//时间
	uint64_t		number;		//编号
	long double		value;		//数据值
};

struct trans_args//传输参数
{
	uint64_t	size_per_data	= sizeof(data);	//每个物理量数据的大小
	uint32_t	afreq,				//采集平率
			tfreq,				//传输频率
			kind_per_sensor,		//一个探头产生几种数据
			sensor_num_per_node,		//每个节点有几个探头
			node_num	= 1;		//总共多少个节点
	float		ele_size_scale	= 1.5,		//注册内存分片冗余比例
			ele_num_scale	= 2,		//注册内存数量冗余比例
			th_wait_time,			//线程等待时间,us
			data_handle_time,		//数据处理时间,us
			sys_lat		= 10,		//系统延时,us
			pre_rr_scale  = 0.7;		//预发布接收工作请求的比例

	char*		dev_name	= nullptr;	//RDMA设备名
	char*		servername	= nullptr;	//服务端的IP或域名
	uint32_t	port		= 12345,	//TCP端口
			ib_port		= 1,		//ib物理端口
			gid_index	= 1,		//gid索引
			sl		= 0,		//服务级别
			wr_sge_num	= 1;		//sg列表元素数目
//		, th_num(ceil((__th_wait_time + __data_handle_time) / 1000000 *
//				__tfreq * __node_num))
	friend std::ostream& operator << (std::ostream& out, trans_args& ref)  noexcept;
	uint64_t get_elesize(void) const noexcept;
	uint32_t get_elenum(void) const noexcept;
};

inline uint32_t trans_args::get_elenum(void) const noexcept
{
	return ceil(tfreq * sys_lat / 1000000 * node_num * ele_num_scale);
}

inline uint64_t trans_args::get_elesize(void) const noexcept
{
	return ceil(static_cast<float>(afreq) / tfreq * size_per_data *
		kind_per_sensor * sensor_num_per_node * node_num *
		ele_size_scale);
}

inline std::ostream& operator << (std::ostream& out, trans_args& ref)  noexcept
	{
	out << "afreq              : "	<< ref.afreq			<< "\n"
	    << "tfreq              : "	<< ref.tfreq			<< "\n"
	    << "size_per_data      : "	<< ref.size_per_data		<< "\n"
	    << "kind_per_sensor    : "	<< ref.kind_per_sensor		<< "\n"
	    << "sensor_num_per_node: "	<< ref.sensor_num_per_node	<<"\n"
	    << "node_num           : "	<< ref.node_num			<< "\n"
	    << "ele_num_scale_num  : "	<< ref.ele_num_scale		<< "\n"
	    << "ele_size_scale     : "	<< ref.ele_size_scale		<< "\n"
	    << "th_wait_time       : "	<< ref.th_wait_time		<< "\n"
	    << "data_handle_time   : "	<< ref.data_handle_time		<< "\n"
	    << "sys_lat            : "	<< ref.sys_lat			<< "\n"
	    << "dev_name           : "	<< ref.dev_name			<< "\n"
	    << "servername         : "	<< ref.servername		<< "\n"
	    << "port               : "	<< ref.port			<< "\n"
	    << "ib_port            : "	<< ref.ib_port			<< "\n"
	    << "gid_index          : "	<< ref.gid_index		<< "\n"
	    << "sl                 : "	<< ref.sl			<< "\n"
	    << "wr_sge_num         : "	<< ref.wr_sge_num		<< "\n";
		return out;
	}

}
#endif /* end of include guard: RFTS_TRANS_ARG_H */
