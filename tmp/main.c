#include "../include/comman.h"




int main(void)
{

	uint64_t var = -1;

//	var = SetBit64(var, 63);
	var = ClearBit64(var, 63);
	printf ("%d\n", GetBit64(var, 63));
}
