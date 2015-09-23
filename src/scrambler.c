#include "../include/comman.h"





int main(void)
{
	uint8_t reset_status = 0x7f;
	uint8_t cur_status = 0x7f;
	uint8_t sequence_arr[127] = {0x00};

	int tmp_bit = 0;
	int var ;
	for (var = 0;;var++){
			if ((cur_status == reset_status) && (var != 0)){
				printf ("length:%d\n",var);
				break;
			}
			sequence_arr[var] = cur_status;
			tmp_bit = GetBit(cur_status, 0) ^ GetBit(cur_status, 1)	;
			cur_status = cur_status>>1;
			tmp_bit ? SetBit(cur_status, 6): ClearBit(cur_status, 6);	
	}

#ifdef _DEBUG 
	for (var = 0; var < sizeof(sequence_arr); var++){
		if (var % 16 == 0)
				printf ("\n");
		printf ("[0x%02x] ", sequence_arr[var]);
	}
	printf ("\n");
#endif

	int loop_count = 0;
	uint8_t test_code = 12;
	int loop ;
	for (loop = 7; loop >= 0; loop-- ){
		(GetBit(test_code, loop) ^ GetBit(sequence_arr[loop_count++], 0))? SetBit( test_code, loop):ClearBit( test_code, loop);	
		if (loop_count == 127)
				loop_count = 0;
	}
	printf ("screambler_num:%u\n", test_code);	
	
	loop_count = 0;
	for (loop = 7; loop >= 0; loop-- ){
		(GetBit(test_code, loop) ^ GetBit(sequence_arr[loop_count++], 0))? SetBit( test_code, loop):ClearBit( test_code, loop);	
		if (loop_count == 127)
				loop_count = 0;
	}

	printf ("de-screambler_num:%u\n", test_code);	
	return 0;
}
