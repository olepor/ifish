#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * This function prints inputfile line by line.
 */
int printFile(char* filename) {
  FILE* file = fopen(filename, "r");
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  
  if (file == NULL) {
    printf("Error while opening file.\n");
    exit(1);
  }

  while((read = getline(&line, &len, file) != -1)) {
    printf("%s", line);
  }
  printf("\n");
  free(line);
  fclose(file);
  return 0;
}

/*
 * This function converts char to binary.
 * Works by reading character one by one and checks what binary value it will get.
 * To create a full byte, a value is OR'ed in then bitshifted by 2. By OR'ing 
 * in the value after bitshifting the binary value will be added.
 * If the byte has reached four characters it is written to file.
 * This function dosent work 100%, there are some bits on places it should not be.
 */
int compressor(char* readfile, char* writefile) {
  FILE* infile = fopen(readfile, "r");
  FILE* outfile = fopen(writefile, "w");
  char buf;
  unsigned char postbuf;
  int counter = 0;
  
  if (infile == NULL) {
    printf("Error while opening infile.\n");
    exit(1);
  }
  
  if (outfile == NULL) {
    printf("Error while opening outfile.\n");
    exit(1);
  }
  
  while (fread(&buf, 1, 1, infile) != 0) {
    if (counter == 4) {
      if (fwrite(&postbuf, 1, 1, outfile) != 0) {
	counter = 0;
      }
    }
    if (buf == ' ') {
      postbuf = (postbuf << 2) ^ 0;
    } else if (buf == ':') {
      postbuf = (postbuf << 2) ^ 1;
    } else if (buf == '@') {
      postbuf = (postbuf << 2) ^ 2;
    } else if (buf == '\n') {
      postbuf = (postbuf << 2) ^ 3;
    }
    counter++;
  } 
  fclose(infile);
  fclose(outfile);
  return 0;
}

/*
 * This function uncompresses files.
 * Works by comparing the binary value through an AND operation. 
 * The binary number is shifted to the right where it gets AND'ed
 * with 3. 
 */

int uncompressor(char* inputfile) {
  FILE* file = fopen(inputfile, "r");
  char chars[] = {' ', ':', '@', '\n'};
  unsigned char buf;
  int counter = 0;
  
  if (file == NULL) {
    printf("Error while opening file.\n");
    exit(1);
  }
  
  while(fread(&buf, 1, 1, file) != 0) {
    for(counter = 1; counter < 5; counter++) {
      if (counter == 1) {
	printf("%c", chars[buf >> 6]);
      } else if (counter == 2) {
	printf("%c", chars[(buf >> 4) & 3]);
      } else if (counter == 3) {
	printf("%c", chars[(buf >> 2) & 3]);
      } else if (counter == 4) {
	printf("%c", chars[buf & 3]);
      }
    }
  }
  printf("\n");
  fclose(file);
  return 0;
}

int main (int argc, char* argv[]) {
  
  if (argc == 1) {
    printf("Ingen argument.\n");
    exit(1);
  }

  
  if (strcmp(argv[1], "p") == 0) {
    printFile(argv[2]);
  } else if (strcmp(argv[1], "e") == 0) {
    compressor(argv[2], argv[3]);
  } else if (strcmp(argv[1], "d") == 0) {
    uncompressor(argv[2]);
  } else {
    printf("Ugjyldig kommando.\n");
  }
  return 0;
}
