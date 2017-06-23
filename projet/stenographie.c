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

 /*
     This methods crypts the buffer passed as parameter
     with a xor on the last crypted block, then a xor on the key
 */
 int xorBuffer(int length, char *destination, char *buffer, char *lastCryptedBlock, char *key, bool crypt) {
     int i = 0;

     // for(i = 0; i < length; i ++) {
     //     if(!crypt) {
     //         destination[i] = (char)(buffer[i] ^ key[i]);
     //     }
     // }

     // printf(" After part 1 : %s\n", destination);

     for(i = 0; i < length; i ++) {
         /* CBC : Xor on last crypted block */
         destination[i] = buffer[i] ^ lastCryptedBlock[i];
     }

     // for(i = 0; i < length; i ++) {
     //     if(crypt) {
     //         destination[i] = (char)(destination[i] ^ key[i]);
     //     }
     // }
     return i;
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

 int dechiffre(char *cryptedBuffer, int cryptedBufferLength, char *decryptedFilePath, char *key) {

     /* Get key length */
     int len = (sizeof(char) * strlen(key));

     /* Open the destination file in write mode */
     FILE *fileWrite = fopen(decryptedFilePath, "wb");
     if (fileWrite == NULL) {
         printf("Error opening destination file!\n");
         exit(1);
     }

     /* Store the last crypted block in this
         We initialize this with our key to have an IV */
     char *lastXoredBlock = malloc(sizeof(char) * len);
     if(lastXoredBlock == NULL) {
         printf("Allocation Error !\n");
         exit(1);
     }
     memcpy(lastXoredBlock, key, len);

     /* The read buffer */
     char *buffer = malloc(sizeof(char) * len);
     if(buffer == NULL) {
         printf("Allocation Error !\n");
         exit(1);
     }

     /* The decrypted read buffer */
     char *decryptedBuffer = malloc(sizeof(char) * len);
     if(decryptedBuffer == NULL) {
         printf("Allocation Error !\n");
         exit(1);
     }

     /* set all entries in buffer to NULL by default*/
     memset(buffer, 0, len);
     memset(decryptedBuffer, 0, len);

     int count = 0;
     int i = 0;
     int padding = cryptedBufferLength % len;
     int paddingFrom = padding > 0 ? cryptedBufferLength - padding : cryptedBufferLength;

     printf("Padding %d, from : %d", padding, paddingFrom);

     for(i = 0; i <= cryptedBufferLength; i++) {
         // printf("Buffer %d => %c \n", i, cryptedBuffer[i]);
         printf (" i : %d // count : %d \n", i, count);
         if(count < len) {
             printf("Fill buffer with %c \n", cryptedBuffer[i]);
             /* fill buffer */
             buffer[count++] = cryptedBuffer[i];
         }
         if(count == len || (i > paddingFrom && count == padding)) {
             printf("__________\n");
             printf("decrypt buffer \n");
             /* Buffer full, let's decrypt it */
             int decryptedBytes = 0;
             if(i < paddingFrom) {
                 decryptedBytes = xorBuffer(len, decryptedBuffer, buffer, lastXoredBlock, key, false);
             } else {
                 printf(" IN PADDING \n");
                 decryptedBytes = xorBuffer(padding, decryptedBuffer, buffer, lastXoredBlock, key, false);
             }

             // printf("Decrypted %d bytes : %s\n", decryptedBytes, decryptedBuffer);
             memcpy(lastXoredBlock, buffer, len);
             fwrite(decryptedBuffer, sizeof(char), decryptedBytes, fileWrite);

             /* Reset for next buffer */
             count = 0;
             memset(buffer, 0, len);
             memset(decryptedBuffer, 0, len);
         }
     }



     free(buffer);
     free(decryptedBuffer);
     free(lastXoredBlock);
     fclose(fileWrite);

     return 0;
 }

 /*
     This method crypts or decrypts (depending on "crypt" bool)
     the source file and write the result in destination file
     using the key parameter, in CBC mode
 */
 int chiffre(char *sourceFilePath, char *destFilePath, char *key, bool crypt) {

     /* Open source file in read mode */
     FILE *fileRead = fopen(sourceFilePath, "rb");
     if (fileRead == NULL) {
         perror("Error opening source file!\n");
         exit(1);
     }

     int nbCharsInFile = getFileNumberOfChars(sourceFilePath);
     char *cryptedBuffer = malloc(sizeof(char) * nbCharsInFile);
     memset(cryptedBuffer, 0, nbCharsInFile);
     int count = 0;

     printf("Number of chars in file : %d \n", nbCharsInFile);

     /* Get key length */
     int len = (sizeof(char) * strlen(key));
     printf("- Key length : %d \n", len);

     /* Store the last crypted block in this
         We initialize this with our key to have an IV */
     char *lastXoredBlock = malloc(sizeof(char) * len);
     if(lastXoredBlock == NULL) {
         printf("Allocation Error !\n");
         exit(1);
     }
     memcpy(lastXoredBlock, key, len);

     /* The read buffer */
     char *buffer = malloc(sizeof(char) * len);
     size_t bytesRead = 0;

     /* set all entries in buffer to NULL by default*/
     memset(buffer, 0, len);

     printf("- %s content ...  \n", crypt ? "Crypting" : "Decrypting");

     char *crypted = malloc(sizeof(char) * len);
     if(crypted == NULL) {
         printf("Allocation Error !\n");
         exit(1);
     }

     /* Read file using buffer */
     while ((bytesRead = fread(buffer, sizeof(char), len, fileRead)) > 0)
     {
         int cryptedBytes = xorBuffer((int)bytesRead, crypted, buffer, lastXoredBlock, key, crypt);
         //  printf(" decrypted : %s\n", decrypted);
         // printf("Crypted => %s\n", crypted);

         // strncat(cryptedBuffer, crypted, bytesRead);
         printf("cryptedBufferContent %s \n", cryptedBuffer);
         int i = 0;
         for(i = 0; i < bytesRead; i++) {
             /* We fill the globalBuffer with the crypted buffer */
             cryptedBuffer[count++] = crypted[i];
         }
         // count+= bytesRead;

         // Store last crypted block for next iteration
         memcpy(lastXoredBlock, crypted, len);

         memset(buffer, 0, bytesRead);
         memset(crypted, 0, len);
     }

     /* Close our files */
     fclose(fileRead);


     // ---------------------TEST------------------------
     // DONE Crypting buffer, let's decrypt it
     dechiffre(cryptedBuffer, count, destFilePath, key);
     // ------------------- ENDTEST ---------------------

     // Finally we free our allocations
     free(lastXoredBlock);
     free(buffer);
     free(crypted);
     free(cryptedBuffer);

     return 0;
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
        memcpy(output, encryptedBytes, endOfFilePadding);
      } else {
        fwrite(encryptedBytes, 1, strlen(userKey), outputFile);
        memcpy(output, encryptedBytes, strlen(userKey));
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
void decryptAZ(char *input, int inputSize, char *outputFileName, char *userKey) {
  printf("• START DECRYPTING\n");
  printf("DECRYPT INPUT SIZE : %d\n", inputSize);
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
        if(iterations*4+bufferCounter<inputSize) {
          // add the current read byte to the buffer
          currentBytes[bufferCounter] = input[iterations*4+bufferCounter];
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
      if(endOfFilePadding != 0) {
        fwrite(decryptedBytes, 1, endOfFilePadding-1, outputFile);
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

  char test[50] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

  bool crypt = true;
  // char key[] = "password";
  char *key = "password";
  char sourceFilePath[] = "test.txt";
  char *crypted = malloc(sizeof(char)*getFileNumberOfChars(sourceFilePath));
  // encryptAZ(sourceFilePath, crypted, key);
  chiffre(sourceFilePath, crypted, key, crypt);
  printf("CRYPTED :%s\n", crypted);

  insertDataInBMPData(crypted, "test32b.bmp", getFileNumberOfChars(sourceFilePath));

  struct retrievedData azeaze = retrieveDataInBMPData("test32bOutput.bmp");

    printf("AZZAEZAEAZEZAE : %d\n", azeaze.size);
      printf("AZZAEZAEAZEZAE : %s\n", azeaze.data);
  // decryptAZ(azeaze.data, azeaze.size, "DECRYPTEDTEXT.txt", key);
  dechiffre(azeaze.data, azeaze.size, "azeaze.txt", key);


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
