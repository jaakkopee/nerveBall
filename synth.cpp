#include "synth.h"
#include <iostream>
#include <mutex>

//a sound file player class

SoundFilePlayer::SoundFilePlayer() {
    filename = "";
    buffer = sf::SoundBuffer();
    sound = sf::Sound();
}

SoundFilePlayer::SoundFilePlayer(std::string filename) {
    this->filename = filename;
    buffer = sf::SoundBuffer();
    sound = sf::Sound();
    load(filename);
}

void SoundFilePlayer::load(std::string filename) {
    if (!buffer.loadFromFile(filename)) {
        std::cout << "Error loading sound file" << std::endl;
    }
    sound.setBuffer(buffer);
}

void SoundFilePlayer::play() {
    sound.play();
}

void SoundFilePlayer::stop() {
    sound.stop();
}

void SoundFilePlayer::setVolume(float volume) {
    sound.setVolume(volume);
}

//a note class

Note::Note() {
    note = "A4";
    duration = 1;
    volume = 1;
    frequency = noteToFreq[note];
}

Note::Note(std::string note, double duration, double volume) {
    this->note = note;
    this->duration = duration;
    this->volume = volume;
    this->frequency = noteToFreq[note];
    std::cout << this->frequency << " " << this->note << std::endl;
}
// an oscillator class

Oscillator::Oscillator() {
    carrierFrequency = 440;
    carrierPhase = 0;
    carrierAmplitude = 1;
    modulatorFrequency = carrierFrequency*1.333;
    modulatorPhase = 0;
    modulatorAmplitude = carrierAmplitude/2;
}

Oscillator::Oscillator(double carrierFrequency, double carrierPhase, double carrierAmplitude, double modulatorFrequency, double modulatorPhase, double modulatorAmplitude) {
    this->carrierFrequency = carrierFrequency;
    this->carrierPhase = carrierPhase;
    this->carrierAmplitude = carrierAmplitude;
    this->modulatorFrequency = modulatorFrequency;
    this->modulatorPhase = modulatorPhase;
    this->modulatorAmplitude = modulatorAmplitude;
}

void Oscillator::setFrequency(double frequency) {
    carrierFrequency = frequency;
    modulatorFrequency = carrierFrequency*1.333;
    //std::cout << carrierFrequency << " " << modulatorFrequency << std::endl;
}

void Oscillator::setPhase(double phase) {
    carrierPhase = phase;
    modulatorPhase = phase;
}

void Oscillator::setAmplitude(double amplitude) {
    carrierAmplitude = amplitude;
    modulatorAmplitude = amplitude/2;
}

float Oscillator::getSample(unsigned int sampleRate) {
    float sample = 0;
    carrierPhase += carrierFrequency/sampleRate;
    modulatorPhase += modulatorFrequency/sampleRate;
    if (carrierPhase >= 1) {
        carrierPhase -= 1;
    }
    if (modulatorPhase >= 1) {
        modulatorPhase -= 1;
    }
    sample = carrierAmplitude * sin(carrierPhase * 2 * M_PI + modulatorAmplitude * sin(modulatorPhase * 2 * M_PI));
    return sample;
}

//a sequence class

Sequence::Sequence() {
    notes = {};
    for (int i = 0; i < 8; i++) {
        Oscillator oscillator;
        oscillators.push_back(oscillator);
    }
}

Sequence::Sequence(std::vector<Note> notes) {
    this->notes = notes;
    this->oscIndex = 0;
    this->noteIndex = 0;
    // Create 8 oscillators for multitimbral synthesis
    for (int i = 0; i < 8; i++) {
        Oscillator oscillator;
        oscillators.push_back(oscillator);
    }

}

Sequence::~Sequence() {
}

void Sequence::add(Note note) {
    notes.push_back(note);
}

void Sequence::remove(int index) {
    notes.erase(notes.begin() + index);
}

void Sequence::clear() {
    notes.clear();
}

float Sequence::getSample(unsigned int sampleRate){
    //using multiple oscillators is redundant here, fix it.
    // Only generate a sample from the current oscillator
    float sample = oscillators[oscIndex].getSample(sampleRate);
    //run getSample on all oscillators except the current one
    for (int i = 0; i < oscillators.size(); i++) {
        if (i != oscIndex) {
            oscillators[i].getSample(sampleRate);
        }
    }

    // Increment oscIndex and wrap it around if it exceeds the size of oscillators
    oscIndex = (oscIndex + 1) % oscillators.size();

    return sample;
}


std::vector<float> Sequence::playSequenceOnce(unsigned int sampleRate) {
    std::vector<float> buffer;
    float totalDuration = 0;
    std::cout << notes.size() << std::endl;
    for (int i = 0; i < notes.size(); i++) {
        Note note = notes[i];
        float duration = note.duration;
        float frequency = note.frequency;
        float volume = note.volume;
        float sample = 0;
        float sampleDuration = duration*sampleRate;
        for (int j = 0; j < sampleDuration; j++) {
            oscillators[oscIndex].setFrequency(frequency);
            oscillators[oscIndex].setAmplitude(volume);
            sample = getSample(sampleRate);
            buffer.push_back(sample);
        }
        totalDuration += duration;
        std::cout << totalDuration << std::endl;
    }
    std::cout << totalDuration << std::endl;    
    return buffer;
}

void Sequence::reset() {
    startTime = std::chrono::system_clock::now();
}

