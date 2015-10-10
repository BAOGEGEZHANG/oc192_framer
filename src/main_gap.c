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
//            printf("[0x%02x] ",  hex_word);
            org_buff[word_pos++] =  hex_word;
        }

//          printf ("\n");
    }

		uint64_t word_cnt ;
		word_cnt = word_pos ;	
	
#if 1
		uint64_t register_rol = -1;
		uint64_t tmp_loop_register;

		for (tmp_loop_register = 0; tmp_loop_register < 6; tmp_loop_register++){
			register_rol = ClearBit64(register_rol, 63 - tmp_loop_register);			
		}
		int tmp_loop_bits;
		uint64_t tmp_bit;	

		word_pos = 0;
		for (word_pos = 0; word_pos < word_cnt; word_pos++){
			for (tmp_loop_bits = 7; tmp_loop_bits >= 0 ; tmp_loop_bits--){
				tmp_bit = GetBit64(org_buff[word_pos], tmp_loop_bits) ;
				tmp_bit ^ (GetBit64(register_rol, 38) ^ GetBit64(register_rol, 57)) ? SetBit(org_buff[word_pos], tmp_loop_bits):ClearBit(org_buff[word_pos], tmp_loop_bits) ;	
				register_rol = 	register_rol << 1;
				register_rol = ClearBit64(  register_rol, 58);	
				register_rol = (tmp_bit ? SetBit64(register_rol, 0): ClearBit64(register_rol, 0));
			}
		}
#endif
		for (word_pos = 0; word_pos < word_cnt; word_pos++){
				if (word_pos % 16 == 0)
							printf ("\n");
				printf ("[0x%02x] ", org_buff[word_pos]);
			
		}

							printf ("\n");


    fclose ( fp );

    return 0;
}

