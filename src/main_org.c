#include "../include/comman.h"


//#define _SHOW_SCREAMBLER_SEQUENCE

//#define _SHOW_DIRECTFILE
//#define _DESCREAMBLER
#define _SHOW_FREAMER_ARRAY

int scrambler_get_sequence ( uint8_t *sequence_arr );

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

	



#ifdef _DESCREAMBLER
    /* de-screamble */
    printf ( "readly to descreambler\n" );

    uint8_t screamble_sequnce[127] = {0x00};

    scrambler_get_sequence ( screamble_sequnce );
    printf ( "Get screamble_sequence is oK\n" );

    int i = 1;

    while ( i-- )
    {
        uint8_t loop_count = 0;
        int  loop_bit;

        for ( var_y = 0; var_y < 9; var_y++ )
        {
            for ( var_x = 0; var_x < 17280; var_x++ )
            {
                /* skip A1 A2 J0 Z0 total:576 */
                if ( ( var_y == 0 ) && ( var_x == 0 ) )
                {
                    var_x = 576;
                }

                for ( loop_bit = 7; loop_bit >= 0; loop_bit -- )
                {
                    ( GetBit ( framer_struct[var_x][var_y], loop_bit ) ^ GetBit ( screamble_sequnce[loop_count++], 0 ) ) \
                                ? SetBit ( framer_struct[var_x][var_y], loop_bit ) : ClearBit ( framer_struct[var_x][var_y], loop_bit );

                    if ( loop_count == 127 )
                    {
                        loop_count = 0;
                    }
                }
            }
        }
    }

#endif /*_DESCREAMBLER*/

#ifdef _SHOW_FREAMER_ARRAY
    printf ( "framer_num:%u\n", framer_num );

    for ( var_x = 0; var_x < 17280; var_x++ )
    {
        for ( var_y = 0; var_y < 9; var_y++ )
        {
            printf ( "[0x%02x] ", framer_struct[var_x][var_y] );
        }

        printf ( "\n" );
    }
#endif //_SHOW_FREAMER_ARRAY
    if ( framer_num == 1 )
    {
        goto NEXT;
    }
END:
    fclose ( fp );

    return 0;
}



int scrambler_get_sequence ( uint8_t *sequence_arr )
{
    uint8_t reset_status = 0x7f;
    uint8_t cur_status = 0x7f;

    int tmp_bit = 0;
    int var ;

    for ( var = 0;; var++ )
    {
        if ( ( cur_status == reset_status ) && ( var != 0 ) )
        {
            printf ( "length:%d\n", var );
            break;
        }

        sequence_arr[var] = cur_status;
        tmp_bit = GetBit ( cur_status, 0 ) ^ GetBit ( cur_status, 1 ) ;
        cur_status = cur_status >> 1;
        tmp_bit ? SetBit ( cur_status, 6 ) : ClearBit ( cur_status, 6 );
    }

#ifdef _SHOW_SCREAMBLER_SEQUENCE

    for ( var = 0; var < sizeof ( sequence_arr ); var++ )
    {
        if ( var % 16 == 0 )
            printf ( "\n" );

        printf ( "[0x%02x] ", sequence_arr[var] );
    }

    printf ( "\n" );
#endif


    return 0;
}
