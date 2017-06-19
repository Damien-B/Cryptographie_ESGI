# -*- coding: latin-1 -*-

# we are taking only lowercases and uppercases, no special characters
dictionary = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
textToEncrypt = "La frequence des lettres dans un texte differe de celle de la liste des mots."

from collections import Counter

# get shift of a given string (in french)
def getShift(message) :
    counter = Counter(message)
    finalCounter = ({})
    maxFrequency = 0
    maxFrequencyValue = None

    # create a dictionary with the letters frequencies
    for key, value in counter.iteritems() :
        # remove characters not in our dictionary
        if dictionary.find(key) != -1 :
            finalCounter[key] = value

    # detect which letter appear the most
    for key, value in finalCounter.iteritems() :
        if value > maxFrequency :
            maxFrequency = value
            maxFrequencyValue = key

    # return the shift
    return dictionary.find(maxFrequencyValue) - dictionary.find("e")

# encrypt/decrypt a message with a given shift (RTO)
def rotateMessageWithShift(message, shift) :
    decryptedMessage = ""
    for i, value in enumerate(message) :
        if dictionary.find(value) != -1 :
            valueIndexInDict = dictionary.find(value)
            newValueIndexInDict = valueIndexInDict+len(dictionary)+shift
            newValueIndexInDictSimplified = newValueIndexInDict%len(dictionary)
            decryptedMessage += str(dictionary[newValueIndexInDictSimplified])
        else :
            decryptedMessage += message[i]
    return decryptedMessage



#####################
# MAIN
#####################


userInput = raw_input("enter a message to encrypt : ")
userShift = raw_input("enter a number for the rotation (must be an integer) : ")
encryptedMessage = rotateMessageWithShift(userInput, int(userShift))
print("message chiffré : " + encryptedMessage)
print("shift = " + str(getShift(encryptedMessage)))
print("message déchiffré : " + rotateMessageWithShift(encryptedMessage, -getShift(encryptedMessage)))
