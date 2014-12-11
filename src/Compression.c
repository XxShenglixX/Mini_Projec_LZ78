#include "Compression.h"
#include "Dictionary.h"
#include "Stream.h"
#include <string.h>
#include <stdio.h>

/* LZ78 Compressor
 *
 *
 * Input :	dictionary	:	dictionary is the pointer to the LZ78 dictionary
 *			in			:	in is the pointer to InStream
 *			out			:	out is the pointer to OutStream
 *			mode 		:	Fixed -> fixed output dictionary index to 16bits
 *							Variable -> use just sufficient number of bits to represent the dictionary index
 */
void LZ78_Compressor(Dictionary *dictionary, InStream *in, OutStream *out, int mode)
{       
    char readByte[2] ={}, dataString[1024] ;
    int returnedIndex, lastIndex, EOFstate = 0 , i = 0;

    while (1)
    {   
        readByte[0] = (char)(streamReadBits(in,8));
        
        if (checkEndOfFile(in)) // if EOF encountered
            break;  // break loop
        
        if(isDictionaryFull(dictionary)) 
            refreshDictionaryEntryData(dictionary,dictionary->dictionarySize); 
            
        if(isDictionaryEmpty(dictionary)) // if dictionary is empty
        {
            addEntryData(dictionary, readByte); // directly add it into dictionary
            LZ78_Output(dictionary,out,readByte[0],0,EOFstate,mode); // output (0,x) *without ()
        }       
        else // dictionary is not empty
        {
            returnedIndex = compare_DictionaryData(readByte,dictionary); //check is there any matched data in dictionaryEntry
            if ( returnedIndex >= 0 ) // if true
            {
                lastIndex = findLastMatchEntry(dictionary,in,dataString,readByte,&returnedIndex, &EOFstate);
                
                if (EOFstate != 1)//prevent adding EOF into dictionary
                    addEntryData(dictionary,dataString); // add dataString into dictionary
                    
                LZ78_Output(dictionary,out,readByte[0],lastIndex+1,EOFstate,mode); // produce output (lastIndex+1 , X) *without ()
            }
            else // no matched data
            {
                addEntryData(dictionary,readByte);
                LZ78_Output(dictionary,out,readByte[0],0,EOFstate,mode); // output (1,x) *without ()
            }
        }
        
        if (EOFstate == 1) //EOF encountered previously 
            break ; //break while loop
        
    }
   
}

/* Produce output for LZ78 Compressor
 *
 *
 * Input :	out			:	out is the pointer to OutStream
 *			outputByte	:	outputByte is the byte of data to be output
 *			index		: 	index is the dictionary index
 *			EOFstate	: 	EOFstate is used to prevent writing invalid data into the file
 *			mode 		:	Fixed -> fixed output dictionary index to 16bits
 *							Variable -> use just sufficient number of bits to represent the dictionary index
 */
void LZ78_Output(Dictionary *dictionary,OutStream *out,char outputByte,int index,int EOFstate, int mode)
{
    int bitsRequired = 0 ;
    
    if (mode == Fixed)
        bitsRequired = 16 ;
    else if (mode == Variable)
        bitsRequired = determineNumberOfBitsRequired(dictionary->currentIndex - 1);
        
    streamWriteBits(out,index,bitsRequired);
    if (EOFstate == 0 ) // prevent writing EOF to file
        streamWriteBits(out,(unsigned int)(outputByte),8);       
}


/* *
 *  Compare data in the dictionary with input data
 *
 * 
 *	Input :	inputString	:	inputString is the pointer to the input data
 *		  	dictionary	:	dictionary is the pointer to the LZ78 dictionary
 *
 *	Output :	Return index of match entry if found
 * 				Return -1 if not found
 * 
 */
int compare_DictionaryData(char *inputString,Dictionary *dictionary)
{
    int i ;
    
    for ( i = 0 ; i < (dictionary->currentIndex) ; i ++ )
    {
        if (strcmp(inputString,(dictionary->Entry[i].data)) == 0)
            return i ;
    }

    return (-1);
}

/*  Merge data in the selected index of dictionaryEntry with input data
*
*	Input :	inputString	:	inputString is the pointer to the input data
*		  	dictionary	:	dictionary is the pointer to the LZ78 dictionary 
*/ 
void merge_InputDataDictionaryData(char *inputString,Dictionary *dictionary,int index)
{
   strcat(inputString,dictionary->Entry[index].data);
}


/*  Find the last match of dictionaryEntry
*
*  Return the index of the last match dictionaryEntry
*/ 
int findLastMatchEntry(Dictionary *dictionary, InStream *in, char *dataString, char *readByte, int *returnedIndex, int *EOFstate)
{
    int lastIndex = *returnedIndex ; // store the index of first match in dictionaryEntry
    
    memset (dataString,0,1024); //clear dataString
    merge_InputDataDictionaryData(dataString,dictionary,lastIndex); //merge input character with data in dictionary
    
    while(*returnedIndex != -1)
    {
        readByte[0] = (char)(streamReadBits(in,8)) ;// read next character

        if (checkEndOfFile(in)) //if EOF detected
        {    
            *EOFstate = 1 ; // use to remember EOF encountered for later uses
            *returnedIndex = -1 ; //quit loop
        }
        else    
        {
            strcat(dataString,readByte); //add next character to dataString
            *returnedIndex = compare_DictionaryData(dataString,dictionary); //check again is there any matched data
                    
            if (*returnedIndex != -1 )  // if there is still existing a match in dictionaryEntry
                lastIndex = *returnedIndex ; // store the index of last match in dictionaryEntry
        }
    }
    

    return lastIndex ;

}


