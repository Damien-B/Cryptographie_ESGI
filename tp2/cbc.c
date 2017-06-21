#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITERATIONS 3
#define MASKLENGTH 8

unsigned char key[] = "password";

// test function, not used
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

void encryptWithXOR(char *inputFileName, char *outputFileName) {
  FILE *inputFile;
  inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen(outputFileName, "wb");

  if(inputFile==NULL) {
    printf("NOTHING IN FILE\n");
  } else {
    printf("FILE LOADED\n");

    int currentByte;
    int i = 0;
    char digest;
    while((currentByte = fgetc(inputFile)) != EOF)
    {
      char byte = currentByte;
      if(i==0) {// if first iteration take the key to make a xor
        // encrypt this byte with the corresponding password character
        digest = byte ^ key[i % (sizeof(key)-1)];
      } else {// for all the other iterations take the previous result to make a xor
        digest = byte ^ digest;
      }
      fputc(digest, outputFile);
      i++;
    }
  }

  fclose(inputFile);
  fclose(outputFile);
}

void decryptWithXOR(char *inputFileName, char *outputFileName) {
  FILE *inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen(outputFileName, "wb");

  if(inputFile==NULL) {
    printf("NOTHING IN FILE\n");
  } else {
    printf("FILE LOADED\n");

    int currentByte;
    int i = 0;
    char previousByte;
    char digest;
    while((currentByte = fgetc(inputFile)) != EOF)
    {
      char byte = currentByte;
      if(i==0) {// if first iteration take the key to make a xor
        // encrypt this byte with the corresponding password character
        digest = byte ^ key[i % (sizeof(key)-1)];
      } else {// for all the other iterations take the previous result to make a xor
        digest = byte ^ previousByte;
      }
      // write the decrypted byte in our output file
      fputc(digest, outputFile);
      // set the current byte as previousByte for the next iteration
      previousByte = byte;
      i++;
    }
  }

  fclose(inputFile);
  fclose(outputFile);
}

int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n");

  char userInputFileName[100];
  char userOutputFileName[100];

  printf("Enter the name of the file you want to encrypt (with the extension) :\n");
  scanf("%s", userInputFileName);
  printf("Enter the name of the file you want to encrypt (with the extension) :\n");
  scanf("%s", userOutputFileName);

  // printf("Enter a password to encrypt your file :\n");
  // scanf("%s", &key);

  encryptWithXOR(userInputFileName, "encrypted/encryptedFile.txt");
  decryptWithXOR("encrypted/encryptedFile.txt", userOutputFileName);


  printf("##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
