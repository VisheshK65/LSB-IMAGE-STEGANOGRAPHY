#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo) //Function to read and validate the arguements provided during encoding 
{
	//compare the 2nd arguement if it is a .bmp file or not 
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{	
		encInfo -> src_image_fname = argv[2];    //store the input image name 
	}
	else
	{
		printf("\nProvided input image file is not compatible.\nUsage : image_file.bmp\n");
		return e_failure;
	}

	//compare the 3rd arguement if it is a .txt or .c or not 
	if((strcmp(strstr(argv[3],"."),".txt") == 0) || (strcmp(strstr(argv[3],"."),".c") == 0))
	{
		strcpy(encInfo->extn_secret_file, strstr(argv[3],".")); //copy the extension in variable declared in structure
		encInfo -> secret_fname =  argv[3];     			    //store the secret file name   
	}
	else
	{
		printf("\nProvided input message file is not in supported format\nUsage : filename.txt or filename.c\n");
		return e_failure;
	}

	//check if output .bmp file is given or not, If yes then check if it is .bmp else if not given then create a default .bmp output file 
	if(argv[4] == NULL)
	{
		printf("\nOuput File not mentioned. Creating steged_img.bmp as default output file\n"); 
		encInfo -> stego_image_fname = "steged_img.bmp";            
	}
	else 
	{
		if(strcmp(strstr(argv[4],"."),".bmp") == 0)  			//perform comparison if .bmp output file is provided or not 
		{
			encInfo -> stego_image_fname = argv[4];
		}
		else
		{
			return e_failure;
		}
	}
	return e_success;                                 			//if all the files are according to the requirement then return success
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("\nwidth = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/*Get file size
 * Input : FILE *fptr
 * Output : total size of the content fptr stores 
 * using fseek to make fptr point to the end position and then calculate length using ftell
 */

uint get_file_size(FILE *fptr)
{
	fseek(fptr,0L,SEEK_END);
	int f_size = ftell(fptr);
	fseek(fptr,0L,SEEK_SET);
	return f_size;
} 


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
	// Source Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");   //file pointer to the source image file 
	// Do Error Handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "\nERROR: Unable to open file %s\n", encInfo->src_image_fname);
		return e_failure;
	}
	sleep(1);
	printf("\nOpened %s\n", encInfo->src_image_fname);

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");         //file pointer to secret file 
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "\nERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}
	sleep(1);
	printf("Opened %s\n", encInfo->secret_fname);

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w"); //file pointer to output encoded image file 
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "\nERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}
	sleep(1);
	printf("Opened %s\n", encInfo->stego_image_fname);

	// No failure return e_success
	return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image); //var to store the size of the image file 
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);           //var to hold the size of the secret file  
	printf("\nChecking for %s size\n", encInfo -> secret_fname);
	if(encInfo -> size_secret_file > 0)
	{ 
		sleep(1);
		printf("\nSecret file emptiness checking is done. File is not empty\n");
	}
	sleep(1);
	printf("\nChecking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
	if(encInfo -> image_capacity > (54 + (strlen(encInfo->e_password) + 4 + 4 + 4 + (encInfo -> size_secret_file)) * 8)) /*condition to check if encoding c																													can be carried out or not*/
	{
		return e_success;
	}
	else
	{
		printf("\nCannot fit the message within the input image file\n");
		return e_failure;
	}
}

