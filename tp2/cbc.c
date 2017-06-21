#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITERATIONS 3
#define MASKLENGTH 8

char key[] = "passwordazfaefefezfze";

// test function, not used
// void printCharInBits(char input) {
//   printf("PRINTING (in bits) : %c\n", input);
//   int i;
//   for(i=0;i<8;i++){
//       printf("%d", (input & (1 << i)) >> i);
//       if(i==7) {
//         printf("\n");
//       }
//    }
// }

void encryptDecryptWithXOR(char *input, char *output, char *key) {

  int i;
  for(i=0;i<strlen(input);i++) {
    // printf("okokok %d\n", i);
    output[i] = input[i] ^ key[i];
    // printf("KOKO %c\n", input[i] ^ key[i]);
    // printf("KOKO %c\n", output[i]);
  }
  printf("input : %s\n", input);
  printf("output : %s\n", output);
  printf("key : %s\n", key);
  // return;
}

void encrypt(char *inputFileName, char *outputFileName) {
  FILE *inputFile;
  inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen(outputFileName, "wb");

  if(inputFile==NULL) {
    printf("NOTHING IN FILE\n");
  } else {
    printf("FILE %s LOADED\n", inputFileName);

    int currentChar;
    char currentBytes[strlen(key)];
    char digestedBytes[strlen(key)];
    // printf("digestedBytes : %lu\n", strlen(digestedBytes));
    int i = 0;
    int j = 0;
    while(1)
    {
      currentChar = fgetc(inputFile);
      if(currentChar == EOF) {
        // encrypt the bytes & put it in the output file
        char encryptedBytes[strlen(key)];
        encryptDecryptWithXOR(currentBytes, encryptedBytes, digestedBytes);
        fputs(encryptedBytes, outputFile);
        break;
      }
      // check if our stocked bytes are the same size as the key
      if(i>=strlen(key)) {
        // printf("i = %d\n", i);
        // reinitialize the counter
        i = 0;
        // encrypt the bytes
        char encryptedBytes[strlen(key)];
        // printf("strlenkey : %lu\n", strlen(key));
        // printf("currentBytes : %lu\n", strlen(currentBytes));
        // printf("encryptedBytes : %lu\n", strlen(encryptedBytes));
        if(j==0) {// encrypt with the key
          encryptDecryptWithXOR(currentBytes, encryptedBytes, key);
        } else {
          encryptDecryptWithXOR(currentBytes, encryptedBytes, digestedBytes);
        }

        // put the encrypted bytes in the output file
        fputs(encryptedBytes, outputFile);
        // use the encrypted bytes as digested bytes for the next iterative encryption
        memcpy(digestedBytes, encryptedBytes, strlen(key));
        // printf("digestedBytes : %lu\n", strlen(digestedBytes));
        // printf("encryptedBytes : %lu\n", strlen(encryptedBytes));
        // printf("key : %lu\n", strlen(key));
        j++;
      }

      char byte = currentChar;
      // printf("byte added : %c\n", byte);
      currentBytes[i] = byte;
      i++;
    }
  }

  fclose(inputFile);
  fclose(outputFile);
}

void decrypt(char *inputFileName, char *outputFileName) {
  FILE *inputFile;
  inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen(outputFileName, "wb");

  if(inputFile==NULL) {
    printf("NOTHING IN FILE\n");
  } else {
    printf("FILE %s LOADED\n", inputFileName);

    int currentChar;
    char currentBytes[strlen(key)];
    char digestedBytes[strlen(key)];
    char previousBytes[strlen(key)];
    int i = 0;
    int j = 0;

    while(1)
    {
      currentChar = fgetc(inputFile);
      if(currentChar == EOF) {
        // decrypt the bytes & put it in the output file
        char encryptedBytes[strlen(key)];
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousBytes);
        fputs(encryptedBytes, outputFile);
        break;
      }
      char byte = currentChar;
      // check if our stocked bytes are the same size as the key
      if(i>=strlen(key)) {
        // reinitialize the counter
        i = 0;
        // decrypt the bytes
        char encryptedBytes[strlen(key)];
        if(j==0) {// encrypt with the key
          encryptDecryptWithXOR(currentBytes, encryptedBytes, key);
        } else {
          encryptDecryptWithXOR(currentBytes, encryptedBytes, previousBytes);
        }
        // put the decrypted bytes in the output file
        fputs(encryptedBytes, outputFile);
        // use the current encrypted bytes as previous bytes for the next iterative decryption
        memcpy(previousBytes, currentBytes, strlen(key));
        j++;
      }

      currentBytes[i] = byte;
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

  // printf("Enter the name of the file you want to encrypt (with the extension) :\n");
  // scanf("%s", userInputFileName);
  // printf("Enter the name of the file you want to encrypt (with the extension) :\n");
  // scanf("%s", userOutputFileName);

  // printf("Enter a password to encrypt your file :\n");
  // scanf("%s", &key);

  encrypt("test.txt", "encrypted/encryptedFile.txt");
  decrypt("encrypted/encryptedFile.txt", "test2.txt");


  printf("##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
