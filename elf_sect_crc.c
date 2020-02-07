/* 
    This is a c program that takes to command line arguments FILE and SECTION_NAME.
    FILE will be a target elf file and SECTION_NAME will be a string identifying 
    which sections for which the program will produce a crc32 checksum. If there 
    are multiple sections that match the provided SECTION_NAME, the program will
    produce crc32 checksums for each of them.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned int crc32(unsigned char *message, int length);
unsigned reverse(unsigned x);

int main(int argc, char *argv[]){

    /* Checks for one and only one command line argument */
    if(argc>3){
        printf("Error: Too many arguments.\n");
        exit(1);
    }


    FILE *fd;
    char *buffer;
	char *targetName;   //for argv[2]
    long fdlen;
    long int e_shoff;
	long int sh_offset;
    short e_shentsize;
    short e_shnum;
    int sh_name;
	short e_shstrndx;
    unsigned int checkSum;
	long int str_tbl;
	char sectionName[256];
	int i;

     /* opens file in binary mode */
    fd = fopen(argv[1],"rb");

    /* error catch to ensure file was opened correctly */
    if(fd == NULL){
        printf("Unable to open file.\n");
        exit(1);
    }
    /* assigns the 2nd command argument as the specified section to crc32 */
	targetName = argv[2];

    /* ensures there is 2nd command provided to the program */
    if(targetName == NULL){
        printf("argv[2] cannot be null.\n");
        return 1;
    }

    /* seeks to the offset of the section header table in file header and reads it */
    fseek(fd,0x28,SEEK_SET);
    fread(&e_shoff,sizeof(e_shoff),1,fd);

	/* seeks to the offsets in header file and reads them into variables*/
	fseek(fd,0x3A,SEEK_SET);
    fread(&e_shentsize,sizeof(e_shentsize),1,fd);
	fread(&e_shnum,sizeof(e_shnum),1,fd);
	fread(&e_shstrndx,sizeof(e_shstrndx),1,fd);

    /* allocate enough memory to store section name */
	fdlen = e_shentsize;
	buffer = (char *)malloc(e_shentsize);

	/* seeks to index of section header containing section names */
	str_tbl = e_shoff + e_shstrndx * e_shentsize + 0x18;
	fseek(fd,str_tbl,SEEK_SET);
	fread(&sh_offset,1,sizeof(sh_offset),fd);
	
	/* for loop to iterate through section names */
	for(i=0;i<e_shnum;i++){
        
		/* seeks to section headers */
		fseek(fd,e_shoff,SEEK_SET);

		/*reads first 4 bytes to find offset of string for each section */
        fread(&sh_name,sizeof(sh_name),1,fd);
		
		/* seek to correct section name and place it into buffer */
		fseek(fd,sh_offset + sh_name,SEEK_SET);
		fgets(sectionName,256,fd);
		 
		if(strcmp(targetName,sectionName) == 0){
            /* goes to the section row specified by targetName */
			fseek(fd,e_shoff,SEEK_SET);
            /* reads the section row into buffer */
			fread(buffer,e_shentsize,1,fd);
            /* calculates crc32 checksum and prints it */
			checkSum = crc32(buffer,fdlen);
			printf("%x\n",checkSum);
		}
		/* moves the e_shoff pointer to the next section */
		e_shoff = e_shoff + e_shentsize; 
	}
    
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

