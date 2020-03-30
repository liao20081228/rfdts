#pragma once
#ifndef HPP_RFTS_MAGIC_MACRO_HPP
	#define HPP_RFTS_MAGIC_MACRO_HPP
#include<cerrno>
#include<cstdio>
#include<cstdlib>


namespace rfts
{
	const unsigned int RFTS_LATENCY = 100;
	const unsigned int ELEMENT_SIZE_SCALE = 2;
	const unsigned int ELEMENT_NUM_SCALE = 2;
	const unsigned int GET_TRY_NUM = 100;
	const unsigned int GET_WAIT_TIME = 1000; //ns
	const unsigned int WR_NUM_SGE = 1;
}
#endif /* end of include guard: HPP_RFTS_MAGIC_HPP */
