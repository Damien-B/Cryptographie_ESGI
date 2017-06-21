#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void encryptDecryptWithXOR(char *input, char *output, char *keyToEncrypt, int length) {
  // printf("XOR input '%s' of length %lu\n", input, strlen(input));
  // printf("XOR keyToEncrypt '%s' of length %lu\n", keyToEncrypt, strlen(keyToEncrypt));
  int i;
  for(i=0;i<length;i++) {
    output[i] = (char)(input[i] ^ keyToEncrypt[i]);
  }
  // printf("XOR output '%s' of length %lu\n", output, strlen(output));
}

void encrypt(char *inputFileName, char *outputFileName, char *userKey) {
  printf("• START ENCRYPTING\n");
  FILE *inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen(outputFileName, "wb");

  if(inputFile != NULL) {
    printf("• FILE LOADED (%s)\n", inputFileName);

    int currentChar;

    int bufferCounter;
    int iterations = 0;
    char *key = malloc(strlen(userKey));
    strcpy(key, userKey);
    char *currentBytes = malloc(strlen(userKey));
    char *encryptedBytes = malloc(strlen(userKey));
    char *previousDigestedBytes = malloc(strlen(userKey));
    int endOfFilePadding = 0;
    int notEndOfFile = 1;
    while(notEndOfFile) {
      for(bufferCounter=0;bufferCounter<strlen(userKey);bufferCounter++) {
        if((currentChar = fgetc(inputFile)) != EOF) {
          currentBytes[bufferCounter] = currentChar;
        } else {
          notEndOfFile = 0;
          endOfFilePadding = bufferCounter;
          break;
        }
      }
      if(iterations == 0) {
        encryptDecryptWithXOR(currentBytes, encryptedBytes, key, strlen(userKey));
      } else if (endOfFilePadding == 0) {
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousDigestedBytes, strlen(userKey));
      } else {
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousDigestedBytes, endOfFilePadding);
      }
      fputs(encryptedBytes, outputFile);
      memcpy(previousDigestedBytes, encryptedBytes, strlen(userKey));


      iterations++;
    }
  }

  fclose(inputFile);
  fclose(outputFile);
  printf("• ENCRYPTING FINISHED\n");
}


void decrypt(char *inputFileName, char *outputFileName, char *userKey) {
  printf("• START DECRYPTING\n");
  FILE *inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen(outputFileName, "wb");

  if(inputFile != NULL) {
    printf("• FILE LOADED (%s)\n", inputFileName);

    int currentChar;

    int bufferCounter;
    int iterations = 0;
    char *key = malloc(strlen(userKey));
    strcpy(key, userKey);
    char *currentBytes = malloc(strlen(userKey));
    char *encryptedBytes = malloc(strlen(userKey));
    char *previousBytes = malloc(strlen(userKey));
    int notEndOfFile = 1;
    int endOfFilePadding = 0;
    while(notEndOfFile) {
      for(bufferCounter=0;bufferCounter<strlen(userKey);bufferCounter++) {
        if((currentChar = fgetc(inputFile)) != EOF) {
          currentBytes[bufferCounter] = currentChar;
        } else {
          notEndOfFile = 0;
          endOfFilePadding = bufferCounter;
          break;
        }
      }
      if(iterations == 0) {
        encryptDecryptWithXOR(currentBytes, encryptedBytes, key, strlen(userKey));
      } else if (endOfFilePadding == 0) {
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousBytes, strlen(userKey));
      } else {
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousBytes, endOfFilePadding);
      }
      fputs(encryptedBytes, outputFile);
      memcpy(previousBytes, currentBytes, strlen(userKey));


      iterations++;
    }
  }

  fclose(inputFile);
  fclose(outputFile);
  printf("• DECRYPTING FINISHED\n");
}


int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n\n");

//   char userInputFileName[256];
//   char userOutputFileName[256];
//   char userKey[] = "";
//   char type;
//
//
//   while(type != 'e' && type != 'd') {
//     printf("Do you want to encrypt (type e) or decrypt (type d) a file ?\n");
//     scanf("%c", &type);
//     printf("\n");
//   }
// /*
//   printf("Enter the name of the file you want to %s (with the extension) :\n", type=='e'?"encrypt":"decrypt");
//   scanf("%s", userInputFileName);
//   printf("Enter the name of the file you want to generate (with the extension) :\n");
//   scanf("%s", userOutputFileName);
//
//   printf("Enter a password to %s your file :\n", type=='e'?"encrypt":"decrypt");
//   scanf("%s", userKey);
//   printf("\n");
//
//
//   */
//   if(type == 'e') {
//     // encrypt(userInputFileName, userOutputFileName, userKey);
//     encrypt("test.txt", "testencr.txt", "oiaufgbjl");
//   } else if(type == 'd') {
//     // decrypt(userInputFileName, userOutputFileName, userKey);
//     decrypt("testencr.txt", "testout.txt", "oiaufgbjl");
//   }

  encrypt("test.txt", "testencr", "password");
  decrypt("testencr", "testout.txt", "password");


  printf("\n##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
