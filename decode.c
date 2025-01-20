#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"

Status do_open_files(DecodeInfo *decInfo)
{
    printf("INFO : Opened %s\n",decInfo->stego_image_fname);
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");
    if(decInfo->fptr_stego_image==NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->stego_image_fname);
        return e_failure;
    }
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    decInfo->fptr_output=fopen(decInfo->output_file_fname,"w");
    if(decInfo->fptr_output==NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->output_file_fname);
        return e_failure;
    }
    return e_success;
}
Status read_and_validate_decode_args(DecodeInfo *decInfo,int argc,char *argv[])
{
     if (argc < 2) 
     {
        fprintf(stderr, "ERROR : Enter atleast 2 arguments\n");
        return e_failure;
    }
    if(strstr(argv[2],".bmp") != NULL)
    {
        decInfo->stego_image_fname=argv[2];
    }
    else
    {
        fprintf(stderr,"ERROR: Input file is not a BMP file\n");
        return e_failure;
    }
    if(argv[3] != NULL)
    {
        decInfo->output_file_fname = argv[3];   
    }
    else
    {
	decInfo->output_file_fname = "default.txt";
    }
    return e_success;

}
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO : ## Decoding Procedure Started ##\n");
    printf("INFO : Open required files\n");
    if(do_open_files(decInfo) == e_success)
    {
        printf("INFO : DONE\n");
    }
    else
    {
        fprintf(stderr,"Error %s function failed\n","do_open_files()");
    }
    long magic_string_size;
    printf("INFO : Decoding magic string length\n");
    if(decode_magic_string_size(decInfo,&magic_string_size) == e_success)
    {
        printf("INFO : Size of magic string - %ld\n",magic_string_size);
        printf("INFO : DONE\n");
        if(decode_magic_string(decInfo,magic_string_size) == e_success)
        {
            printf("INFO : Decoding magic String Signature\n");
            printf("INFO : DONE\n");
        }
        else
        {
            fprintf(stderr,"Error %s function failed\n","decode_magic_string()");
        }
    }
    else
    {
        fprintf(stderr,"Error %s function failed\n","decode_magic_string_size()");
        return e_failure;
    }
    printf("Req : Enter magic string : ");
    char enter_magic_string[20];
    scanf("%s",enter_magic_string);
    if(strcmp(decInfo->magic_string,enter_magic_string) == e_success)
    {
        printf("INFO : User entered magic string and decoded magic string same\n");
    }
    else
    {
        printf("INFO : User enter magic string and decoded magic string are not same\n");
        return e_failure;
    }
    long extn_size;
    printf("INFO : Decoding Output file Extension size\n");

    if(decode_secret_file_extn_size(decInfo, &extn_size) == e_success)
    {
        //printf("Decoded Extension length %ld\n",extn_size);
        printf("INFO : DONE\n");
    }
    else
    {
        fprintf(stderr,"Error %s function failed\n","decode_extn_size()");
        return e_failure;
    }
    printf("INFO : Decoding Output file extension\n");
    if(decInfo->output_file_fname != NULL)
    {
	printf("INFO : Output file not mentioned. Creating %s as default\n",decInfo->output_file_fname);
	printf("INFO : Opened %s\n",decInfo->output_file_fname);
    }
    else
    {
	printf("INFO : Output file mentioned\n");
    }
    printf("INFO : DONE. Opened all required files\n");
    if(decode_secret_file_extn(decInfo,extn_size) == e_success)
    {
       // printf("decode extention file :%s\n",decInfo->extn_secret_file);
        printf("INFO : DONE\n");
    }
    else
    {
        fprintf(stderr,"Error %s function failed\n","decode_extn()");
        return e_failure;
    }
    printf("INFO : Decoding %s File Size\n",decInfo->output_file_fname);
    long secret_file_size = 0;
    if(decode_secret_file_size(decInfo, &secret_file_size) == e_success)
    {
        //printf("decoded secret file length %ld\n",secret_file_size);
        printf("INFO : DONE\n");
    }
    else
    {
        fprintf(stderr,"Error %s function failed\n","decode_secret_file_size()");
        return e_failure;
    }
    printf("INFO : Decoding %s File Data\n",decInfo->output_file_fname);
    if(decode_secret_file_data(decInfo, secret_file_size) == e_success)
    {
        printf("INFO : DONE\n");
    }
    else
    {
        fprintf(stderr,"Error %s function failed\n","decode_secret_file()");
        return e_failure;
    }
    return e_success;

}
Status decode_magic_string_size(DecodeInfo *decInfo,long *magic_string_size)
{
    char buffer[32];
    fread(buffer,sizeof(char),32,decInfo->fptr_stego_image);
    decode_size_from_lsb(magic_string_size,buffer);
    return e_success;

}
Status decode_magic_string(DecodeInfo *decInfo,long magic_string_size)
{
    char temp_buffer[8];
    for(int i=0;i<magic_string_size;i++)
    {
        fread(temp_buffer,sizeof(char),8,decInfo->fptr_stego_image);
        decode_byte_from_lsb(&(decInfo->magic_string[i]),temp_buffer);
    }
    decInfo->magic_string[magic_string_size] ='\0';
    return e_success;
}
Status decode_size_from_lsb(long *data, char *image_buffer)
{
    *data = 0;
    for(int i = 0;i < 32;i++)
    {
        char lsb = (image_buffer[i] & 1) << i;
        *data= *data | lsb;
    }
    return e_success;


}
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        *data = *data| (image_buffer[i] & 1) << i;
    }
    return e_success;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo, long *extn_size)
{
    char buffer[32];
    fread(buffer,sizeof(char),32,decInfo->fptr_stego_image);
    decode_size_from_lsb(extn_size,buffer);
   // printf("%ln",extn_size);
    return e_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo, long extn_size)
{
    char temp_buffer[8];
    for(int i = 0;i < extn_size;i++)
    {
        fread(temp_buffer,sizeof(char),8,decInfo->fptr_stego_image);
        decode_byte_from_lsb(&(decInfo->extn_secret_file[i]),temp_buffer);
	//printf("Deocoded character %d:%c\n",i,decInfo->extn_secret_file[i]);

    }
    decInfo->extn_secret_file[extn_size] = '\0';
    //printf("%s",decInfo->extn_secret_file);
    return e_success;

}
Status decode_secret_file_size(DecodeInfo *decInfo, long *secret_file_size)
{
    char buffer[32];
    fread(buffer,sizeof(char),32,decInfo->fptr_stego_image);
    decode_size_from_lsb(secret_file_size,buffer);
   // printf("%ln",secret_file_size);
    return e_success;

}
Status decode_secret_file_data(DecodeInfo *decInfo,long secret_file_size)
{
    //printf("Decodded Secret File Data: ");

    char temp_buffer[8];
    char  decoded_byte;
    char *secret_file_data= (char *)malloc(secret_file_size + 1);
    if(secret_file_data == NULL)
    {
        printf("Memory not allocated\n");
        return e_failure;
    }
    for (int i = 0; i < secret_file_size; i++)
    {
        fread(temp_buffer, sizeof(char), 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&decoded_byte, temp_buffer);
        secret_file_data[i] = decoded_byte;
        fwrite(&decoded_byte, sizeof(char), 1, decInfo->fptr_output);
       // printf("%c\n", decoded_byte);
    }
    secret_file_data[secret_file_size] = '\0';
   // printf("%s\n", secret_file_data); 
    free(secret_file_data); 
    return e_success;
}
