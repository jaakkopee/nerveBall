//a sound syntheziser and sequencer

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <SFML/Audio.hpp>


//a sound file player class
class SoundFilePlayer {
    public:
        std::string filename;
        sf::SoundBuffer buffer;
        sf::Sound sound;
        SoundFilePlayer();
        SoundFilePlayer(std::string filename);
        void load(std::string filename);
        void play();
        void stop();
        void setVolume(float volume);
};

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
        std::chrono::time_point<std::chrono::system_clock> startTime;
        Sequence();
        Sequence(std::vector<Note> notes);
        ~Sequence();
        void add(Note note);
        void remove(int index);
        void clear();
        std::vector<float> playSequenceOnce(unsigned int sampleRate);
        float getSample(unsigned int sampleRate);
        void updateSequence();
        void reset();
        float applyEnvelope(float sample, unsigned int sampleRate);
        int noteIndex;
        float getDuration();
};

//a sound output class
class SoundOutputSFML {
    public:
        sf::SoundBuffer buffer;
        sf::Sound sound;
        SoundOutputSFML();
        void play(std::vector<float> samples, unsigned int sampleRate);
        void stop();
        void setVolume(float volume);
        int sampleRate;
};

//a synth class

class Synth {
    public:
        Sequence sequence;
        SoundOutputSFML soundOutput;
        Synth();
        Synth(Sequence sequence, SoundOutputSFML soundOutput);
        ~Synth();
        void play();
        void stop();
        void setVolume(double volume);
        void setSequence(Sequence sequence);
        void setSoundOutput(SoundOutputSFML soundOutput);
        double volume;
        bool playing;
};


