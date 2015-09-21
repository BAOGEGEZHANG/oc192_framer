#include "../include/comman.h"

uint8_t framer_start_flag = 0xf6;

int main(void)
{
	FILE *fp ;
	fp = fopen( "../tmp/rel.tmp", "rw+");

	if (NULL == fp){
		printf ("[main]:fopen file:../tmp/rel.tmp failed\n");
		exit(-1);
	}

	uint8_t tmp_line_buff[128] = {0x00};
	uint8_t org_buff[2*1024*1024] = {0x00};
	uint64_t word_pos = 0;
	
	
	uint8_t tmp_word_buff[8] = {0x00};
	uint8_t hex_word;

/* translate text_mode to hex */
	while(fgets( tmp_line_buff, 128, fp)){
//		fgets( tmp_line_buff, 128, fp);
		uint8_t tmp_count = 0 ;

		while(tmp_count < 8){
						strncpy(tmp_word_buff, tmp_line_buff + tmp_count * 8, 8);
					//	puts(tmp_word_buff);

						tmp_count++;
						hex_word = strtol( tmp_word_buff, NULL, 2);
						printf("[0x%02x] ",  hex_word);
						org_buff[word_pos++] =  hex_word;
		}
		printf ("\n");
	}

	word_pos = 0;	
	uint64_t sum_count = 0;
	while(1){
					if(org_buff[word_pos++] == framer_start_flag )	{
									sum_count ++;
									continue;
					}

					if (word_pos > sizeof(org_buff)){
									printf ("Not found framer flag\n");
									break;
					}else if (sum_count == 192){
						printf ("line:%lu col:%lu\n", (word_pos - 192)/8, (word_pos - 192) %8);	
						break;
				}

				sum_count = 0;
	}

	fclose(fp);

	return 0;
}
