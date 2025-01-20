#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    } 
    if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    } else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (argv[4] == NULL)
    {
        printf("INFO: Output file not mentioned. Creating output.bmp as default\n");
        encInfo->stego_image_fname = "output.bmp";
    }
    printf("INFO: Opening required files\n");
    if (strstr(argv[2], ".bmp") != NULL)
    {
        printf("INFO: Opened %s\n", argv[2]);
        encInfo->src_image_fname = argv[2];
    } else
    {
        return e_failure;
    }
    if (strstr(argv[3], ".") != NULL)
    {
        printf("INFO: Opened %s\n", argv[3]);
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, strchr(argv[3], '.'));
    } else
    {
        return e_failure;
    }
    if (argv[4] != NULL)
    {
        if (strstr(argv[4], ".bmp") != NULL)
        {
            printf("INFO: Opened %s\n", argv[4]);
            encInfo->stego_image_fname = argv[4];
        } else
        {
            return e_failure;
        }
    }
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    if(open_files(encInfo) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","open_files()");
        return e_failure;
    }
    printf("INFO : ## Encoding Procedure Started ##\n");
    if(check_capacity(encInfo) == e_success)
    {
        printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    }
    else
    {
        fprintf(stderr,"Error : %s function failed\n","check_capacity()");
        return e_failure;
    }
    if(encInfo->secret_fname)
    {
        printf("INFO: DONE.Not Empty\n");
    }
    else{
        printf("Secret file is Empty\n");
        return e_failure;
    }
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if(encInfo->image_capacity)
    {
        printf("INFO: DONE.Found OK\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","image_capacity");
        return e_failure;
    }
    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","copy_bmp_header()");
        return e_failure;
    }
    printf("Req : Enter magic string : ");
    scanf("%s",encInfo->magic_string);
    printf("INF0: Magic string length is %ld\n",strlen(encInfo->magic_string));

    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string_size(strlen(encInfo->magic_string),encInfo) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","encode_magic_string_size()");
        return e_failure;
    }
    if(encode_magic_string(encInfo->magic_string,encInfo) == e_success)
    {
        //printf("INFO: DONE\n");
        printf("INFO: Encoding %s File Extenstion Size\n", encInfo->secret_fname);
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","encode_magic_string()");
        return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","encode_secret_file_extn_size()");
        return e_failure;
    }
    printf("INFO: Encoding %s File Extenstion\n", encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","encode_secret_file_extn()");
        return e_failure;
    }
    printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error : %s function failed\n","encode_secret_file_size()");
        return e_failure;
    }
    printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);

    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error: %s function failed\n","encode_secret_file_data()");
        return e_failure;
    }
    printf("INFO: Copying left Over Data\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: DONE\n");
    }
    else{
        fprintf(stderr,"Error: %s function failed","copy_remaining_img_data()");
        return e_failure;
    }
    return e_success;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{

    int header_size = 54;
    char BMP_header[header_size];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(BMP_header, 1, header_size, fptr_src_image);
    fwrite(BMP_header, 1, header_size, fptr_dest_image);
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET); // Reset the file pointer to the beginning
    return size;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //printf("\nImage capacity = %d\n", encInfo->image_capacity);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    //printf("Secret file size = %ld Bytes\n", encInfo->size_secret_file);

    int Encoding_Things = 54 + ((4 + strlen(encInfo->magic_string)) + (4 + strlen(encInfo->extn_secret_file)) + (4 + encInfo->size_secret_file)) * 8;
    //printf("Encoding things length = %d\n", Encoding_Things);

    if (encInfo->image_capacity > Encoding_Things)
    {
        //printf("INFO: Checking beautiful.bmp capacity to handle secret.txt\n");

        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_magic_string_size(long magic_string_size, EncodeInfo *encInfo)
{
    //printf("INFO: Encoding Magic String Signature\n");
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(magic_string_size, temp_buffer);
    fwrite(temp_buffer, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_size_to_lsb(long data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        char get_bit = (data >> i) & 1;
        image_buffer[i] = (image_buffer[i] & ~1) | get_bit;
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char image_buffer[8];

    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(image_buffer, sizeof(char), 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], image_buffer);
        fwrite(image_buffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
     {
        char get_bit = (data >> i) & 1;
        image_buffer[i] = (image_buffer[i] & ~1) | get_bit;
    }
    return e_success;
}

Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, temp_buffer);
    fwrite(temp_buffer, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[8];
    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(image_buffer, sizeof(char), 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], image_buffer);
        fwrite(image_buffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, temp_buffer);
    fwrite(temp_buffer, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char image_buffer[8];
    char*data = (char*)malloc(encInfo->size_secret_file + 1);
    printf("INFO: Secret file data length : %ld\n",encInfo->size_secret_file);
    rewind(encInfo->fptr_secret);
    fread(data,1,encInfo->size_secret_file,encInfo->fptr_secret);
    printf("INFO :Secret file data: %s\n",data);
    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(image_buffer, sizeof(char), 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(data[i], image_buffer);
        fwrite(image_buffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[1];
    while (fread(buffer, sizeof(char), 1, fptr_src_image) > 0)
    {
        fwrite(buffer, sizeof(char), 1, fptr_dest_image);
    }
    return e_success;
}
