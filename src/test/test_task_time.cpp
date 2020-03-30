#include<stdio.h>
#include"../get_clock.hpp"
#include<stdlib.h>
#include<time.h>
#include"../trans_args.hpp"
int main()
{
	srand(time(NULL));
	rfts::data a[8];
	for(int i = 0; i < 4;i++)
	{	
		a[i].nodeid=3;
		clock_gettime(CLOCK_REALTIME, &a[i].time);
		a[i].number=i;
		a[i].type=1;
		for(int m=1;i<9;i++)
		{
			a[i].sensorid=m;
			a[i].value=rand()/(double)RAND_MAX+1;
		}
	}
	for(int i = 4; i < 8;i++)
	{	
		a[i].nodeid=3;
		clock_gettime(CLOCK_REALTIME, &a[i].time);
		a[i].number=i;
		a[i].type=2;
		for(int m=0;i<7;i++)
		{
			a[i].sensorid=m;
			a[i].value=3.123456+m;
		}
	}
	double arg=0;
	double mhz=0;
	cycles_t s,e;
	for(int k=0;k<100;k++)
	{
		double arg_500_1=0;
		double arg_500_2=0;
		double arg_500_3=0;
		double arg_500_4=0;
		double arg_500_5=0;
		double arg_500_6=0;
		double arg_500_7=0;
		double arg_500_8=0;
		double arg_10_1=0;
		double arg_10_2=0;
		double arg_10_3=0;
		double arg_10_4=0;
		double arg_10_5=0;
		double arg_10_6=0;
		double arg_10_7=0;
		double arg_10_8=0;
		mhz= get_cpu_mhz(0);
		s=get_cycles();
		for(int i=0; i<8;i++)
		{
			switch(a[i].nodeid)
			{
				case 1:
					switch(a[i].type)
					{
						case 1:
							switch(a[i].sensorid)
							{
								case 1:
									arg_500_1+=a[i].value;
									break;
								case 2:
									arg_500_2+=a[i].value;
									break;
								case 3:
									arg_500_3+=a[i].value;
									break;
								case 4:
									arg_500_4+=a[i].value;
									break;
								case 5:
									arg_500_5+=a[i].value;
									break;
								case 6:
									arg_500_6+=a[i].value;
									break;
								case 7:
									arg_500_7+=a[i].value;
									break;
								case 8:
									arg_500_8+=a[i].value;
									break;
								default:break;
							}
							break;
						case 2:
							switch(a[i].sensorid)
							{
								case 1:
									arg_10_1+=a[i].value;
									break;
								case 2:
									arg_10_2+=a[i].value;
									break;
								case 3:
									arg_10_3+=a[i].value;
									break;
								case 4:
									arg_10_4+=a[i].value;
									break;
								case 5:
									arg_10_5+=a[i].value;
									break;
								case 6:
									arg_10_6+=a[i].value;
									break;
								case 7:
									arg_10_7+=a[i].value;
									break;
								case 8:
									arg_10_8+=a[i].value;
									break;
								default:break;
							}
							break;
						default:break;
					}
					break;
				case 2:
					switch(a[i].type)
					{
						case 1:
							switch(a[i].sensorid)
							{
								case 1:
									arg_500_1+=a[i].value;
									break;
								case 2:
									arg_500_2+=a[i].value;
									break;
								case 3:
									arg_500_3+=a[i].value;
									break;
								case 4:
									arg_500_4+=a[i].value;
									break;
								case 5:
									arg_500_5+=a[i].value;
									break;
								case 6:
									arg_500_6+=a[i].value;
									break;
								case 7:
									arg_500_7+=a[i].value;
									break;
								case 8:
									arg_500_8+=a[i].value;
									break;
								default:break;
							}
							break;
						case 2:
							switch(a[i].sensorid)
							{
								case 1:
									arg_10_1+=a[i].value;
									break;
								case 2:
									arg_10_2+=a[i].value;
									break;
								case 3:
									arg_10_3+=a[i].value;
									break;
								case 4:
									arg_10_4+=a[i].value;
									break;
								case 5:
									arg_10_5+=a[i].value;
									break;
								case 6:
									arg_10_6+=a[i].value;
									break;
								case 7:
									arg_10_7+=a[i].value;
									break;
								case 8:
									arg_10_8+=a[i].value;
									break;
								default:break;
							}
							break;
						default:break;
					}
					break;
				case 3:
					switch(a[i].type)
					{
						case 1:
							switch(a[i].sensorid)
							{
								case 1:
									/*arg_500_1+=a[i].value;*/
									break;
								case 2:
									/*arg_500_2+=a[i].value;*/
									break;
								case 3:
									/*arg_500_3+=a[i].value;*/
									break;
								case 4:
									/*arg_500_4+=a[i].value;*/
									break;
								case 5:
									/*arg_500_5+=a[i].value;*/
									break;
								case 6:
									/*arg_500_6+=a[i].value;*/
									break;
								case 7:
									/*arg_500_7+=a[i].value;*/
									break;
								case 8:
									/*arg_500_8+=a[i].value;*/
									break;
								default:break;
							}
							break;
						case 2:
							switch(a[i].sensorid)
							{
								case 1:
									/*arg_10_1+=a[i].value;*/
									break;
								case 2:
									/*arg_10_2+=a[i].value;*/
									break;
								case 3:
									/*arg_10_3+=a[i].value;*/
									break;
								case 4:
									/*arg_10_4+=a[i].value;*/
									break;
								case 5:
									/*arg_10_5+=a[i].value;*/
									break;
								case 6:
									/*arg_10_6+=a[i].value;*/
									break;
								case 7:
									/*arg_10_7+=a[i].value;*/
									break;
								case 8:
									/*arg_10_8+=a[i].value;*/
									break;
								default:break;
							}
							break;
						default:break;
					}
					break;
				default:
					break;
			}
		}
		printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",arg_500_1/500,arg_500_2/500,arg_500_3/500,arg_500_4/500,arg_500_5/500,arg_500_6/500,arg_500_7/500,arg_500_8/500);
		printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",arg_10_1/80,arg_10_2/80,arg_10_3/80,arg_10_4/80,arg_10_5/80,arg_10_6/80,arg_10_7/80,arg_10_8/80);
		e=get_cycles();
		arg += (e-s)/mhz*100;
	}
	printf("========     %lf=============\n",arg/100);
}
