#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct retrievedData
{
    char *data;
    int size;
};


// debug function, not used
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



// function used to encrypt an array of char with a key
void encryptDecryptWithXOR(char *input, char *output, char *keyToEncrypt, int length) {
  int i;
  for(i=0;i<length;i++) {
    // do XOR with input and key for each byte
    output[i] = input[i] ^ keyToEncrypt[i];
  }
}

// function used to encrypt a file
void encryptAZ(char *inputFileName, char *output, char *userKey) {
  printf("• START ENCRYPTING\n");
  // opening files
  FILE *inputFile = fopen(inputFileName, "rb");
  FILE *outputFile = fopen("ENCRYPTEDTEST.txt", "wb");

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
        int a;
        for(a=0;a<endOfFilePadding;a++) {
          output[iterations*endOfFilePadding+a] = encryptedBytes[a];
        }
        // memcpy(output, encryptedBytes, endOfFilePadding);
      } else {
        fwrite(encryptedBytes, 1, strlen(userKey), outputFile);
        int a;
        for(a=0;a<strlen(userKey);a++) {
          output[iterations*strlen(userKey)+a] = encryptedBytes[a];
        }
        // memcpy(output, encryptedBytes, strlen(userKey));
      }
      // set previous encrypted bytes to use for the next iteration
      memcpy(previousDigestedBytes, encryptedBytes, strlen(userKey));
            //
            // FILE *out = fopen("azeazezae2.txt", "wb");
            // fwrite(encry, 1, azeaze.size, out);
            // fclose(out);
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
void decryptAZ(char *input, int inputSize, char *outputFileName, char *userKey) {
  printf("• START DECRYPTING\n");
  printf("DECRYPT INPUT : %s\n", input);
  printf("DECRYPT INPUT SIZE : %d\n", inputSize);
  printf("DECRYPT KEY LENGTH : %lu\n", strlen(userKey));
  // opening files
  // FILE *inputFile = fopen(inputFileName, "rb");

  FILE *outputFile = fopen(outputFileName, "wb");

  // if(inputFile != NULL) {
    // printf("• FILE LOADED (%s)\n", inputFileName);

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
        if(iterations*strlen(userKey)+bufferCounter<inputSize) {
          // add the current read byte to the buffer
          printf("INPUT AT INDEX %lu : %c\n", iterations*strlen(userKey)+bufferCounter, input[iterations*strlen(userKey)+bufferCounter]);
          currentBytes[bufferCounter] = (char)(unsigned int)input[iterations*strlen(userKey)+bufferCounter];
        } else {// get out of the while loop
          notEndOfFile = 0;
          // set the endOfFilePadding to use as the size of the key to decrypt with a XOR
          endOfFilePadding = bufferCounter;
          break;
        }
      }
      printf("CURRENT BYTES TO DECRYPT : %s\n", currentBytes);
      if(iterations == 0) {// first bytes
        // first iteration : use the key as initialization vector
        encryptDecryptWithXOR(currentBytes, decryptedBytes, key, strlen(userKey));
        // doing a XOR with the key
        encryptDecryptWithXOR(decryptedBytes, decryptedBytes, key, strlen(userKey));
      } else if (notEndOfFile == 1) {
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
      printf("DECRYPTED BYTES BUFFER : %s\n", decryptedBytes);
      // write the bytes in the output file
      if(endOfFilePadding != 0) {
        printf("endOfFilePadding : %d\n", endOfFilePadding);
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
  // }

  // close the files
  // fclose(inputFile);
  fclose(outputFile);

  printf("• DECRYPTING FINISHED\n");
}



void insertDataInBMPData(char *encryptedDataBuffer, char *bmpFileName, int sizeOfData) {
    int bmpBytesCount = getFileNumberOfChars(bmpFileName);
    // open the bmp file to read
    FILE *inputFile = fopen(bmpFileName, "rb");
    if(inputFile != NULL) {

      // get size of the bmp file in bytes
      // fseek(inputFile, 0, SEEK_END);
      // int bmpBytesCount = ftell(inputFile);
      // fseek(inputFile, 0, SEEK_SET);

      // create a copy of the bmp file
      // char *bmpWithEncryptedDataBuffer;
      // commented try to handle larger data than what we can handle in the 4th bytes
      // if(sizeOfData>(bmpBytesCount-54)/4) {
      //   printf("BIGGER");
      //   bmpWithEncryptedDataBuffer = malloc(sizeof(char)*(bmpBytesCount+sizeOfData-(bmpBytesCount-54)/4));
      // } else {
        // bmpWithEncryptedDataBuffer = malloc(sizeof(char)*bmpBytesCount);
      // }
      char *bmpWithEncryptedDataBuffer = malloc(sizeof(char)*bmpBytesCount);
      memcpy(bmpWithEncryptedDataBuffer, inputFile, bmpBytesCount);

      int currentChar;
      int bytesCounter;
      int iterations = 0;
      int notEndOfFile = 1;
      int endOfFilePadding = 0;

      // set header in the bmp buffer
      char *headerBytes = malloc(54);
      fread(headerBytes, 1, 54, inputFile);
      int i;
      for(i=0;i<54;i++) {
        bmpWithEncryptedDataBuffer[i] = headerBytes[i];
      }



      // set read cursor just after header
      fseek(inputFile, 54, SEEK_SET);

      while(notEndOfFile) {
        // creating a buffer of 4 bytes
        for(bytesCounter=0;bytesCounter<4;bytesCounter++) {
            if((currentChar = fgetc(inputFile)) != EOF) {// test if we are at the end of file
              if(iterations==0) {
                if(bytesCounter==3) {// reserved byte for encrypted bytes count (max 256)
                  // printCharInBits(currentChar);
                  bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = (unsigned char)sizeOfData;
                } else {
                  // add the current read byte to the buffer without changes
                  bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = currentChar;
                }
              } else {
                if(bytesCounter==3) {
                  // printCharInBits(currentChar);
                  // insert data in the 4th byte
                  if(iterations<sizeOfData) {
                    bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = encryptedDataBuffer[iterations-1];
                  } else {
                    // printf("CURRENTCHAR: %c\n", currentChar);
                    bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = currentChar;
                  }
                } else {
                  // add the current read byte to the buffer without changes
                  bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = currentChar;
                }
              }

            } else {// get out of the while loop
              // bmpWithEncryptedDataBuffer[bmpBytesCount] = currentChar;
              notEndOfFile = 0;
              break;
            }
        }
        iterations++;
      }
      // free allocated memory (previous malloc)
      free(headerBytes);

      // close read of the bmp
      fclose(inputFile);

      // commented try to handle larger data than what we can handle in the 4th bytes
      // if(sizeOfData>(bmpBytesCount-54)/4) {
      //   int k;
      //   for(k=0;k<sizeOfData-(bmpBytesCount-54)/4;k++) {
      //     bmpWithEncryptedDataBuffer[bmpBytesCount+k] = encryptedDataBuffer[(bmpBytesCount-54)/4+k];
      //   }
      // }


      FILE *writeInputFile = fopen("test32bOutput.bmp", "wb");
      if(writeInputFile != NULL) {
        fwrite(bmpWithEncryptedDataBuffer, sizeof(char), bmpBytesCount, writeInputFile);
        // int j;
        // for(j=0;j<bmpBytesCount;j++) {
        //   printf("put char %c in 4th byte\n", bmpWithEncryptedDataBuffer[i]);
        //   char currentChar[1];
        //   memcpy(currentChar, &bmpWithEncryptedDataBuffer[i], 1);
        //   fwrite(&currentChar, sizeof(char), 1, writeInputFile);
        // }
      }
      fclose(writeInputFile);
    }
}

int getSizeOfHiddenFileInBMP(char *bmpFileName) {
  FILE *inputFile = fopen(bmpFileName, "rb");
  // if(inputFile != NULL) {
    fseek(inputFile, 57, SEEK_SET);
    int size = fgetc(inputFile);
    int intSize = (unsigned int)size;
    fclose(inputFile);
    return (int)intSize;
  // }
  // return 0;
}

struct retrievedData retrieveDataInBMPData(char *bmpFileName) {
    int encryptedBytesSize = getSizeOfHiddenFileInBMP(bmpFileName);
    printf("encryptedBytesSize %d\n", encryptedBytesSize);
    int sizeOfDecrypted;
    char *encryptedDataBuffer = malloc(sizeof(char)*encryptedBytesSize);
    // open the bmp file to read
    FILE *inputFile = fopen(bmpFileName, "rb");
    if(inputFile != NULL) {

      // get size of the bmp file in bytes
      fseek(inputFile, 0, SEEK_END);
      int bmpBytesCount = ftell(inputFile);
      fseek(inputFile, 0, SEEK_SET);




      int currentChar;
      int bytesCounter;
      int iterations = 0;
      int notEndOfFile = 1;
      int endOfFilePadding = 0;

      // set read cursor just after header
      fseek(inputFile, 54, SEEK_SET);


      while(notEndOfFile) {
        // creating a buffer of 4 bytes
        for(bytesCounter=0;bytesCounter<4;bytesCounter++) {
            if((currentChar = fgetc(inputFile)) != EOF) {// test if we are at the end of file
              if(iterations!=0) {
                if(bytesCounter==3) {
                  // printCharInBits(currentChar);
                  // insert data in the 4th byte
                  if(iterations<encryptedBytesSize) {
                    encryptedDataBuffer[iterations-1] = currentChar;
                  } else {// get out of the while loop
                    sizeOfDecrypted = iterations;
                    notEndOfFile = 0;
                    break;
                  }
                }
              }

            } else {// get out of the while loop
              sizeOfDecrypted = iterations+bytesCounter;
              notEndOfFile = 0;
              break;
            }
        }
        iterations++;
      }

      // close read of the bmp
      fclose(inputFile);

      // commented try to handle larger data than what we can handle in the 4th bytes
      // if(sizeOfData>(bmpBytesCount-54)/4) {
      //   int k;
      //   for(k=0;k<sizeOfData-(bmpBytesCount-54)/4;k++) {
      //     bmpWithEncryptedDataBuffer[bmpBytesCount+k] = encryptedDataBuffer[(bmpBytesCount-54)/4+k];
      //   }
      // }


      // FILE *writeInputFile = fopen("TEXTOUTPUT.txt", "wb");
      // if(writeInputFile != NULL) {
      //   fwrite(encryptedDataBuffer, sizeof(char), encryptedBytesSize, writeInputFile);
      //   // int j;
      //   // for(j=0;j<bmpBytesCount;j++) {
      //   //   printf("put char %c in 4th byte\n", bmpWithEncryptedDataBuffer[i]);
      //   //   char currentChar[1];
      //   //   memcpy(currentChar, bmpWithEncryptedDataBuffer[i], 1);
      //   //   fwrite(currentChar, sizeof(char), 1, writeInputFile);
      //   // }
      // }
    }
    // free(encryptedDataBuffer);
    printf("writing");
    FILE *fileWrite = fopen("ouloulou.txt", "wb");
    if (fileWrite == NULL) {
        printf("Error opening destination file!\n");
        exit(1);
    }
    fwrite(encryptedDataBuffer, sizeof(char), sizeOfDecrypted, fileWrite);
    fclose(fileWrite);


    struct retrievedData data;
    data.size = sizeOfDecrypted;
    data.data = encryptedDataBuffer;
    free(encryptedDataBuffer);
    return data;
}




int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n\n");


  bool crypt = true;
  char key[] = "azeee";
  char sourceFilePath[] = "test.txt";
  char *crypted = malloc(sizeof(char)*getFileNumberOfChars(sourceFilePath));

  encryptAZ(sourceFilePath, crypted, key);
  insertDataInBMPData(crypted, "test32b.bmp", getFileNumberOfChars(sourceFilePath));
  struct retrievedData azeaze = retrieveDataInBMPData("test32bOutput.bmp");
  decryptAZ(azeaze.data, azeaze.size, "DECRYPTEDTEXT.txt", key);


  // char userInputFileName[256];
  // char userOutputFileName[256];
  // char userKey[] = "";
  // char type;
  //
  // // user choose if he wants to encrypt, decrypt, or test the program
  // while(type != 'e' && type != 'd' && type != 'a') {
  //   printf("Do you want to encrypt (type e) or decrypt (type d) a file ? (type a to launch in automatic mode, for test purpose)\n");
  //   scanf("%c", &type);
  //   printf("\n");
  // }
  // if(type == 'a') {
  //   // launch with test files
  //   encrypt("test.txt", "testEncrypted.txt", "randomPass");
  //   decrypt("testEncrypted.txt", "testDecrypted.txt", "randomPass");
  //   encrypt("test.bmp", "testEncrypted.bmp", "randomPass");
  //   decrypt("testEncrypted.bmp", "testDecrypted.bmp", "randomPass");
  // } else {
  //   // user give the name of the input file
  //   printf("Enter the name of the file you want to %s (with the extension) :\n", type=='e'?"encrypt":"decrypt");
  //   scanf("%s", userInputFileName);
  //   // user give the name of the output file
  //   printf("Enter the name of the file you want to generate (with the extension) :\n");
  //   scanf("%s", userOutputFileName);
  //   // user give the password
  //   printf("Enter a password to %s your file :\n", type=='e'?"encrypt":"decrypt");
  //   scanf("%s", userKey);
  //   printf("\n");
  //
  //   if(type == 'e') {
  //     encrypt(userInputFileName, userOutputFileName, userKey);
  //   } else if(type == 'd') {
  //     decrypt(userInputFileName, userOutputFileName, userKey);
  //   }
  // }

  printf("\n##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
