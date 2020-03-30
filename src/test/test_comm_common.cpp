#include"../comm_common.hpp"
#include<iostream>

void print_id(rfts::cm_id& cm_id1)
{
	std::cout << cm_id1.get_rdma_cm_id()->verbs << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->channel << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->context << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->qp << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->ps << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->port_num << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->event << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->send_cq_channel << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->send_cq << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->recv_cq_channel << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->recv_cq << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->srq << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->pd << std::endl;
	std::cout << cm_id1.get_rdma_cm_id()->qp_type << std::endl;
}





int main()
{
	rfts::cm_event_channel cm_ch;

	rfts::cm_id cm_id1(cm_ch, nullptr ,RDMA_PS_TCP);
	rfts::cm_id cm_id2(cm_ch, nullptr, RDMA_PS_UDP);
	rfts::cm_id cm_id3(cm_ch, nullptr, RDMA_PS_IPOIB);
	rfts::cm_id cm_id4(cm_ch, nullptr, RDMA_PS_IB);
	print_id(cm_id1);
	print_id(cm_id2);
	print_id(cm_id3);
	print_id(cm_id4);
}