float Sequence::getDuration() {
    float duration = 0;
    for (int i = 0; i < notes.size(); i++) {
        duration += notes[i].duration;
    }
    return duration;
}

//a sound output class implemented with sfml

SoundOutputSFML::SoundOutputSFML() {
    sound = sf::Sound();
    sound.setLoop(false);
    buffer = sf::SoundBuffer();
    sampleRate = 44100;
}

void SoundOutputSFML::play(std::vector<float> samples, unsigned int sampleRate) {
    std::cout << "Playing sound (soundOutput)" << std::endl;
    //convert samples to sf::Int16
    std::vector<sf::Int16> sfSamples;
    for (int i = 0; i < samples.size(); i++) {
        sfSamples.push_back(samples[i]*32767);
    }
    buffer.loadFromSamples(&sfSamples[0], samples.size(), 1, sampleRate);
    sound.setBuffer(buffer);
    sound.play();
}

//a synth class

Synth::Synth() {
    sequence = Sequence();
    soundOutput = SoundOutputSFML();
    volume = 1;
    this->playing = true;
}

Synth::Synth(Sequence sequence, SoundOutputSFML soundOutput) {
    this->sequence = sequence;
    this->soundOutput = soundOutput;
    volume = 1;
    this->playing = true;
}

Synth::~Synth() {
}

void Synth::setVolume(double volume) {
    this->volume = volume;
}

void Synth::play() {
    sequence.reset();
    std::vector<float> samples = sequence.playSequenceOnce(soundOutput.sampleRate);
    soundOutput.play(samples, soundOutput.sampleRate);
}

void Synth::stop() {
    this->playing = false;
    this->sequence.reset();
}

void Synth::setSequence(Sequence sequence) {
    this->sequence = sequence;
}

void Synth::setSoundOutput(SoundOutputSFML soundOutput) {
    this->soundOutput = soundOutput;
}

std::unordered_map<std::string, double> noteToFreq ={
    {"C0", 16.35},
    {"C#0", 17.32},
    {"D0", 18.35},
    {"D#0", 19.45},
    {"E0", 20.60},
    {"F0", 21.83},
    {"F#0", 23.12},
    {"G0", 24.50},
    {"G#0", 25.96},
    {"A0", 27.50},
    {"A#0", 29.14},
    {"B0", 30.87},
    {"C1", 32.70},
    {"C#1", 34.65},
    {"D1", 36.71},
    {"D#1", 38.89},
    {"E1", 41.20},
    {"F1", 43.65},
    {"F#1", 46.25},
    {"G1", 49.00},
    {"G#1", 51.91},
    {"A1", 55.00},
    {"A#1", 58.27},
    {"B1", 61.74},
    {"C2", 65.41},
    {"C#2", 69.30},
    {"D2", 73.42},
    {"D#2", 77.78},
    {"E2", 82.41},
    {"F2", 87.31},
    {"F#2", 92.50},
    {"G2", 98.00},
    {"G#2", 103.83},
    {"A2", 110.00},
    {"A#2", 116.54},
    {"B2", 123.47},
    {"C3", 130.81},
    {"C#3", 138.59},
    {"D3", 146.83},
    {"D#3", 155.56},
    {"E3", 164.81},
    {"F3", 174.61},
    {"F#3", 185.00},
    {"G3", 196.00},
    {"G#3", 207.65},
    {"A3", 220.00},
    {"A#3,", 233.08},
    {"B3", 246.94},
    {"C4", 261.63},
    {"C#4", 277.18},
    {"D4", 293.66},
    {"D#4", 311.13},
    {"E4", 329.63},
    {"F4", 349.23},
    {"F#4", 369.99},
    {"G4", 392.00},
    {"G#4", 415.30},
    {"A4", 440.00},
    {"A#4", 466.16},
    {"B4", 493.88},
    {"C5", 523.25},
    {"C#5", 554.37},
    {"D5", 587.33},
    {"D#5", 622.25},
    {"E5", 659.25},
    {"F5", 698.46},
    {"F#5", 739.99},
    {"G5", 783.99},
    {"G#5", 830.61},
    {"A5", 880.00},
    {"A#5", 932.33},
    {"B5", 987.77},
    {"C6", 1046.50},
    {"C#6", 1108.73},
    {"D6", 1174.66},
    {"D#6", 1244.51},
    {"E6", 1318.51},
    {"F6", 1396.91},
    {"F#6", 1479.98},
    {"G6", 1567.98},
    {"G#6", 1661.22},
    {"A6", 1760.00},
    {"A#6", 1864.66},
    {"B6", 1975.53},
    {"C7", 2093.00},
    {"C#7", 2217.46},
    {"D7", 2349.32},
    {"D#7", 2489.02},
    {"E7", 2637.02},
    {"F7", 2793.83},
    {"F#7", 2959.96},
    {"G7", 3135.96},
    {"G#7", 3322.44},
    {"A7", 3520.00},
    {"A#7", 3729.31},
    {"B7", 3951.07},
    {"C8", 4186.01},
    {"C#8", 4434.92},
    {"D8", 4698.63},
    {"D#8", 4978.03},
    {"E8", 5274.04},
    {"F8", 5587.65},
    {"F#8", 5919.91},
    {"G8", 6271.93},
    {"G#8", 6644.88},
    {"A8", 7040.00},
    {"A#8", 7458.62},
    {"B8", 7902.13}
};


