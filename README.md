# LSB-Image-Steganography

An application of cryptography, used for securely transferring data using images.

Developed a C-based application for securely transmitting data using image steganography. This method encodes sensitive information into the least significant bits of an image's byte data, ensuring that messages remain hidden from unauthorized users. Leveraged command line arguments, file operations, bitwise operations, and pointers to achieve successful data encoding and decoding. The process is secured by a key that allows only authorized parties to decode the embedded message.

Skills Gained: C, Command Line Arguments, File Operations, Bitwise Operations, Pointers, Data Structures

# Jist About the Project
Steganography is the art of hiding the fact that communication is taking place, by hiding information in other information. Many different carrier file formats can be used, but digital images are the most popular because of their frequency on the internet. For hiding secret information in images, there exists a large variety of steganography techniques some are more complex than others and all of them have respective strong and weak points. Different applications may require absolute invisibility of the secret information, while others require a large secret message to be hidden. 

Steganography is the practice of hiding private or sensitive information within something that appears to be nothing out to the usual. Steganography is often confused with cryptology because the two are similar in the way that they both are used to protect important information. The difference between two is that steganography involves hiding information so it appears that no information is hidden at all. If a person or persons views the object that the information is hidden inside of he or she will have no idea that there is any hidden information, therefore the person will not attempt to decrypt the information.

What steganography essentially does is exploit human perception, human senses are not trained to look for files that have information inside of them, although this software is available that can do what is called Steganography. The most common use of steganography is to hide a file inside another file.

***Requirements***:

    • The application accept an image file say .bmp along with the a text file which contains the message to be steged
    • Analize the size of the message file and the data part of the .bmp file to check whether the messsage could fit in the provided .bmp image
    • Provide a option to steg a magic string which could be usefull to identify whether the image is steged or not
    • The appliaction should provide a option to decrpt the file 
    • This has to be an command line apliaction and all the options has to be passed as an command line argument

***Pre-requisite***:

    • Knowledge of encryption and decprytion
    • File I/O
    • Pointers and Structures
    • General idea of .bmp image file
    
***Output*** :
1. *Original Image File* :
   <br>
   <br>
   <img src="https://github.com/VisheshK65/LSB-Image-Steganography/blob/main/Original_Image.png"  width="900" height="450">
   <br>
2. *Performing Encoding* :
   <br>
   <br>
   <img src="https://github.com/VisheshK65/LSB-Image-Steganography/blob/main/Encoding_Command.png"  width="900" height="450">
   <br>
3. *Encoded Image* :
   <br>
   <br>
   <img src="https://github.com/VisheshK65/LSB-Image-Steganography/blob/main/Encoded_Image.png"  width="900" height="450">
   <br>
4. *Perform Decoding* :
   <br>
   <br>
   <img src="https://github.com/VisheshK65/LSB-Image-Steganography/blob/main/Decoding_Command.png"  width="900" height="450">
   <br>
5. *Error Output* :
   <br>
   <br>
   <img src="https://github.com/VisheshK65/LSB-Image-Steganography/blob/main/Error_Output.png"  width="900" height="250">
   <br>


