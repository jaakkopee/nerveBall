#include "synth.h"
#include <iostream>
#include <mutex>


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
    oscillators = {};
}

Sequence::Sequence(std::vector<Note> notes) {
    this->notes = notes;
    this->oscIndex = 0;
    // Create 8 oscillators for multitimbral synthesis
    for (int i = 0; i < 8; i++) {
        Oscillator oscillator;
        oscillators.push_back(oscillator);
    }

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
    float sample = 0;
    for (int i = 0; i < oscillators.size(); i++) {
        if (i == oscIndex) {
            sample += oscillators[i].getSample(sampleRate);
        }
        else {
            oscillators[i].getSample(sampleRate);
        }
    }

    oscIndex++;
    if (oscIndex >= oscillators.size()) {
        oscIndex = 0;
    }
    return applyEnvelope(sample, sampleRate);
}

std::vector<float> Sequence::playSequenceOnce(unsigned int sampleRate) {
    // set frequency and amplitude of oscillators
    for (int i = 0; i < oscillators.size(); i++) {
        for (int j = 0; j < notes.size(); j++) {
            oscillators[i].setFrequency(notes[j].frequency);
            oscillators[i].setAmplitude(notes[j].volume);
        }
    }
    // Calculate the total duration of the sequence
    float totalDuration = 0;
    for (const auto& note : notes) {
        totalDuration += note.duration;
    }
    
    // Calculate the total number of samples in the sequence
    int totalSamples = totalDuration*sampleRate;
    
    // Create a buffer to store the samples
    std::vector<float> buffer(totalSamples);
    
    // Generate the samples
    for (int i = 0; i < totalSamples; i++) {
        buffer[i] = getSample(sampleRate);
        updateSequence();
    }
    
    return buffer;
}

float Sequence::applyEnvelope(float sample, unsigned int sampleRate) {
    // Calculate the current time in seconds
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
    
    // Calculate the total duration of the sequence
    float totalDuration = 0;
    for (const auto& note : notes) {
        totalDuration += note.duration;
    }
    
    // Calculate the current note index based on the elapsed time and the note durations
    noteIndex = 0;
    float noteStartTime = 0;
    for (int i = 0; i < notes.size(); i++) {
        if (elapsedTime >= noteStartTime && elapsedTime < noteStartTime + notes[i].duration) {
            noteIndex = i;
            break;
        }
        noteStartTime += notes[i].duration;
    }
    
    // Calculate the current note's start time and end time
    float noteStart = 0;
    for (int i = 0; i < noteIndex; i++) {
        noteStart += notes[i].duration;
    }
    float noteEnd = noteStart + notes[noteIndex].duration;
    
    // Calculate the current note's attack, decay, sustain, and release times
    float attackTime = notes[noteIndex].duration/10;
    float decayTime = notes[noteIndex].duration/10;
    float sustainTime = notes[noteIndex].duration/2;
    float releaseTime = notes[noteIndex].duration/10;
    
    // Calculate the current note's attack, decay, sustain, and release volumes
    float attackVolume = notes[noteIndex].volume;
    float decayVolume = notes[noteIndex].volume/2;
    float sustainVolume = notes[noteIndex].volume/2;
    float releaseVolume = 0;
    
    // Calculate the current note's attack, decay, sustain, and release start times
    float attackStart = noteStart;
    float decayStart = attackStart + attackTime;
    float sustainStart = decayStart + decayTime;
    float releaseStart = sustainStart + sustainTime;
    
    // Apply the envelope to the sample
    if (elapsedTime >= attackStart && elapsedTime < decayStart) {
        sample *= (attackVolume/attackTime)*(elapsedTime - attackStart);
    }
    else if (elapsedTime >= decayStart && elapsedTime < sustainStart) {
        sample *= (decayVolume/decayTime)*(elapsedTime - decayStart) + attackVolume;
    }
    else if (elapsedTime >= sustainStart && elapsedTime < releaseStart) {
        sample *= sustainVolume;
    }
    else if (elapsedTime >= releaseStart && elapsedTime < noteEnd) {
        sample *= (releaseVolume/releaseTime)*(elapsedTime - releaseStart) + sustainVolume;
    }
    else {
        sample = 0;
    }
    return sample;
}

