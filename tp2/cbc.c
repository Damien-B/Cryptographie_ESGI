#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITERATIONS 3
#define MASKLENGTH 8

unsigned char key[] = "password";

void printCharInBits(char input) {
  printf("PRINTING (in bits) : %c\n", input);
  int i;
  for(i=0;i<8;i++){
      printf("%d", (input & (1 << i)) >> i);
      if(i==7) {
        printf("\n");
      }
    }
}

int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n");


  // printf("Enter a password to encrypt your file :\n");
  // scanf("%s", &key);


  FILE *inputFile;
  inputFile = fopen("test.txt", "rb");//fopen("test.bmp", "rb");
  FILE *outputFile = fopen("test2.bmp", "wb");//fopen("test.txt", "wb");
  
  if(inputFile==NULL) {
    printf("NOTHING IN FILE\n");
  } else {
    printf("FILE LOADED\n");

    unsigned char out[sizeof(test)];
    int currentByte;
    int i = 0;
    while((currentByte = fgetc(test)) != EOF)
    {
      char byte = currentByte;
      fputc(byte ^ key[i % sizeof(key)], outputFile);
      i++;
    }
  }


  fclose(inputFile);
  fclose(outputFile);

  printf("##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
