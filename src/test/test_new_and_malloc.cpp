#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<infiniband/verbs.h>
#include"../get_clock.hpp"


void fun()
{
	double mean1 = 0,
	       mean2 = 0,
	       max1  = 0,
	       max2  = 0,
	       min1  =  10000000,
	       min2  =  10000000,
	       mhz   =  0,
		v    =  0;
	ibv_send_wr* p = 0;
	cycles_t s,e;
	for(int i = 0; i < 100;++i)
	{
		mhz = get_cpu_mhz(0);
		s=get_cycles();
		p=(ibv_send_wr*)malloc(sizeof(ibv_send_wr));
		e=get_cycles();
		v= (e-s) / mhz * 1000;
		mean1+=v;
		max1= max1>v?max1:v;
		min1= min1<v?min1:v;
		
		mhz = get_cpu_mhz(0);
		s=get_cycles();
		free(p);
		p=nullptr;
		e=get_cycles();
		v= (e-s) / mhz * 1000;
		mean2+=v;
		max2= max2>v?max2:v;
		min2= min2<v?min2:v;
	}
	std::cout<< " malloc, mean: " << mean1 /100 <<"  ,max:  " << max1 << "  ,min:  " << min1 <<std::endl;
	std::cout<< " free, mean: " << mean2 /100 <<"  ,max:  " << max2 << "  ,min:  " << min2 <<std::endl;

}


void fun1()
{
	double mean1 = 0,
	       mean2 = 0,
	       max1  = 0,
	       max2  = 0,
	       min1  =  10000000,
	       min2  =  10000000,
	       mhz   =  0,
		v    =  0;
	ibv_send_wr* p = 0;
	cycles_t s,e;
	for(int i = 0; i < 100;++i)
	{
		mhz = get_cpu_mhz(0);
		s=get_cycles();
		p=new ibv_send_wr;
		e=get_cycles();
		v= (e-s) / mhz * 1000;
		mean1+=v;
		max1= max1>v?max1:v;
		min1= min1<v?min1:v;
		
		mhz = get_cpu_mhz(0);
		s=get_cycles();
		delete p;
		p=nullptr;
		e=get_cycles();
		v= (e-s) / mhz * 1000;
		mean2+=v;
		max2= max2>v?max2:v;
		min2= min2<v?min2:v;
	}
	std::cout<< " new, mean: " << mean1 /100 <<"  ,max:  " << max1 << "  ,min:  " << min1 <<std::endl;
	std::cout<< " delete, mean: " << mean2 /100 <<"  ,max:  " << max2 << "  ,min:  " << min2 <<std::endl;

}






int main()
{
	fun();
	fun1();
}

