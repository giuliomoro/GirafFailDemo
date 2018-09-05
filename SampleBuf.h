/***** SampleLoad.h *****/
#ifndef SAMPLEBUF_H_
#define SAMPLEBUF_H_

#include <SampleData.h>
#include <string>
#include <Bela.h>

#include <sndfile.h>	// to load audio files
#include <iostream>
#include <cstdlib>

#include <stdio.h>
#include <cmath>

using namespace std;

class SampleBuf
{

public:
    SampleBuf(const char* filename, int channels, int startFrame, int endFrame);
    SampleBuf(const char* filename, int channels);
    SampleBuf(const char* filename);
    ~SampleBuf();
   
   int openFile(const char* filename);
   int openFile(const char* filename, int channels);
   int openFile(const char* filename, int channels, int startFrame, int endFrame);
   
   	// Function to replace the buffer
   	int openSF(const char* filename);
	int replace();
	int replace(int _channels);
	int replace(int channels, int startFrame, int endFrame);
   
   int getNumChannelsInFile();
   int getNumFramesInFile();
   const char* getFileName();
   const char* filename;
   
   int startFrame;
   int endFrame;
   int numChannels;
   int numFrames;
   
   int rootKey = -1;
   
   SampleData* sampleData = NULL;
   
   // Functions for reading the buffers. 
   // These are protected from buffer reading whil loading.
   float read(int ch, int index);	// No Interpolation
   float read(int ch, float index);	// Linear Interpolation
   
   int busy=0;
   bool fileToReplace = false;
    
private:
	// private libsndfile wrappers
    int getSamples(const char* file, float *buf, int channel, int startFrame, int endFrame);
	SNDFILE *sndfile ;
	SF_INFO sfinfo ;
};

#endif // SAMPLEBUF_H_