/*function to copy the header i.e 1st 54 bytes from source image file 
  to destination image file*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *file_dest_image)
{
	char buff[54];
	fseek(fptr_src_image,0L,SEEK_SET);   //image source pointer pointing to the 1st byte 
	fread(buff,54,1,fptr_src_image);     //read the 54 bytes from source image file  
	fwrite(buff,54,1,file_dest_image);   //write the 54 bytes to destination file 
	return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
	if(encode_magic_string(encInfo->e_password,encInfo) == e_success)  //check if encoding of  magic string operation is performed successfully 
	{
		int ext_size = strlen(encInfo->extn_secret_file);       //calculate the length of the extension 
		if(encode_secret_file_extn_size(ext_size ,encInfo, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) //check if encoding of                                                                     extension size is performed successfully 
		{
			if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success) //check if encoding of extension is performed or not 
			{
				if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success) //check if encoding of secret file size if performed
				{ 
					if(encode_secret_file_data(encInfo) == e_success)                 //check if encoding of secret file data is performed or not  
					{
						if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) /* performing copy of remaining                                                                                        bytes once the encoding of required details is performed*/                                                                                             
							return e_success;
						else
							return e_failure;
					}
					else
						return e_failure;
				}
				else
					return e_failure;
			}
			else
				return e_failure;
		}
		else
			return e_failure;
	}
	else
		return e_failure;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)  //function to encode the magic string   
{
	int m_size = strlen(magic_string);                               //calculate the length of the magic string  
	if(encode_data_to_image(magic_string, m_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) /* call the function to encode t                                                                        the characters in magic string one by one*/
	{
		return e_success;
	}
	else
		return e_failure;
}

Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo, FILE *fptr_src_image, FILE *fptr_stego_image) /*function to encode the                                                                                                                     size of secret file extension*/
{
	char buff[32];
	fread(buff,1,32,fptr_src_image);
	if(encode_size_to_lsb(file_extn_size, buff) == e_success) //function to performing encoding bit by bit of the content 
	{
		fwrite(buff,1,32,fptr_stego_image);       //write the encoded content to output file image file 
		return e_success;
	}
	else
		return e_failure;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo) //function to encode the secret file extension 
{
	int size = strlen(encInfo->extn_secret_file);                    //store the length of the extension of the secret file  
	if(encode_data_to_image(file_extn, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) //perform encoding of the extension
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo) //function to encode the secret file size 
{
	char sec_buff_size[32];
	fread(sec_buff_size,1,32,encInfo->fptr_src_image);
	if(encode_size_to_lsb(file_size,sec_buff_size) == e_success)  //call the function to encode the size in stego image file 
	{
		fwrite(sec_buff_size,1,32,encInfo->fptr_stego_image);     //if encoding is done then write the encoded value in the stego image file
		return e_success;
	}
	else
		return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)  //function to encode the secret file data 
{
	char sec_array[encInfo->size_secret_file];
	fread(sec_array,1,encInfo->size_secret_file, encInfo->fptr_secret);
	if(encode_data_to_image(sec_array, strlen(sec_array), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) /*call the function to e                                                                              encode the content of the secret file data*/
		return e_success;
	else
		return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) //function to the copy the remaining soure image data to destination image file 
{
	char ch;
	while(fread(&ch,1,1,fptr_src) > 0)
	{
		fwrite(&ch,1,1,fptr_dest);
	}
    return e_success;
}

Status encode_data_to_image(char *data , int size, FILE *fptr_src_image, FILE *fptr_stego_image) /* function to encode the data data input to image                                                                                                     file*/
{
	EncodeInfo en;
	while(size > 0)
	{
		fread(en.image_data,1,8,fptr_src_image);                   //read the 8 bytes of data from current position of image file to image buffer 
		if(encode_byte_to_lsb(*data,en.image_data) == e_success)   //perform encoding of the data been passed with the image data buffer				
			fwrite(en.image_data,1,8,fptr_stego_image);
		*data++;
		size--;
	}
	return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)            //function to encode the data bits with the bytes stored in image buffer 
{
	unsigned int mask = 0x80, i;                                    //mask used to access the bits of data 
	for(int i=0; i<8; i++)
	{
		image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7-i)); /*update the image buffer to store the bit of data with the byte 
																				 stored image buffer obtained from the source image file */ 
		mask = mask >> 1;                                           //update mask by performing left shift by one each time 
	}
	return e_success;
}

Status encode_size_to_lsb(int file_size, char *image_buffer) //function to encode the integer value size 32 bit by taking 32 byte from imag_buffer
{
	unsigned int mask = 1<<31, i;   //mask used for performing encoding 
	for(i=0; i<32; i++)
	{
		image_buffer[i] = (image_buffer[i] & 0xFE) | ((file_size & mask) >> (31 -i)); /* update the source image file bytes by encoding them to the
																						 bit of the integer value which is to be encoded*/
		mask = mask >> 1;
	}
	return e_success;
}





