#include"../trans_args.hpp"
#include"../get_clock.hpp"
#include"../wr_pool.hpp"
#include"../queue.hpp"
#include<thread>
#include<cstring>
std::atomic<bool> start(true);
int main(void)
{

	rfts::trans_args tsas(10000000, 10000000, 1, 8 ,10, 5, 0.2, 0.2,1,2,2);
	std::cout << tsas << std::endl;
	
	rfts::wr_pool<ibv_send_wr> wrpool(tsas);

	std::cout << "mr addr: " << wrpool.get_mrs_addr() << std::endl;
	std::cout << "mr length: " << wrpool.get_mrs_length() << std::endl;
	std::cout << "wr addr: " << wrpool.get_wrs_addr() << std::endl;
	std::cout << "wr length: " << wrpool.get_wrs_length() << std::endl;
	std::cout << "sg addr: " << wrpool.get_sgs_addr() << std::endl;
	std::cout << "sg length: " << wrpool.get_sgs_length() << std::endl;
	std::cout << "rq addr: " << wrpool.get_rq_addr() << std::endl;
	std::cout << "rq length: " << wrpool.get_rq_length() << std::endl;
#ifdef RFTS_MPSC
	std::cout << "flags addr: " << wrpool.get_flags_addr() << std::endl;
	std::cout << "flags length: " << wrpool.get_flags_length() << std::endl;
#endif /* #ifdef  define */

	rfts::queue<ibv_send_wr*> nosend(tsas);

	return 0;
}

