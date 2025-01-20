#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }
    if(strcmp(argv[1], "-e") == e_success)
    {
        if(argc < 4)
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
            return e_failure;
        }
	const char *bmp = strrchr(argv[2],'.');

	if(bmp == NULL ||strcmp(bmp,".bmp") != 0)
	{
	    fprintf(stderr,"Error %s is not a valid .bmp file. Please enter valid .bmp file\n",argv[2]);
	    return e_failure;
	}

        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            if (do_encoding(&encInfo) == e_success)
            {
                printf("## ENCODING DONE SUCCESSFULLY ##\n");
            }
            else
            {
                fprintf(stderr, "Error: %s function failed\n", "do_encoding()");
                return e_failure;
            }
        }
        else
        {
            fprintf(stderr, "Error: %s function failed\n", "read_and_validate_encode_args()");
            return e_failure;
        }
    }
    else if(strcmp(argv[1], "-d") == e_success)
    {
        if(argc < 3)
        {
            printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
            return e_failure;
        }
	const char *de_bmp = strrchr(argv[2],'.');
	if(de_bmp == NULL || strcmp(de_bmp,".bmp") != 0)
	{
	   fprintf(stderr,"Error %s is not valid .bmp file. Please enter valid .bmp file\n",argv[2]);
	   return e_failure;
	}
        DecodeInfo decInfo;
        if(read_and_validate_decode_args(&decInfo, argc, argv) == e_success)
        {
            if(do_decoding(&decInfo) == e_success)
            {
                printf("## DECODING DONE SUCCESSFULLY ##\n");
            }
            else
            {
                fprintf(stderr, "Error: %s function failed\n", "do_decoding()");
                return e_failure;
            }
        }
        else
        {
            fprintf(stderr, "Error: %s function failed\n", "read_and_validate_decode_args()");
            return e_failure;
        }
    }
    else
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }
    return 0;
}
    /* Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";

    // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
        printf("ERROR: %s function failed\n", "open_files" );
        return 1;
    }
    else
    {
        printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);*/
