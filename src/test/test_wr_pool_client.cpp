#include"../get_clock_ns.hpp"
#include"../wr_pool.hpp"
#include<thread>
#include<cstring>
#include<array>
int main()
{

	rfts::trans_args tsas;
	tsas.afreq = 1;
	tsas.tfreq = 3;
	tsas.kind_per_sensor = 1;
	tsas.sensor_num_per_node = 1;
	tsas.node_num =1;
	tsas.ele_size_scale = 1;
	tsas.ele_num_scale = 1;
	tsas.sys_lat = 1000000;
	
	ibv_device** dev_list = ibv_get_device_list(nullptr);
	ibv_context* context = ibv_open_device(dev_list[0]);
	ibv_free_device_list(dev_list);
	ibv_pd* pd = ibv_alloc_pd(context);
	
	FILE* fp = fopen("./test_wr_pool_client.txt", "w+");

	double sum_ghz = 0,max_ghz = 0, min_ghz = 1000, mean_ghz=0;
	
	double temp;
	int samplenum=102;
	for (int i = 0;   i< 102; ++i)
	{
		temp = get_cpu_ghz();
		if(!temp)
		{
			samplenum--;
			continue;
		}
		sum_ghz+=temp;
		max_ghz = temp > max_ghz ? temp : max_ghz;
		min_ghz = temp < min_ghz ? temp : min_ghz;
	}
	mean_ghz = (sum_ghz - max_ghz - min_ghz) / (samplenum-2);

	fprintf(fp,"validated samplenum:%d, cpu mean_ghz:%lf, max_ghz:%lf, min_ghz:%lf\n",samplenum, mean_ghz, max_ghz, min_ghz);
//----------------------------------------------
	std::array<double, 33>  sum_get, max_get, min_get,
				sum_put, max_put, min_put,
				sum_reg, max_reg, min_reg,
				sum_dre, max_dre, min_dre,
				sum_cre, max_cre, min_cre,
				sum_des, max_des, min_des;
	sum_get.fill(0); sum_put.fill(0); sum_reg.fill(0);
	sum_dre.fill(0); sum_cre.fill(0); sum_des.fill(0);
	max_get.fill(0); max_put.fill(0); max_reg.fill(0);
	max_dre.fill(0); max_cre.fill(0); max_des.fill(0);
	min_get.fill(10000000000); min_put.fill(10000000000); min_reg.fill(10000000000);
	min_dre.fill(10000000000); min_cre.fill(10000000000); min_des.fill(10000000000);
	int testnum = 1002;
for(int j=0; j< 33;j++ )
{	
	volatile cycles_t s,e;

	ibv_send_wr* wr = nullptr;
	ibv_mr*	mr = nullptr;
	char*	addr = nullptr;
	ibv_sge* sg = nullptr;
	
	double elesize = pow(2,j);
	tsas.size_per_data = elesize;
	rfts::wr_pool<ibv_send_wr> wrpool(tsas);
	
	mr=ibv_reg_mr(pd, wrpool.get_mrs_addr(), wrpool.get_mrs_length(), IBV_ACCESS_LOCAL_WRITE);
	mlock(wrpool.get_sgs_addr(), wrpool.get_sgs_length());
	mlock(wrpool.get_wrs_addr(), wrpool.get_wrs_length());
	mlock(wrpool.get_rq_addr(), wrpool.get_rq_length());
	for(int i = 0; i < testnum; ++i)
	{
		s=e=0;
		s=get_cycles();
		wr = wrpool.get();
		e=get_cycles();
		temp = e-s;
		max_get[j] = temp > max_get[j] ? temp : max_get[j];
		min_get[j] = temp < min_get[j] ? temp : min_get[j];
		sum_get[j] += temp;
		//printf("allocate wr address:%p , mr address:%p\n", wr, reinterpret_cast<void*>(wr->sg_list->addr));
		
		s=e=0;
		s=get_cycles();
		wrpool.put(wr);
		e=get_cycles();
		temp = e-s;
		max_put[j] = temp > max_put[j] ? temp : max_put[j];
		min_put[j] = temp < min_put[j] ? temp : min_put[j];
		sum_put[j] += temp;
		//printf("%15lf,%15lf,%15lf\n", sum_get[j], max_get[j],min_get[j]);
	}
	munlock(wrpool.get_sgs_addr(), wrpool.get_sgs_length());
	munlock(wrpool.get_wrs_addr(), wrpool.get_wrs_length());
	munlock(wrpool.get_rq_addr(), wrpool.get_rq_length());
	ibv_dereg_mr(mr);
	for(int i = 0; i < testnum; ++i)
	{
		s=e=0;
		s=get_cycles();
		addr = new char [static_cast<uint64_t>(elesize)];
		mr = ibv_reg_mr(pd, addr, elesize, IBV_ACCESS_LOCAL_WRITE);
		e=get_cycles();
		temp = e-s;
		max_reg[j] = temp > max_reg[j] ? temp : max_reg[j];
		min_reg[j] = temp < min_reg[j] ? temp : min_reg[j];
		sum_reg[j] += temp;
		
		s=e=0;
		s=get_cycles();
		wr = new ibv_send_wr();
		memset(wr, 0 , sizeof(ibv_send_wr));
		sg = new ibv_sge;
		sg->addr = reinterpret_cast<uint64_t>(addr);
		sg->length = mr->length;
		sg->lkey =mr->lkey;
		wr->next = nullptr;
		wr->sg_list = sg;
		wr->num_sge = 1;
		wr->wr_id = 1;
		wr->opcode = IBV_WR_SEND;
		wr->send_flags = IBV_SEND_SIGNALED|IBV_SEND_INLINE;
		wr->wr.ud.ah = nullptr;
		wr->wr.ud.remote_qkey = 111111;
		wr->wr.ud.remote_qpn  = 111111;
		e=get_cycles();
		temp = e-s;
		max_cre[j] = temp > max_cre[j] ? temp : max_cre[j];
		min_cre[j] = temp < min_cre[j] ? temp : min_cre[j];
		sum_cre[j] += temp;
		
		s=e=0;
		s=get_cycles();
		delete wr;
		wr = nullptr;
		delete sg;
		sg = nullptr;
		e=get_cycles();
		temp = e-s;
		max_des[j] = temp > max_des[j] ? temp : max_des[j];
		min_des[j] = temp < min_des[j] ? temp : min_des[j];
		sum_des[j] += temp;


		s=e=0;
		s=get_cycles();
		ibv_dereg_mr(mr);
		delete[] addr;
		e=get_cycles();
		temp = e-s;
		max_dre[j] = temp > max_dre[j] ? temp : max_dre[j];
		min_dre[j] = temp < min_dre[j] ? temp : min_dre[j];
		sum_dre[j] += temp;
	}
}
fprintf(fp,"%6s,%18s,%18s,%18s,%18s,%18s,%18s\n","size","mean_get","max_get","min_get","mean_put","max_put","min_put");
for (int i = 0; i < 33; ++i)
{
fprintf(fp,"%6d,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf\n",
	i,(sum_get[i]-max_get[i]-min_get[i])/(testnum - 2)/mean_ghz,max_get[i]/mean_ghz,min_get[i]/mean_ghz,
	(sum_put[i]-max_put[i]-min_put[i])/(testnum - 2)/mean_ghz,max_put[i]/mean_ghz,min_put[i]/mean_ghz);
}

fprintf(fp,"\n%6s,%18s,%18s,%18s,%18s,%18s,%18s\n","size","mean_reg","max_reg","min_reg","mean_dre","max_dre","min_dre");
for (int i = 0; i < 33; ++i)
{
fprintf(fp,"%6d,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf\n",
	i,(sum_reg[i]-max_reg[i]-min_reg[i])/(testnum - 2)/mean_ghz,max_reg[i]/mean_ghz,min_reg[i]/mean_ghz,
	(sum_dre[i]-max_dre[i]-min_dre[i])/(testnum - 2)/mean_ghz,max_dre[i]/mean_ghz,min_dre[i]/mean_ghz);
}

fprintf(fp,"\n%6s,%18s,%18s,%18s,%18s,%18s,%18s\n","size","mean_cre","max_cre","min_cre","mean_des","max_des","min_des");
for (int i = 0; i < 33; ++i)
{
fprintf(fp,"%6d,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf\n",
	i,(sum_cre[i]-max_cre[i]-min_cre[i])/(testnum - 2)/mean_ghz,max_cre[i]/mean_ghz,min_cre[i]/mean_ghz,
	(sum_des[i]-max_des[i]-min_des[i])/(testnum - 2)/mean_ghz,max_des[i]/mean_ghz,min_des[i]/mean_ghz);
}


fprintf(fp,"\n%6s,%18s,%18s,%18s,%18s,%18s,%18s\n","size","mean_get","max_get","min_get","mean_put","max_put","min_put");
for (int i = 0; i < 33; ++i)
{
fprintf(fp,"%6d,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf\n",
	i,log2((sum_get[i]-max_get[i]-min_get[i])/(testnum - 2)/mean_ghz),log2(max_get[i]/mean_ghz),log2(min_get[i]/mean_ghz),
	log2((sum_put[i]-max_put[i]-min_put[i])/(testnum - 2)/mean_ghz),log2(max_put[i]/mean_ghz),log2(min_put[i]/mean_ghz));
}

fprintf(fp,"\n%6s,%18s,%18s,%18s,%18s,%18s,%18s\n","size","mean_reg","max_reg","min_reg","mean_dre","max_dre","min_dre");
for (int i = 0; i < 33; ++i)
{
fprintf(fp,"%6d,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf\n",
	i,log2((sum_reg[i]-max_reg[i]-min_reg[i])/(testnum - 2)/mean_ghz),log2(max_reg[i]/mean_ghz),log2(min_reg[i]/mean_ghz),
	log2((sum_dre[i]-max_dre[i]-min_dre[i])/(testnum - 2)/mean_ghz),log2(max_dre[i]/mean_ghz),log2(min_dre[i]/mean_ghz));
}

fprintf(fp,"\n%6s,%18s,%18s,%18s,%18s,%18s,%18s\n","size","mean_cre","max_cre","min_cre","mean_des","max_des","min_des");
for (int i = 0; i < 33; ++i)
{
fprintf(fp,"%6d,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf,%18.2lf\n",
	i,log2((sum_cre[i]-max_cre[i]-min_cre[i])/(testnum - 2)/mean_ghz),log2(max_cre[i]/mean_ghz),log2(min_cre[i]/mean_ghz),
	log2((sum_des[i]-max_des[i]-min_des[i])/(testnum - 2)/mean_ghz),log2(max_des[i]/mean_ghz),log2(min_des[i]/mean_ghz));
}

	ibv_dealloc_pd(pd);
	ibv_close_device(context);
	fclose(fp);
}
