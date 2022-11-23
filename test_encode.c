/*Documentation
Name          : VISHESH K
Date          : 15-10-2022
Documentation : LSB Image Steganography

Sample Input  :
a] Encoding 
1. ./a.out -e beautiful.bmp secret.txt
2. ./a.out

b]Decoding
1. ./a.out -d steged_img.bmp
2. ./a.out 

Sample Output :
a] Encoding 
1. 
Ouput File not mentioned. Creating steged_img.bmp as default output file

Opened beautiful.bmp
Opened secret.txt
Opened steged_img.bmp

All the required files opened successfully

---------------Encoding Procedure Started--------------

width = 1024
height = 768

Checking for secret.txt size

Secret file emptiness checking is done. File is not empty

Checking for beautiful.bmp capacity to handle secret.txt

Capacity checking done. It can be encoded

--------------Encoding Done----------------------------

So here a encoded image file is generated once encoding is successful. That image will hold the secret file data 

2. 
Error : Please pass command line arguements
Usage: ./a.out -e beautiful.bmp secret.txt/secret.c

b] Decoding
1. 
Opened steged_img.bmp

Encoded image file opened successfully

-------------Decoding Procedure Started--------------

Decoded the magic string

Decoded the secret file extension

output.txt successfully opened

Decoded the content of the secret file successfully to output.txt

--------------Decoding Done--------------------------
So here we are decoding the encoding image file to get the data which stored in the secret.txt. Hence an output.txt file is created in our example

2. 
Error : Please pass command line arguements
Usage: ./a.out -e beautiful.bmp secret.txt/secret.c

*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{	
	//Check if atleast 3 arguements are passed with required arguement 
	if(argc < 3)                       
	{
		printf("Error : Please pass command line arguements\nUsage: ./a.out -e beautiful.bmp secret.txt/secret.c\n");
		return e_failure; 										 //If condition fails then print the error message and return 1
	}

	//Check which operation is to be performed whether encode or decode
	if(check_operation_type(argv) == e_encode)
	{
		EncodeInfo encInfo; 									 //create a structure variable  
		if(read_and_validate_encode_args(argv, &encInfo) == e_success)  //check if the arguements provided is matching the requirement or not 
		{
			if(open_files(&encInfo) == e_success)                //if condition satisfies then call the function to open all the required files
			{
				char password[5];
				printf("\nEnter the password [4 digit pin]: ");
				scanf("%s",password);
				encInfo.e_password = password;
				sleep(1);
				printf("\nAll the required files opened successfully\n");
				sleep(1);
				printf("\n---------------Encoding Procedure Started--------------\n");
				if(check_capacity(&encInfo) == e_success)        //function to check if required message can be encoded within the img file   
				{
					sleep(1);
					printf("\nCapacity checking done. It can be encoded\n");  
					if(copy_bmp_header(encInfo.fptr_src_image, encInfo.fptr_stego_image) == e_success) /*function to copy the header from the source                                                                                                         to dest image*/ 
					{
						if(do_encoding(&encInfo) == e_success)   //function to perform encoding 
						{
							sleep(1);
							printf("\n--------------Encoding Done----------------------------\n");
							return e_success;                    //if successful encoding is done then return 0
						}
					}
				}
			}
		}
	}
	else if(check_operation_type(argv) == e_decode)  //function to check if decoding operation is to be performed 
	{
		DecodeInfo decInfo;                          //create a DecodeInfo structure variable 
		if(read_and_validate_decode_args(argv, &decInfo) == d_success) //check if all the arguements provided are according to requirement 
		{
			if(open_d_files(&decInfo) == d_success)  //if condition satisfies then open all the files 
			{
				char password[5];
				printf("\nEnter the password [4 digit pin]: ");
				scanf("%s",password);
				decInfo.d_password = password;

				sleep(1);
				//printf("\nEncoded image file opened successfully\n");                
				printf("\n-------------Decoding Procedure Started--------------\n");
				if(do_decoding(&decInfo) == d_success) //call the funtion do_decoding to perform decoding and check if successfully 
				{
					sleep(1);
					printf("\n--------------Decoding Done--------------------------\n");
					return d_success;                  //upon successfully decoding return 0 
				}
			}
		}
	}
	else
	{
		printf("!! Enter a valid operation input [Usage: -e or -d] !!\n"); /*if check_operation is not matching with either -e and -d then print err
																			 -or message*/
	}
	return 0;
}

OperationType check_operation_type(char *argv[])      //Function to check which operation has been selected by the user    
{
	if(strcmp(argv[1],"-e") == 0)
	{
		return e_encode;
	}
	else if(strcmp(argv[1],"-d") == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}

