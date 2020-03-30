#define RFTS_SERVER
#include"../wr_pool.hpp"
#include"../get_clock_ns.hpp"
#include<thread>
#include<iostream>
#include<unistd.h>
#include<algorithm>
#define  MEM_POOL wr_pool<ibv_recv_wr>

using namespace rfts;
const int testnum=100;
ibv_recv_wr* addr;
void fun_malloc(MEM_POOL&);
int main()
{
	trans_args tsas;
	tsas.afreq = 100;
	tsas.tfreq = 100;
	tsas.size_per_data = 100;
	tsas.sys_lat = 1000000;
	tsas.node_num =1;
	tsas.kind_per_sensor =1;
	tsas.sensor_num_per_node =1;

	MEM_POOL mempool = MEM_POOL(tsas);
	std::cout << mempool.get_mrs_length() << std::endl;

	fun_malloc(mempool);
}

void fun_malloc(MEM_POOL& mempool)
{
	long double sum1 = 0, sum2 = 0, max1 = 0, min1 = 10000, max2 = 0, min2 = 10000;
	cycles_t s=0;
	cycles_t e=0;
	double mhz = 0;
	double temp = 0;
		mhz=get_cpu_ghz(0);
	if (mhz==0)
		return;
	for(int i=0; i<testnum;i++)
	{
		s=get_cycles();
		addr = mempool.get();
		e=get_cycles();
		temp =  ( e-s ) / mhz ;
		sum1 += temp;
		max1 = temp > max1 ? temp : max1;
		min1 = temp < min1 ? temp : min1;

		s=get_cycles();
		mempool.put(addr);
		e=get_cycles();
		temp =  ( e-s ) / mhz ;
		sum2 += temp;
		max2 = temp > max2 ? temp : max2;
		min2 = temp < min2 ? temp : min2;
		//std::cout<<"wr's address: " << addr << " ,mr's address: "<< addr->sg_list->addr << std::endl;
		
	}
std::cout << "malloc mean: " << sum1/testnum << ", max: " <<  max1 << ", min:" << min1 << std::endl
	<< "free   mean: " << sum2/testnum << ", max: " <<  max2 << ", min:" << min2 << std::endl;
}


