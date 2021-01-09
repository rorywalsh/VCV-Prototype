#include "ScriptEngine.hpp"
#include <csound.hpp>


struct CsoundEngine : ScriptEngine {
	Csound* csound = nullptr;
    int ksmps = 32;
    int csScale = 1;
    int compileError = 1;
    int perfError = 0;
    int numberOfInputsOutputs = NUM_ROWS;
    MYFLT** audioOutputChannels;
	MYFLT** audioInputChannels;
    std::string debugMessage = "";
    std::vector<std::string> knobs;
    std::vector<std::string> switches;
    std::vector<std::string> lights;
    std::vector<std::string> switchLights;

    int frames = 0;

	~CsoundEngine() 
    {
		if(csound)
        {
            csound->Reset();
		    delete csound; 
        }
        
        free(audioInputChannels);
		free(audioOutputChannels);

	}

	std::string getEngineName() override {
		return "Csound";
	}

    static void messageCallback(CSOUND* csound, int /*attr*/,  const char* fmt, va_list args)
    {
       if(fmt)
       {
            CsoundEngine* ud = (CsoundEngine *) csoundGetHostData(csound);
            char msg[MAX_BUFFER_SIZE];
            vsnprintf(msg, MAX_BUFFER_SIZE, fmt, args);
            //ud->debugMessage +=msg;
            //ud->display(ud->debugMessage);
            ud->display(msg);
       }
    }

	int run(const std::string& path, const std::string& script) override {
		//ProcessBlock* block = getProcessBlock();


        csound = new Csound(); 
        csound->SetOption((char*)"-n");
        csound->SetOption((char*)"-d");
        csound->SetHostData(this);
        csound->SetMessageCallback(messageCallback);
        
        csound->SetHostImplementedAudioIO(1, 0);

        //CSOUND_PARAMS* csoundParams = nullptr;
        //csoundParams = new CSOUND_PARAMS();
        //csoundParams->sample_rate_override = block->sampleRate;
        //csoundParams->displays = 0;		
        //csound->SetParams(csoundParams);
        compileError = csound->CompileCsdText(script.c_str());
        csound->Start();
        
        
        audioOutputChannels = new MYFLT*[6];
		audioInputChannels = new MYFLT*[6];
        
        for( int i = 0 ; i < numberOfInputsOutputs ; i++)
		{
            std::string inputChannel = std::string("in")+std::to_string(i+1);
            std::string outputChannel = std::string("out")+std::to_string(i+1);
            knobs.push_back(std::string("k")+std::to_string(i+1));
            switches.push_back(std::string("s")+std::to_string(i+1));
        
            lights.push_back(std::string("l")+std::to_string(i+1)+std::string("r"));
            lights.push_back(std::string("l")+std::to_string(i+1)+std::string("g"));
            lights.push_back(std::string("l")+std::to_string(i+1)+std::string("b"));

            switchLights.push_back(std::string("sl")+std::to_string(i+1)+std::string("r"));
            switchLights.push_back(std::string("sl")+std::to_string(i+1)+std::string("g"));
            switchLights.push_back(std::string("sl")+std::to_string(i+1)+std::string("b"));

			audioOutputChannels[i] = new MYFLT[ksmps];
			csoundGetChannelPtr(csound->GetCsound(), &audioOutputChannels[i], outputChannel.c_str(),
                            CSOUND_AUDIO_CHANNEL | CSOUND_OUTPUT_CHANNEL);

			audioInputChannels[i] = new MYFLT[ksmps];
			csoundGetChannelPtr(csound->GetCsound(), &audioInputChannels[i], inputChannel.c_str(),
                           CSOUND_AUDIO_CHANNEL | CSOUND_INPUT_CHANNEL);
		}      	

       
        if(compileError != 0)
            display("Csound could not compile");
        else
        {   		    
            ksmps = csound->GetKsmps();
            setBufferSize(ksmps);
            csScale = csound->Get0dBFS();
            setFrameDivider(1);
        }	
		return 0;
	}

    void updateLights(ProcessBlock* block)
    {
        for( int i = 0 ; i < (int)lights.size()/3 ; i++)
        {
            block->lights[i][0] = csound->GetControlChannel(lights[i*3].c_str());
            block->lights[i][1] = csound->GetControlChannel(lights[i*3+1].c_str());
            block->lights[i][2] = csound->GetControlChannel(lights[i*3+2].c_str());
            block->switchLights[i][0] = csound->GetControlChannel(switchLights[i*3].c_str());
            block->switchLights[i][1] = csound->GetControlChannel(switchLights[i*3+1].c_str());
            block->switchLights[i][2] = csound->GetControlChannel(switchLights[i*3+2].c_str());            
        }
    }

	int process() override {
		ProcessBlock* block = getProcessBlock();
        // return samples to prototype
        if(csound && compileError==0)
			perfError = csound->PerformKsmps();
        else
        {
            return 0;
        }
        

        if(perfError == 0)
        {
            for (int kIndex = 0; kIndex < ksmps; kIndex++) 
            {	
                for ( int i = 0 ; i < numberOfInputsOutputs ; i++)
                   audioInputChannels[i][kIndex] = block->inputs[i][kIndex];	
                for ( int i = 0 ; i < numberOfInputsOutputs ; i++)
                    block->outputs[i][kIndex] = (float)(audioOutputChannels[i][kIndex] / csScale );	
            }

            for( int i = 0 ; i < numberOfInputsOutputs ; i++){
                csound->SetControlChannel(knobs[i].c_str(), block->knobs[i]);
                csound->SetControlChannel(switches[i].c_str(), block->switches[i]);
            }

            updateLights(block);
        }
		return 0;
	}

};


__attribute__((constructor(1000)))
static void constructor() {
	addScriptEngine<CsoundEngine>("csd");
}
