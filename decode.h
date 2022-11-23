#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include <stdio.h>
/* 
 * Structure to store information required for
 * decoding  encoded Image to secret file 
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct _DecodeInfo
{
	char *d_password;
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

	char *magic_string;
    char *d_extn_secret_file;
	char *d_secret_fname;
	int size_extn_secret;

    /* Output File Info */
    char output_fname[50];
    FILE *fptr_output;
    int size_output_file;

} DecodeInfo;                 //structure used to access various information used while decoding 


/* Decoding function prototype */

/* Read and validate decode args from argv */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status_d do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status_d open_d_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status_d decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension size */
int decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status_d decode_secret_file_extn(int *size, char *ext_name, DecodeInfo *decInfo);

/* Decode secret file size */
int decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status_d decode_secret_file_data(int *size,DecodeInfo *decInfo);

/* Decode function, which does decoding of magic string encoded in the stego file */
Status_d decode_magic_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode function to extract the content stored */
Status_d decode_data_to_image(char *data, int size, DecodeInfo *decInfo);

/* Decode LSB to byte of image data array */
Status_d decode_lsb_to_byte(char *data, char *image_buffer);

/* Decode LSB to size of image data array */
Status_d decode_size_from_lsb(int *size, char *data);
#endif
