#include <stdio.h>

#define ADC_DEVICE "/dev/my_adc"

int main()
{
	int ret;
	unsigned int data[2];
	ret=open(ADC_DEVICE,0);
    if(ret<0) {
        printf("Open adc fail/n");
        return ret;
    }
    for(;;) {
        //printf("cnt=%d/n",cnt);
	     read(ret,data,sizeof(data));
	     printf("The value is %d\t%d\n",data[0],data[1]);
	}
	close(ret);

	return 0;
}
