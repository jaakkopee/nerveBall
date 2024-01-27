//a sound syntheziser and sequencer

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <alsa/asoundlib.h>
#include <thread>

//note to frequency conversion map
extern std::unordered_map<std::string, double> noteToFreq;

//a note class

class Note {
    public:
        std::string note;
        double duration;
        double volume;
        double frequency;
        Note();
        Note(std::string note, double duration, double volume);
        void setFrequency();
};

// an oscillator class

class Oscillator {
    public:
        double carrierFrequency;
        double carrierPhase;
        double carrierAmplitude;
        double modulatorFrequency;
        double modulatorPhase;
        double modulatorAmplitude;

        Oscillator();
        Oscillator(double carrierFrequency, double carrierPhase, double carrierAmplitude, double modulatorFrequency, double modulatorPhase, double modulatorAmplitude);
        void setFrequency(double frequency);
        void setPhase(double phase);
        void setAmplitude(double amplitude);
        float getSample(unsigned int sampleRate);      
};

//a sequence class

class Sequence {
    public:
        std::vector<Note> notes;
        std::vector<Oscillator> oscillators;
        int oscIndex;
        std::chrono::_V2::system_clock::time_point startTime;
        Sequence();
        Sequence(std::vector<Note> notes);
        void add(Note note);
        void remove(int index);
        void clear();
        std::vector<float> playSequenceOnce(unsigned int sampleRate);
        float getSample(unsigned int sampleRate);
        void updateSequence();
        void reset();
        float applyEnvelope(float sample, unsigned int sampleRate);
        int noteIndex;
};

//alsa sound output

class SoundOutput {
    public:
        unsigned int sampleRate;
        snd_pcm_uframes_t bufferSize;
        int channels;
        snd_pcm_uframes_t periodSize;
        unsigned int periodCount;
        int latency;
        int err;
        snd_pcm_t* handle;
        snd_pcm_hw_params_t* params;
        snd_pcm_uframes_t frames;
        SoundOutput();
        SoundOutput(int sampleRate, int bufferSize, int channels);
        void open();
        void close();
        void write(float* buffer, int bufferSize);
};

//a synth class

class Synth {
    public:
        Sequence sequence;
        SoundOutput soundOutput;
        Synth();
        Synth(Sequence sequence, SoundOutput soundOutput);
        ~Synth();
        void play();
        void stop();
        void setVolume(double volume);
        void setSequence(Sequence sequence);
        void setSoundOutput(SoundOutput soundOutput);
        double volume;
};

extern bool isRunning;

