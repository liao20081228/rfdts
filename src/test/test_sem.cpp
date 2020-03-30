#include"../sem.hpp"
#include<thread>
#include<iostream>
#include<unistd.h>
#include<time.h>
void fun1(rfts::posix_sem& ref)
{
	sleep(10);
	ref.post();

}


void fun2(rfts::posix_sem& ref)
{
	ref.wait();
	std::cout << "wait" << std::endl;
}


void fun3(rfts::posix_sem& ref)
{

	if(EAGAIN == ref.trywait())
	{
		std::cout << "try again" << std::endl;
	}
	struct timespec timeout=
	{
		.tv_sec = time(nullptr)+5,
		.tv_nsec = 0
	};
	if(ETIMEDOUT == ref.timewait(&timeout))
	{
		std::cout << "timeout" << std::endl;
	}
}

int main()
{
	rfts::posix_sem sem1;
	rfts::posix_sem sem4("/sem1", O_RDWR|O_CREAT, REGULAR_FILE_MODE);
	std::thread th1(fun1, std::ref(sem1));
	std::thread th2(fun2, std::ref(sem1));
	std::thread th3(fun3, std::ref(sem1));
	th1.join();
	th2.join();
	th3.join();
}



