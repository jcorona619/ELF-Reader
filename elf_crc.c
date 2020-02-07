/* 
   This is a c program that takes a single command line argument FILE.
   The FILE will be a target elf file that is read as a binary file.
   Once the FILE is read, its crc32 checksum is calculated and printed
   as a 8-digit hex number.
*/

#include <stdlib.h>
#include <stdio.h>

unsigned int crc32(unsigned char *message, int length);
unsigned reverse(unsigned x);

int main(int argc, char *argv[]){

    /* Checks for one and only one command line argument */
    if(argc<2){
        printf("Error: Too many arguments.\n");
        exit(1);
    }


    FILE *fd;
    char *buffer;
    long fdlen;
    unsigned int checkSum;

    fd = fopen(argv[1],"rb"); //opens file in binary mode

    /* error catch to ensure file was opened correctly */
    if(fd == NULL){
        printf("Unable to open file.\n");
        exit(1);
    }

    /* Finds the end of the file */
    fseek(fd,0,SEEK_END);

    /* gets the length of the file by returning the current offset */
    fdlen = ftell(fd);

    /* returns to the beginning of the file */
    rewind(fd);
    
    /* allocates a buffer to be the size of the file */
    buffer = (char *)malloc((fdlen));

    /* reads the file into the allocated buffer */
    fread(buffer,fdlen,1,fd);

    /* calculates the crc32 checksum and prints out its hexadecimal value */
    checkSum = crc32(buffer,fdlen);
    printf("%x\n",checkSum);
    

    /* close the file and free allocated memory */
    fclose(fd);
    free(buffer);
}



/* ONLINE SOURCE: "www.hackersdelight.org/hdcodetxt/crc.c.txt */
unsigned int crc32(unsigned char *message, int length){

    int i, j;
    unsigned int byte, crc;

    i = 0;
    crc = 0xFFFFFFFF;

    while(i<length){
        byte = message[i];
        byte = reverse(byte);
        for(j=0; j<=7; j++){
            if((int)(crc ^ byte) < 0)
                crc = (crc << 1) ^ 0x04C11DB7;
            else crc = crc << 1;
            byte = byte << 1;
        }
    i++;
    }
    return reverse(~crc);
}

/* ONLINE SOURCE: "www.hackersdelight.org/hdcodetxt/crc.c.txt */
unsigned reverse(unsigned x){
   
    x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
    x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
    x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
    x = (x << 24) | ((x & 0xFF00) << 8) | 
        ((x >> 8) & 0xFF00) | (x >> 24);
    return x;

}

