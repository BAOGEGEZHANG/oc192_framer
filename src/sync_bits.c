#include "../include/comman.h"



#define PAYLOAD_BYTES  (17280 - 640) * 9


uint8_t framer_start_flag = 0xf6;

int main ( void )
{
    FILE *fp ;
    fp = fopen ( "../tmp/rel.tmp", "rw+" );

    if ( NULL == fp )
    {
        printf ( "[main]:fopen file:../tmp/rel.tmp failed\n" );
        exit ( -1 );
    }

    uint8_t tmp_line_buff[128] = {0x00};
    uint8_t org_buff[2 * 1024 * 1024] = {0x00};
    uint64_t word_pos = 0;

    uint8_t tmp_word_buff[8] = {0x00};
    uint8_t hex_word;

    /* translate text_mode to hex */
    while ( fgets ( tmp_line_buff, 128, fp ) )
    {
        uint8_t tmp_count = 0 ;

        while ( tmp_count < 8 )
        {
            strncpy ( tmp_word_buff, tmp_line_buff + tmp_count * 8, 8 );

            tmp_count++;
            hex_word = strtol ( tmp_word_buff, NULL, 2 );
#ifdef _SHOW_DIRECTFILE
            printf("[0x%02x] ",  hex_word);
#endif //_SHOW_DIRECTFILE
            org_buff[word_pos++] =  hex_word;
        }

#ifdef _SHOW_DIRECTFILE
          printf ("\n");
#endif //_SHOW_DIRECTFILE
    }

    uint8_t framer_num = 0;

    /* found framer_start_flag  */
    word_pos = 0;

NEXT:
    framer_num++;
    word_pos = word_pos;
    uint64_t sum_count = 0;
    uint64_t start_pos = 0;

    while ( 1 )
    {
        if ( org_buff[word_pos++] == framer_start_flag )
        {
            if ( sum_count == 0 )
            {
                start_pos = word_pos - 1;
            }

            sum_count ++;
            continue;
        }

        if ( word_pos > sizeof ( org_buff ) )
        {
            printf ( "Not found framer flag\n" );
            break;
        }
        else if ( sum_count == 192 )
        {
            printf ( "found flag_framer_start:line:%lu col:%lu\n", start_pos / 8 + 1, start_pos % 8 );
            break;
        }

        sum_count = 0;
    }

    word_pos = start_pos;
    /* format_seqnum */
    uint8_t framer_struct[17280][9] = {0x00};
    uint64_t var_x, var_y;

    for ( var_y = 0; var_y < 9; var_y++ )
    {
        for ( var_x = 0; var_x < 17280; var_x ++ )
        {
            framer_struct[var_x][var_y] = org_buff[word_pos++];
        }
    }
	/* format payload */
	uint64_t payload_var_x = 640;
	uint8_t payload_struct[PAYLOAD_BYTES] = {0x00};
	word_pos = 0;
		
	for (var_y = 0; var_y < 9; var_y++){
		for (var_x = payload_var_x; var_x < 17280; var_x ++){
			payload_struct[word_pos++] = framer_struct[var_x][var_y];	
		}
	}		

#if 0
	for (var_x = 0; var_x < PAYLOAD_BYTES; var_x ++){
		if (var_x % 16 == 0){
			printf ("\n");
		}
		printf ("[0x%02x] ",payload_struct[var_x]);
	}

	printf ("\n");
#endif

	uint8_t tmp_byte;
	uint8_t tmp_bit_1, tmp_bit_2;
	uint8_t sync_flag_sum = 0;

	uint64_t start_sync_flag_pos = 0;
	uint64_t tmp_byte_num , tmp_bit_num;

	uint64_t flag_start_pos = 0;

	while(1){
		tmp_byte_num = start_sync_flag_pos / 8;
		tmp_bit_num = 7 - start_sync_flag_pos % 8;	

		tmp_byte = payload_struct[tmp_byte_num];
		tmp_bit_1 =  GetBit(tmp_byte, tmp_bit_num);

		if (!tmp_bit_num){
			tmp_byte = payload_struct[tmp_byte_num + 1];
			tmp_bit_2 = GetBit(tmp_byte, 7);
		}else
		{
			tmp_bit_2 = GetBit(tmp_byte, tmp_bit_num - 1);
		}

//		if (tmp_bit_1 ^ tmp_bit_2)	
		if (tmp_bit_1 == 0 &&   tmp_bit_2 == 1)	
		{
				if (!sync_flag_sum){
					flag_start_pos = start_sync_flag_pos;
				}
				start_sync_flag_pos += 66;	
				sync_flag_sum ++;
				if (sync_flag_sum == 64){
						printf ("found flag_start_pos :%lu\n", flag_start_pos);
						break;
				}
		}else
		{
			start_sync_flag_pos ++;
			sync_flag_sum = 0;
		}
	}

//	for (var_x = 0; var_x < 16; var_x ++){
		while(1){
		tmp_byte_num = flag_start_pos / 8;
		tmp_bit_num = 7 - flag_start_pos % 8;	
		if (tmp_byte_num >= PAYLOAD_BYTES){
			break;
		}

		tmp_byte = payload_struct[tmp_byte_num];
		tmp_bit_1 =  GetBit(tmp_byte, tmp_bit_num);
		
		if (!tmp_bit_num){
			tmp_byte = payload_struct[tmp_byte_num + 1];
			tmp_bit_2 = GetBit(tmp_byte, 7);
		}else
		{
			tmp_bit_2 = GetBit(tmp_byte, tmp_bit_num - 1);
		}

		printf ("%d%d,",tmp_bit_1, tmp_bit_2);
		for (var_y = 0; var_y < 64; var_y ++){
				tmp_byte_num = (flag_start_pos + 2 + var_y) / 8 ;	
				tmp_bit_num =  7 - (flag_start_pos + 2 + var_y) % 8;
				tmp_byte = payload_struct[tmp_byte_num];
				printf ("%d", GetBit(tmp_byte, tmp_bit_num));
		}
		printf ("\n");

		flag_start_pos += 66;
	}

    if ( framer_num == 1 )
    {
				printf ("framer_num next\n");
        goto NEXT;
    }
END:
    fclose ( fp );

    return 0;
}

