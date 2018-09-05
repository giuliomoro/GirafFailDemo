#include <Bela.h>
#include <SampleBuf.h>
#include <iostream>

#define NUM_BUFFERS 1
SampleBuf *bufs[NUM_BUFFERS];

bool setup(BelaContext *context, void *userData)
{	

	bufs[0] = new SampleBuf("NeuGuitar_K40_E1.aif",1);
    
	return true;
}

void render(BelaContext *context, void *userData){	

    for(unsigned int n = 0; n < context->audioFrames; n++){
		float out = 0.0f;

		// Write output		
    	audioWrite(context, n, 0, out);
        audioWrite(context, n, 1, out);
   	}
    
}

void cleanup(BelaContext *context, void *userData){	
	for(int i=0; i<NUM_BUFFERS; i++){
		delete bufs[i];
	}
	cout << "Cleanup Completed..." << endl;
}


