/***** SampleBuf.cpp *****/
#include <SampleBuf.h>

SampleBuf::SampleBuf(const char* filename) {
	sampleData = NULL;
	openFile(filename);
}

SampleBuf::SampleBuf(const char* filename, int channels) {
	sampleData = NULL;
	openFile(filename, channels);
}

SampleBuf::SampleBuf(const char* filename, int channels, int startFrame, int endFrame){
	sampleData = NULL;
	openFile(filename, channels, startFrame, endFrame);
}

SampleBuf::~SampleBuf(){
    sf_close(sndfile);
    if(sampleData != NULL){
    for(int ch=0;ch<numChannels;ch++){
        if(sampleData[ch].samples != NULL)
            delete[] sampleData[ch].samples;
        }
    free(sampleData);
    }
    // cout << "Buffer Object Destroyed !!" << endl;
}

int SampleBuf::openFile(const char* _filename){
	openSF(_filename);
	return replace(sfinfo.channels, 0, sfinfo.frames);
}

int SampleBuf::openFile(const char* _filename, int _channels){
	openSF(_filename);
	return replace(_channels, 0, sfinfo.frames);
}

int SampleBuf::openFile(const char* _filename, int _channels, int _startFrame, int _endFrame){
	openSF(_filename);
	return replace(_channels, _startFrame, _endFrame);
}

int SampleBuf::openSF(const char* _filename) {
	busy = 1;
    sf_close(sndfile);
    sfinfo.format = 0; 
    if (!(sndfile = sf_open (string(_filename).c_str(), SFM_READ, &sfinfo))) {
		cout << "Couldn't open file " << filename << ": " << sf_strerror(sndfile) << endl;
		fileToReplace = false;
		busy = 0;
		return 1;
	}
	filename = _filename;
	fileToReplace = true;
	busy = 0;
	return 0;
}

int SampleBuf::replace(){
	return SampleBuf::replace(sfinfo.channels, 0, sfinfo.frames);
}

int SampleBuf::replace(int _channels){
	return SampleBuf::replace(_channels, 0, sfinfo.frames);
}

int SampleBuf::replace(int _channels, int _startFrame, int _endFrame){
	busy = 1;
	
	// If the a buffer is already in place, cleanup !
    if(sampleData != NULL) {
        // cout << "I AM NOT A NULL" << endl;
        for(int ch=0; ch<numChannels; ch++) {
            if(sampleData[ch].samples != NULL)
                delete[] sampleData[ch].samples;
        }
        free(sampleData);
        sampleData = NULL;
    }
	
	// Get new info from file
	numChannels = min(_channels, sfinfo.channels);
	startFrame = max(0,_startFrame);
	endFrame = min(_endFrame, sfinfo.frames);
	numFrames = endFrame-startFrame;
	sampleData = new SampleData[numChannels];
	
	for(int ch=0;ch<numChannels;ch++) {
        sampleData[ch].sampleLen = numFrames;
    	sampleData[ch].samples = new float[numFrames];
        if(getSamples(filename,sampleData[ch].samples,ch,0,numFrames)){
            printf("error getting samples\n");
            return 1;
        }
    }
	
    cout << "SampleBuf Loaded: " << filename << endl;
    fileToReplace = false;
    busy = 0;
    return 0;
}


// private libsndfile wrappers (previously in SampleLoader.h) from the examples
int SampleBuf::getSamples(const char* file, float *buf, int channel, int startFrame, int endFrame)
{
	int numChannelsInFile = sfinfo.channels;
	if(numChannelsInFile < channel+1){
		cout << "Error: " << file << " doesn't contain requested channel" << endl;
		return 1;
	}
    
    int frameLen = endFrame-startFrame;
    
    if(frameLen <= 0 || startFrame < 0 || endFrame <= 0 || endFrame > sfinfo.frames){
	    cout << "Error: " << file << " invalid frame range requested" << endl;
		return 1;
	}
    
    sf_seek(sndfile,startFrame,SEEK_SET);
    
    float* tempBuf = new float[frameLen*numChannelsInFile];
    
	int subformat = sfinfo.format & SF_FORMAT_SUBMASK;
	int readcount = sf_read_float(sndfile, tempBuf, frameLen*numChannelsInFile); //FIXME

	// Pad with zeros in case we couldn't read whole file
	for(int k = readcount; k <frameLen*numChannelsInFile; k++)
		tempBuf[k] = 0;

	if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE) {
		double	scale ;
		int 	m ;

		sf_command (sndfile, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
		if (scale < 1e-10)
			scale = 1.0 ;
		else
			scale = 32700.0 / scale ;
		cout << "File samples scale = " << scale << endl;

		for (m = 0; m < frameLen; m++)
			tempBuf[m] *= scale;
	}
	
	for(int n=0;n<frameLen;n++)
	    buf[n] = tempBuf[n*numChannelsInFile+channel];

    delete[] tempBuf;

	return 0;
}

int SampleBuf::getNumChannelsInFile() {
	return sfinfo.channels;
}

int SampleBuf::getNumFramesInFile() {
	return sfinfo.frames;
}

const char * SampleBuf::getFileName() {
	return filename;
}

// Methods for reading and writing data from / to buffer.
float SampleBuf::read(int ch, int index){
	if(busy || index < 0 || index >= numFrames) return 0.0f;
	ch = ch % numChannels;
	return sampleData[ch].samples[index];
}

// Read buffer using linear interpolation:
float SampleBuf::read(int ch, float pos){
	if(busy || pos < 0.0f || pos >= numFrames) return 0.0f;
	float fractpart, intpart, xL, xH;
	int iL, iH;
	ch = ch % numChannels;
  	fractpart = modf(pos , &intpart);
	iL = (int) intpart;
	iH = iL+1;
	if(iL<0 || iL >= numFrames) return 0.0f;
	else if(iH >= numFrames) return sampleData[ch].samples[iL];
	else {
		xL = sampleData[ch].samples[iL];
		xH = sampleData[ch].samples[iH];
		return xL*(1.0f-fractpart) + xH*fractpart;
	}
}

