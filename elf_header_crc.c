/*  
    This is a c program that takes a single command line argument FILE.
    The FILE is a target elf file.
    This program reads the program header table of the file and prints
    out the crc32 checksum of the table.
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
    long int ph_off;
    short e_phentsize;
    short e_phnum;
    unsigned int checkSum;

     /* opens file in binary mode */
    fd = fopen(argv[1],"rb");

    /* error catch to ensure file was opened correctly */
    if(fd == NULL){
        printf("Unable to open file.\n");
        exit(1);
    }

    /* sets the current file offset at the program header table and reads it */
    fseek(fd,0x20,SEEK_SET);
    fread(&ph_off,sizeof(ph_off),1,fd);

    /* sets the current file offset to 0x36 and gets the size and number of
       the table header entries and reads them */
    fseek(fd,0x36,SEEK_SET);
    fread(&e_phentsize,sizeof(e_phentsize),1,fd);
    fread(&e_phnum,sizeof(e_phnum),1,fd);

    /* get length of the program header table */
    fdlen = e_phentsize * e_phnum;

    /* delcare buffer size the size of program header table */
    buffer = (char *)malloc(fdlen);
    
    /* sets the offset to the beginning of the progam header table and reads
       it into buffer */
    fseek(fd,ph_off,SEEK_SET);
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

