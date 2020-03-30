#include"../get_clock_ns.hpp"
#include<iostream>
#include<thread>
void fun1()
{
	cycles_t s,e;
	int b =0;
	double khz=get_cpu_ghz();
	for(int n=0; n<20;n++)
	{
		s=get_cycles();
		for(int i =0;  i< 1000;i++)
			b= i%10;
		e=get_cycles();
		std::cout<<"get_cpu: "<< (e-s)/3 << std::endl;
	}
}
void fun2()
{
	timespec s={0,0},e={0,0};
	int b =0;
	for(int n=0; n<20;n++)
	{
		clock_gettime(CLOCK_REALTIME,&s);
		for(int i =0;  i< 1000;i++)
			b= i%10;
		clock_gettime(CLOCK_REALTIME,&e);
		std::cout<<"clock_gettime:"<< (e.tv_sec-s.tv_sec)* 1000000000+ e.tv_nsec-s.tv_nsec << std::endl;
	}
}

void fun3()
{
	timespec s={0,0},e={0,0},b;
	for(int n=0; n<20;n++)
	{
		clock_gettime(CLOCK_REALTIME,&s);
		clock_gettime(CLOCK_REALTIME,&b);
		clock_gettime(CLOCK_REALTIME,&e);
		std::cout<<"clock_gettime overhead clock_gettime:"<< (e.tv_sec-s.tv_sec)* 1000000000+ e.tv_nsec-s.tv_nsec << std::endl;
	}
}
void fun4()
{
	cycles_t s,e;
	timespec b;
	double khz=get_cpu_ghz();
	for(int n=0; n<20;n++)
	{
		s=get_cycles();
		clock_gettime(CLOCK_REALTIME,&b);
		e=get_cycles();
		std::cout<<"get_cpu_khz override clock_gettime: "<< (e-s)/3 << std::endl;
	}
}
void fun5()
{
	timespec s={0,0},e={0,0};
	uint64_t b;
	for(int n=0; n<20;n++)
	{
		clock_gettime(CLOCK_REALTIME,&s);
		b=get_cycles();
		clock_gettime(CLOCK_REALTIME,&e);
		std::cout<<"clock_gettime overhead get_cycles:"<< (e.tv_sec-s.tv_sec)* 1000000000+ e.tv_nsec-s.tv_nsec << std::endl;
	}
}
void fun6()
{
	cycles_t s,e;
	int b =0;
	double khz=get_cpu_ghz();
	for(int n=0; n<20;n++)
	{
		s=get_cycles();
		b =get_cycles();
		e=get_cycles();
		std::cout<<"get_cpu_khz overhead get_cycles: "<< (e-s)/3 << std::endl;
	}
}

int main()
{
	std::thread th1(fun1);
	std::thread th2(fun2);
	std::thread th3(fun3);
	std::thread th4(fun4);
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	std::thread th5(fun5);
	th5.join();
	std::thread th6(fun6);
	th6.join();
}
