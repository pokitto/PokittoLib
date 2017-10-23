/**************************************************************************/
/*!
    @file     WriteWav.h
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2015, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "WriteWav.h"


void writeSwapped(uint16_t data, uint16_t numbytes, FILE *outfile)
{
    unsigned char b;
    while(numbytes>0)
    {
        b = (unsigned char) data & 0xff;
        fwrite(&b, 1,1, outfile);
        b = (unsigned char) (data >>= 8);
        fwrite(&b, 1,1, outfile);
        numbytes-=2;
    }
}

void writeWav(char* rawfile, char * filename, uint16_t inputrate)
{
    FILE* outfile;
    FILE* infile;

    uint32_t filelen = 0;
    uint16_t samplerate;
    uint32_t numsamples;
    uint32_t i;

    if (inputrate<=0) samplerate = 44100;
    else samplerate = (uint16_t) inputrate;

    outfile = fopen(filename, "w");
    infile = fopen(rawfile, "r");

    if (infile == NULL) {
        printf("Failed to open input sound raw file.\n");
        return;
    }

     if (outfile == NULL) {
        printf("Failed to create an output wav file.\n");
        return;
    }


    fseek(infile, 0, SEEK_END);
    filelen = ftell(infile);
    fseek(infile, 0, 0);
    numsamples = filelen;

    // RIFF chunk descriptor
    fwrite("RIFF", 1, 4, outfile);                  //Chunk ID = offset 0, size 4b
    writeSwapped(36 + 2 * numsamples, 4, outfile);  //ChunkSize = size of FMT SubChunk + size of data SubChunk2)
    fwrite("WAVE", 1, 4, outfile);                  //Format specifier

    // FMT SubChunk
    fwrite("fmt ", 1, 4, outfile);                  //Contains the letters "fmt "
    writeSwapped(16, 4, outfile);                   //Size of the rest of this subchunk, VALUE = 16 for PCM
    writeSwapped(1, 2, outfile);                    //Audio format 1=PCM
    writeSwapped(1, 2, outfile);                    //Number of channels
    writeSwapped(samplerate, 4, outfile);           //sample rate 4 bytes
    writeSwapped(samplerate*2, 4, outfile);         //byte rate (simply 2 x samplerate in this case)
    writeSwapped(2, 2, outfile);                    //Sample data block align, 1 channel * 16/8 bits per sample
    writeSwapped(16, 2, outfile);                   //Bits per sample

    fwrite("data", 1, 4, outfile);                  //data SubChunk2 marker
    writeSwapped( 2 * numsamples, 4, outfile);      //Subchunk2Size = 2 bytes * number of samples
    for (i=0; i< numsamples; i++)                   //write the actual data
    {
        unsigned char data;
        data = fgetc(infile);
        signed int d2 = (data-127)*256;
        writeSwapped((unsigned int)d2,2, outfile);
    }

    fclose(outfile);
    fclose(infile);
}
