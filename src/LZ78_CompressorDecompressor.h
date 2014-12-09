#ifndef LZ78_CompressorDecompressor_H
#define LZ78_CompressorDecompressor_H

#include "Stream.h"
#include "Compression.h"
#include "LZ78_Decompressor.h"
#include "Decompression.h"

void LZ78_CompressorDecompressor(char *InfileName,char *OutfileName,int dictSize,int mode);

#endif // LZ78_CompressorDecompressor_H
