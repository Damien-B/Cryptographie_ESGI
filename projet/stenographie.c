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

 int getFileNumberOfChars(char *filePath) {
     FILE *source = fopen(filePath, "r");
     if (source == NULL) {
         printf("Error opening file!\n");
         exit(1);
     }

     fseek(source, 0, SEEK_END);
     int byteCount = ftell(source);
     fclose(source);

     return byteCount;
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
void encryptData(char *inputFileName, char *output, char *userKey) {
  printf("• START ENCRYPTING\n");
  // opening files
  FILE *inputFile = fopen(inputFileName, "rb");

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
      if(endOfFilePadding != 0) {
        int a;
        for(a=0;a<endOfFilePadding;a++) {
          output[iterations*endOfFilePadding+a] = encryptedBytes[a];
        }
      } else {
        int a;
        for(a=0;a<strlen(userKey);a++) {
          output[iterations*strlen(userKey)+a] = encryptedBytes[a];
        }
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

    // close the file
    fclose(inputFile);
  }


  printf("• ENCRYPTING FINISHED\n");
}


// function used to decrypt a file
void decryptData(char *input, int inputSize, char *outputFileName, char *userKey) {
  printf("• START DECRYPTING\n");

  // opening file
  FILE *outputFile = fopen(outputFileName, "wb");

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
        currentBytes[bufferCounter] = (char)(unsigned int)input[iterations*strlen(userKey)+bufferCounter];
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
    } else {
      // decrypt with the last encrypted bytes
      encryptDecryptWithXOR(currentBytes, decryptedBytes, key, notEndOfFile==1?strlen(userKey):endOfFilePadding);
      // doing a XOR with the key
      encryptDecryptWithXOR(decryptedBytes, decryptedBytes, previousBytes, notEndOfFile==1?strlen(userKey):endOfFilePadding);
    }

    // write the bytes in the output file
    fwrite(decryptedBytes, 1, notEndOfFile==1?strlen(userKey):endOfFilePadding, outputFile);

    // set previous bytes to use for the next iteration
    memcpy(previousBytes, currentBytes, strlen(userKey));

    iterations++;
  }
  // free allocated memory (previous malloc)
  free(key);
  free(currentBytes);
  free(decryptedBytes);
  free(previousBytes);

  // close the file
  fclose(outputFile);

  printf("• DECRYPTING FINISHED\n");
}



void insertDataInBMPData(char *encryptedDataBuffer, char *bmpFileName) {
    int bmpBytesCount = getFileNumberOfChars(bmpFileName);
    printf("bmpBytesCount %d\n", bmpBytesCount);
    char *bmpWithEncryptedDataBuffer = malloc(sizeof(char)*bmpBytesCount);
    // open the bmp file to read
    FILE *inputFile = fopen(bmpFileName, "rb");
    if(inputFile != NULL) {

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
                  printf("PUT SIZE IN BMP %c\n", (unsigned char)bmpBytesCount);
                  bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = (unsigned char)bmpBytesCount;
                } else {
                  // add the current read byte to the buffer without changes
                  bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = currentChar;
                }
              } else {
                if(bytesCounter==3) {
                  // insert data in the 4th byte
                  if(iterations<bmpBytesCount) {
                    bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = encryptedDataBuffer[iterations-1];
                  } else {
                    bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = currentChar;
                  }
                } else {
                  // add the current read byte to the buffer without changes
                  bmpWithEncryptedDataBuffer[iterations*4+bytesCounter+54] = currentChar;
                }
              }

            } else {// get out of the while loop
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

    }
    FILE *writeInputFile = fopen(bmpFileName, "wb");
    if(writeInputFile != NULL) {
      fwrite(bmpWithEncryptedDataBuffer, sizeof(char), bmpBytesCount, writeInputFile);
    }
    fclose(writeInputFile);
}

int getSizeOfHiddenFileInBMP(char *bmpFileName) {
  FILE *inputFile = fopen(bmpFileName, "rb");
    fseek(inputFile, 57, SEEK_SET);
    int size = fgetc(inputFile);
    int intSize = (unsigned int)size;
    fclose(inputFile);
    return (int)intSize;
}

struct retrievedData retrieveDataInBMPData(char *bmpFileName) {
    int encryptedBytesSize = getSizeOfHiddenFileInBMP(bmpFileName);
    printf("encryptedBytesSize : %d\n", encryptedBytesSize);
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
      fseek(inputFile, 58, SEEK_SET);


      while(notEndOfFile) {
        // creating a buffer of 4 bytes
        for(bytesCounter=0;bytesCounter<4;bytesCounter++) {
            if((currentChar = fgetc(inputFile)) != EOF) {// test if we are at the end of file
              // if(iterations!=0) {
                if(bytesCounter==3) {
                  // insert data in the 4th byte
                  if(iterations<encryptedBytesSize) {
                    printf("CURRENT RETRIEVED CHAR : %c\n", currentChar);
                    encryptedDataBuffer[iterations] = currentChar;
                  } else {// get out of the while loop
                    sizeOfDecrypted = iterations+58+bytesCounter;
                    notEndOfFile = 0;
                    break;
                  }
                }
              // }

            } else {// get out of the while loop
              sizeOfDecrypted = iterations+54;
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

    }


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
  char sourceFilePath[] = "test.txt";




  char userBMPFileName[256];
  char userDataFileName[256];
  char userKey[256];
  char type;

  // user choose if he wants to encrypt, decrypt, or test the program
  while(type != 'h' && type != 'e') {
    printf("Do you want to hide (type h) or extract (type e) a file ?\n");
    scanf("%c", &type);
    printf("\n");
  }
  if(type == 'h') {
    // user give the name of the BMP file
    printf("Enter the name of the BMP you want to use (with the extension) :\n");
    scanf("%s", userBMPFileName);
    // user give the name of the input file
    printf("Enter the name of the file you want to hide (with the extension) :\n");
    scanf("%s", userDataFileName);
    // user give the password
    printf("Enter a password :\n");
    scanf("%s", userKey);
    printf("\n");

    char *crypted = malloc(sizeof(char)*getFileNumberOfChars("test.txt"));
    encryptData(userDataFileName, crypted, userKey);
    insertDataInBMPData(crypted, userBMPFileName);

  } else {
    // user give the name of the BMP file
    printf("Enter the name of the BMP you want to extract datas (with the extension) :\n");
    scanf("%s", userBMPFileName);
    // user give the name of the output file
    printf("Enter the name of the output file (with the extension) :\n");
    scanf("%s", userDataFileName);
    // user give the password
    printf("Enter a password :\n");
    scanf("%s", userKey);
    printf("\n");

    struct retrievedData dataFromBMP = retrieveDataInBMPData(userBMPFileName);
    printf("dataFromBMP : %s | dataFromBMP size : %d\n", dataFromBMP.data, dataFromBMP.size);
    decryptData(dataFromBMP.data, dataFromBMP.size, userDataFileName, userKey);
  }


  printf("\n##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
