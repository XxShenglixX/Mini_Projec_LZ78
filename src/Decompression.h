#ifndef Decompression_H
#define Decompression_H


#include "Dictionary.h"
#include "Stream.h"


char convertIntToChar(unsigned int inputdata);
int getAPairOfIndexAndData(Dictionary *dictionary, InStream *in);
int addDataToDictionary(Dictionary *dictionary, unsigned int data);



#endif // Decompression_H
