# -*- coding: latin-1 -*-

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
            decryptedMessage += dictionary[(dictionary.find(value)+len(dictionary)-shift)%len(dictionary)]
        else :
            decryptedMessage += message[i]
    return decryptedMessage



#####################
# MAIN
#####################

print("message d'origine : " + textToEncrypt)
encryptedMessage = rotateMessageWithShift(textToEncrypt, 10)
print("message chiffré : " + encryptedMessage)
print("shift = " + str(getShift(encryptedMessage)))
print("message déchiffré : " + rotateMessageWithShift(encryptedMessage, getShift(encryptedMessage)))
