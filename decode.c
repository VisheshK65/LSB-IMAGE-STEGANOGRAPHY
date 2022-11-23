
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "decode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) //function to validate the given arguements 
{   
	//compare the 2nd arguement if it is a .bmp file or not 
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{
		decInfo -> stego_image_fname = argv[2];
	}
	else
	{
		printf("\nProvided input encoded image file is not compatible.\nUsage : image_file.bmp\n");
		return d_failure;
	}
	if(argv[3] == NULL)            //check if output file name is provided or not 
	{
		strcpy(decInfo->output_fname,"output"); //if not then update the "output" as default name 
	}
	else
	{
		strcpy(decInfo->output_fname,argv[3]); //else copy the provided name 
	}
	return d_success;
}

Status_d open_d_files(DecodeInfo *decInfo) //function to open the required files 
{   
	// Src Encoded Image file 
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
	// Do Error handling
	if (decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "\nERROR: Unable to open file %s\n", decInfo->stego_image_fname);

		return d_failure;
	}
	sleep(1);
	printf("\nOpened %s\n", decInfo->stego_image_fname);
	return d_success;
}

Status_d do_decoding(DecodeInfo *decInfo) //function to perform the decoding operation 
{
	fseek(decInfo->fptr_stego_image,54,SEEK_SET); //update the encoded image file pointer to 54th byte
	if(decode_magic_string(decInfo) == d_success) //call the funtion to decode the magic string 
	{
		sleep(1);
		printf("\nDecoded the magic string \n");
		decInfo -> size_extn_secret = decode_secret_file_extn_size(decInfo); //call the function to obtain the extension length 
		if(decInfo->size_extn_secret != -1)
		{
			decInfo->d_extn_secret_file = malloc(decInfo->size_extn_secret * 1); //create a memory in heap to store the extension 
			if(decode_secret_file_extn(&decInfo->size_extn_secret, decInfo->d_extn_secret_file, decInfo) == d_success)
			{
				sleep(1);
				printf("\nDecoded the secret file extension\n");
				strcat(decInfo->output_fname,decInfo->d_extn_secret_file); //concatenate the output file name with the extension 
				decInfo->fptr_output = fopen(decInfo->output_fname,"w");   //open the output file 
				if(decInfo->fptr_output == NULL)
				{
					perror("fopen");
					fprintf(stderr,"\nERROR : Unable to open file %s\n", decInfo->output_fname);
					return d_failure;
				}
				sleep(1);
				printf("\n%s successfully opened\n", decInfo->output_fname); 
				decInfo->size_output_file = decode_secret_file_size(decInfo); //call the function to store the length of the secret file content 
				if(decInfo->size_output_file != -1)
				{
					if(decode_secret_file_data(&decInfo->size_output_file, decInfo) == d_success) //call the function to decode the secret file data
					{
						sleep(1);
						printf("\nDecoded the content of the secret file successfully to %s\n", decInfo->output_fname);
						return d_success;
					}
					else
						return d_failure;
				}
				else
					return d_failure;
			}
			else
				return d_failure;
		}
		else
			return d_failure;
	}
	else
	{
		printf("\n!! Wrong Password Entered. Try Again !!\n");
		return d_failure;
	}
}

Status_d decode_magic_string(DecodeInfo *decInfo)  //function to decode the magic string data 
{
	int str_len = strlen(decInfo->d_password);           //calculate the length of the MAGIC_STRING 
	decInfo->magic_string = calloc((str_len + 1), 1);
	if(decode_magic_data_from_image(str_len, decInfo->fptr_stego_image, decInfo) == d_success) //decode the magic string from the encoded image 
	{
		decInfo->magic_string[str_len] = '\0';    //append the last byte with null character to make it a string 
		if(strcmp(decInfo->magic_string, decInfo->d_password) == 0) //compare the string if both are equal or not 
		{
			return d_success;
		}
		return d_failure;
	}
}

int decode_secret_file_extn_size(DecodeInfo *decInfo) //decode the size of the secret file extension 
{
	char size_buff[32];                               //buffer to store the 32 bytes from encoded image 
	int length = 0;
	fread(size_buff,1,32,decInfo->fptr_stego_image);  //read the 32 bytes from the encoded image to buffer 
	if(decode_size_from_lsb(&length, size_buff) == d_success) //call the function to decode the size of the extension from encoded image 
	{
		return length;
	}
	else
	{
		return -1;
	}
}

Status_d decode_secret_file_extn(int *size, char *extn_data, DecodeInfo *decInfo) //decode the secret file extension 
{
	int i = *size;
	char file_extn_buff[i + 1];
	if(decode_data_to_image(file_extn_buff, i, decInfo) == d_success) //decode the data from the encoded image 
	{
		file_extn_buff[i] = '\0'; 
		strcpy(extn_data, file_extn_buff); //copy the extension stored in buffer to extn_data 
		return d_success;
	}
	else
		return d_failure;
}

int decode_secret_file_size(DecodeInfo *decInfo) //decode the secret file size 
{
	char size_buff[32];  //buffer to store 32bytes from the image file 
	int length = 0;
	fread(size_buff,1,32,decInfo->fptr_stego_image);
	if(decode_size_from_lsb(&length, size_buff) == d_success) //call the function to decode the size from the encoded image 
	{
		return length;
	}
	else
	{
		return -1;
	}
}

Status_d decode_secret_file_data(int *size, DecodeInfo *decInfo) //function to decode the secret file data 
{
	char file_buff[*size+1];
	if(decode_data_to_image(file_buff, *size, decInfo) == d_success) //call the function to decode the data 
	{ 
		fwrite(file_buff,1,*size,decInfo->fptr_output);         //write the data to output file from buffer 
		return d_success;
	}
	return d_failure;
}

Status_d decode_size_from_lsb(int *size, char *data)   //function to decode the size from the data been shared 
{
	int len = 0x00;
	int bit_pos = 31;
	for(int i=0; i<32; i++)
	{
		len = ((data[i] & 0x01) << bit_pos--) | len;  //obatin the size from this opeartion
	}
	*size = len;
	return d_success;
}

Status_d decode_magic_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo) //function to decode the magic string 
{
	char buff[8];
	for(int i=0; i<size; i++)
	{
		fread(buff,1,8,fptr_stego_image);
		decode_lsb_to_byte(&decInfo->magic_string[i], buff); //call the funtion to decode to obatin the bit values which gives the magic string
	}
	return d_success;
}

Status_d decode_data_to_image(char *data, int size, DecodeInfo *decInfo) //function to decode the data from image 
{
	char buff[8];                         //buffer to access 8 bytes at a time from the encoded image 
	for(int i=0; i<size; i++)
	{
		fread(buff,1,8,decInfo->fptr_stego_image);
		decode_lsb_to_byte(&data[i],buff);   //call the function to perform the decoding 
	}
	return d_success;
}

Status_d decode_lsb_to_byte(char *data, char *image_buffer) /*function to decode the byte to obatin the bits stored in LSB of the bytes of encoded 
															  image*/
{
	int bit_pos = 7;
	unsigned char ch = 0x00;
	for(int i=0; i<8; i++)
	{
		ch = ((image_buffer[i] & 0x01) << bit_pos--) | ch;
	}
	*data = ch;
	return d_success;
}

