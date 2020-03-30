#include"custom_shmem.hpp"

int main()
{
	pshmem a;
	char str[100] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	a.mwrite(str, 100, 80 );
	std::cout << (char*)a.getaddr() <<std::endl;
	
	a.mwrite("i love you",10,10 );
	std::cout << (char*)a.getaddr() <<std::endl;

	char ra[100] ={0};
	a.mseek(2,SEEK_SET);
	a.mread(ra,100,20,false);
	std::cout << ra <<std::endl;

}
