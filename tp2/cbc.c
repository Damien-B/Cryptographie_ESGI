/*
Program created by Damien Bannerot
5 MOC ESGI
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function used to encrypt an array of char with a key
void encryptDecryptWithXOR(char *input, char *output, char *keyToEncrypt, int length) {
  int i;
  for(i=0;i<length;i++) {
    // do XOR with input and key for each byte
    output[i] = (char)(input[i] ^ keyToEncrypt[i]);
  }
}

// function used to encrypt a file
void encrypt(char *inputFileName, char *outputFileName, char *userKey) {
  printf("• START ENCRYPTING\n");
  // opening files
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
      // creating a buffer of bytes
      for(bufferCounter=0;bufferCounter<strlen(userKey);bufferCounter++) {
        // test if we are on the end of file
        if((currentChar = fgetc(inputFile)) != EOF) {
          // add the current read byte to the buffer
          currentBytes[bufferCounter] = currentChar;
        } else {// get out of the while loop
          notEndOfFile = 0;
          // set the endOfFilePadding to use as the size of the key to encrypt with a XOR
          endOfFilePadding = bufferCounter;
          break;
        }
      }
      if(iterations == 0) {// first bytes
        // first iteration : use the key as initialization vector
        encryptDecryptWithXOR(currentBytes, encryptedBytes, key, strlen(userKey));
        // doing a XOR with the key
        encryptDecryptWithXOR(encryptedBytes, encryptedBytes, key, strlen(userKey));
      } else if (endOfFilePadding == 0) {
        // encrypt with the last result we got
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousDigestedBytes, strlen(userKey));
        // doing a XOR with the key
        encryptDecryptWithXOR(encryptedBytes, encryptedBytes, key, strlen(userKey));
      } else {// last bytes
        // encrypt the end of the file (handling padding)
        encryptDecryptWithXOR(currentBytes, encryptedBytes, previousDigestedBytes, endOfFilePadding);
        // doing a XOR with the key
        encryptDecryptWithXOR(encryptedBytes, encryptedBytes, key, endOfFilePadding);
      }
      // write the bytes in the output file
      // fputs(encryptedBytes, outputFile);
      if(endOfFilePadding != 0) {
        fwrite(encryptedBytes, 1, endOfFilePadding, outputFile);
      } else {
        fwrite(encryptedBytes, 1, strlen(userKey), outputFile);
      }
      // set previous encrypted bytes to use for the next iteration
      memcpy(previousDigestedBytes, encryptedBytes, strlen(userKey));

      iterations++;
    }
    // free allocated memory (previous malloc)
    free(key);
    free(currentBytes);
    free(encryptedBytes);
    free(previousDigestedBytes);
  }

  // close the files
  fclose(inputFile);
  fclose(outputFile);

  printf("• ENCRYPTING FINISHED\n");
}

// function used to decrypt a file
void decrypt(char *inputFileName, char *outputFileName, char *userKey) {
  printf("• START DECRYPTING\n");
  // opening files
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
    char *decryptedBytes = malloc(strlen(userKey));
    char *previousBytes = malloc(strlen(userKey));
    int notEndOfFile = 1;
    int endOfFilePadding = 0;

    while(notEndOfFile) {
      // creating a buffer of bytes
      for(bufferCounter=0;bufferCounter<strlen(userKey);bufferCounter++) {
        // test if we are on the end of file
        if((currentChar = fgetc(inputFile)) != EOF) {
          // add the current read byte to the buffer
          currentBytes[bufferCounter] = currentChar;
        } else {// get out of the while loop
          notEndOfFile = 0;
          // set the endOfFilePadding to use as the size of the key to decrypt with a XOR
          endOfFilePadding = bufferCounter;
          break;
        }
      }
      if(iterations == 0) {// first bytes
        // first iteration : use the key as initialization vector
        encryptDecryptWithXOR(currentBytes, decryptedBytes, key, strlen(userKey));
        // doing a XOR with the key
        encryptDecryptWithXOR(decryptedBytes, decryptedBytes, key, strlen(userKey));
      } else if (endOfFilePadding == 0) {
        // decrypt with the last encrypted bytes
        encryptDecryptWithXOR(currentBytes, decryptedBytes, key, strlen(userKey));
        // doing a XOR with the key
        encryptDecryptWithXOR(decryptedBytes, decryptedBytes, previousBytes, strlen(userKey));
      } else {// last bytes
        // decrypt the end of the file (handling padding)
        encryptDecryptWithXOR(currentBytes, decryptedBytes, key, endOfFilePadding);
        // doing a XOR with the key
        encryptDecryptWithXOR(decryptedBytes, decryptedBytes, previousBytes, endOfFilePadding);
      }
      // write the bytes in the output file
      // fputs(decryptedBytes, outputFile);
      if(endOfFilePadding != 0) {
        fwrite(decryptedBytes, 1, endOfFilePadding, outputFile);
      } else {
        fwrite(decryptedBytes, 1, strlen(userKey), outputFile);
      }
      // set previous bytes to use for the next iteration
      memcpy(previousBytes, currentBytes, strlen(userKey));

      iterations++;
    }
    // free allocated memory (previous malloc)
    free(key);
    free(currentBytes);
    free(decryptedBytes);
    free(previousBytes);
  }

  // close the files
  fclose(inputFile);
  fclose(outputFile);

  printf("• DECRYPTING FINISHED\n");
}


int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n\n");

  char userInputFileName[256];
  char userOutputFileName[256];
  char userKey[] = "";
  char type;

  // user choose if he wants to encrypt, decrypt, or test the program
  while(type != 'e' && type != 'd' && type != 'a') {
    printf("Do you want to encrypt (type e) or decrypt (type d) a file ? (type a to launch in automatic mode, for test purpose)\n");
    scanf("%c", &type);
    printf("\n");
  }
  if(type == 'a') {
    // launch with test files
    encrypt("test.txt", "testEncrypted.txt", "randomPass");
    decrypt("testEncrypted.txt", "testDecrypted.txt", "randomPass");
    encrypt("test.bmp", "testEncrypted.bmp", "randomPass");
    decrypt("testEncrypted.bmp", "testDecrypted.bmp", "randomPass");
  } else {
    // user give the name of the input file
    printf("Enter the name of the file you want to %s (with the extension) :\n", type=='e'?"encrypt":"decrypt");
    scanf("%s", userInputFileName);
    // user give the name of the output file
    printf("Enter the name of the file you want to generate (with the extension) :\n");
    scanf("%s", userOutputFileName);
    // user give the password
    printf("Enter a password to %s your file :\n", type=='e'?"encrypt":"decrypt");
    scanf("%s", userKey);
    printf("\n");

    if(type == 'e') {
      encrypt(userInputFileName, userOutputFileName, userKey);
    } else if(type == 'd') {
      decrypt(userInputFileName, userOutputFileName, userKey);
    }
  }

  printf("\n##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
