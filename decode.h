/*------------------------------------------------------------------
NAME 		:D.Venkat
DESCRIPTION	:STEGANOGRAPHY
DATE		:28/08/2024

--------------------------------------------------------------------
 */
#ifndef DECODE_H
#define DECODE_H
#include "types.h" 
#include "encode.h"
//#include "common.c"


typedef struct _DecodeInfo 
{
    char *stego_image_fname;
    FILE *fptr_stego_image;
    
    char *output_file_fname;
    FILE *fptr_output;
   
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    char magic_string[20];
    char extn_output_file[MAX_FILE_SUFFIX];
    char extn_secret_file[20];

    //long magic_string_size;

} DecodeInfo;

/*Decoding Function Prototypes */

/* Get file pointers for i/p and o/p files */
Status do_open_files(DecodeInfo *decInfo);
/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(DecodeInfo *decInfo, int argc, char *argv[]);
/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);
/* Decode magic string size */
Status decode_magic_string_size(DecodeInfo *decInfo, long *magic_string_size);
/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo,long magic_string_size);
/* Decode size from lsb */
Status decode_size_from_lsb(long *data, char *image_buffer);
/* Decode byte from lsb */
Status decode_byte_from_lsb(char *data, char *image_buffer);
/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo, long *extn_size);
/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo, long extn_size);
/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo,long *secret_file_size);
/* Deocode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo, long secret_file_size);

#endif