void Sequence::updateSequence() {
    // Calculate the current time in seconds
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
    
    // Calculate the total duration of the sequence
    float totalDuration = 0;
    for (const auto& note : notes) {
        totalDuration += note.duration;
    }
    
    // If the sequence has finished playing, reset startTime and noteIndex
    if (elapsedTime >= totalDuration) {
        startTime = currentTime;
        noteIndex = 0;  // Reset noteIndex
    } else {
        // Update noteIndex based on elapsedTime
        float noteStartTime = 0;
        for (int i = 0; i < notes.size(); i++) {
            if (elapsedTime >= noteStartTime && elapsedTime < noteStartTime + notes[i].duration) {
                noteIndex = i;
                break;
            }
            noteStartTime += notes[i].duration;
        }
    }
}

void Sequence::reset() {
    startTime = std::chrono::high_resolution_clock::now();
}

//alsa sound output

SoundOutput::SoundOutput() {
    sampleRate = 44100;
    bufferSize = 1024;
    
    channels = 1;
    periodSize = 1024;
    periodCount = 2;
    latency = 500000;
}

void SoundOutput::open() {
    this->err = snd_pcm_open(&this->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (this->err < 0) {
        std::cout << "Open error: " << snd_strerror(this->err) << std::endl;
    }
    snd_pcm_hw_params_alloca(&this->params);
    snd_pcm_hw_params_any(this->handle, this->params);
    snd_pcm_hw_params_set_access(this->handle, this->params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(this->handle, this->params, SND_PCM_FORMAT_FLOAT);
    snd_pcm_hw_params_set_channels(this->handle, this->params, this->channels);
    snd_pcm_hw_params_set_rate_near(this->handle, this->params, &this->sampleRate, 0);
    snd_pcm_hw_params_set_period_size_near(this->handle, this->params, &this->periodSize, 0);
    snd_pcm_hw_params_set_periods_near(this->handle, this->params, &this->periodCount, 0);
    this->err = snd_pcm_hw_params(this->handle, this->params);
    if (this->err < 0) {
        std::cout << "Hardware setup error: " << snd_strerror(this->err) << std::endl;
    }
}

void SoundOutput::write(float* buffer, int bufferSize) {
    this->err = snd_pcm_writei(this->handle, buffer, bufferSize);
    if (this->err < 0) {
        std::cout << "Write error: " << snd_strerror(this->err) << std::endl;
    }
}

void SoundOutput::close() {
    snd_pcm_drain(this->handle);
    snd_pcm_close(this->handle);
}

//a synth class

Synth::Synth() {
    sequence = Sequence();
    soundOutput = SoundOutput();
    volume = 1;
}

Synth::Synth(Sequence sequence, SoundOutput soundOutput) {
    this->sequence = sequence;
    this->soundOutput = soundOutput;
    volume = 1;
}

Synth::~Synth() {
}

void Synth::setVolume(double volume) {
    this->volume = volume;
}

void Synth::play() {
    float buffer[soundOutput.bufferSize];
    this->sequence.startTime = std::chrono::high_resolution_clock::now();
    std::vector<float> samples = sequence.playSequenceOnce(soundOutput.sampleRate);
    //split samples into chunks of size bufferSize and write them to the sound card
    for (int i = 0; i < samples.size(); i += soundOutput.bufferSize) {
        for (int j = 0; j < soundOutput.bufferSize; j++) {
            buffer[j] = samples[i + j];
        }
        soundOutput.write(buffer, soundOutput.bufferSize);
    }
}

void Synth::stop() {
    isRunning = false;
}

void Synth::setSequence(Sequence sequence) {
    this->sequence = sequence;
}

void Synth::setSoundOutput(SoundOutput soundOutput) {
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

bool isRunning = true;

/*
int main() {
    isRunning = true;
    Sequence sequence({Note("G2", 0.03, 1), Note ("C4", 0.03, 1), Note ("E4", 0.03, 1)});
    Synth synth(sequence, SoundOutput());
    std::this_thread::sleep_for(std::chrono::seconds(10));
    synth.stop();
}
*/