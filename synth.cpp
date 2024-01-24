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
    modulatorFrequency = carrierFrequency*2;
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
    modulatorFrequency = carrierFrequency*2;
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
    Oscillator oscillator;
    oscillators.push_back(oscillator);
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
    
    // Calculate the current time in seconds
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
    
    // Calculate the current note index based on the elapsed time and the note durations
    int noteIndex = 0;
    float noteStartTime = 0;
    for (int i = 0; i < notes.size(); i++) {
        if (elapsedTime >= noteStartTime && elapsedTime < noteStartTime + notes[i].duration) {
            noteIndex = i;
            break;
        }
        noteStartTime += notes[i].duration;
    }
    
    // Update the oscillator's frequency and amplitude
    oscillators[0].setFrequency(notes[noteIndex].frequency);
    oscillators[0].setAmplitude(notes[noteIndex].volume);
    
    // Generate the sample
    for (int i = 0; i < oscillators.size(); i++) {
        sample += oscillators[i].getSample(sampleRate);
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
    }
}

//alsa sound output

SoundOutput::SoundOutput() {
    sampleRate = 44100;
    bufferSize = 1024;
    
    channels = 1;
    periodSize = 1024;
    periodCount = 2;
    latency = 500000;

    open();
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
}

Synth::Synth(Sequence sequence, SoundOutput soundOutput) {
    this->sequence = sequence;
    this->soundOutput = soundOutput;
    std::thread thread = std::thread(&Synth::play, this);
    thread.detach();
}

Synth::~Synth() {
}

void Synth::play() {
    float buffer[soundOutput.bufferSize];
    this->sequence.startTime = std::chrono::high_resolution_clock::now();
    while (isRunning) {
        for (int i = 0; i < soundOutput.bufferSize; i++) {
            buffer[i] = sequence.getSample(soundOutput.sampleRate);
            sequence.updateSequence();
        }
        soundOutput.write(buffer, soundOutput.bufferSize);
    }
    soundOutput.close();
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


int main() {
    isRunning = true;
    Sequence sequence({Note("G2", 0.3, 1), Note ("C4", 0.3, 1), Note ("E4", 0.3, 1)});
    Synth synth(sequence, SoundOutput());
    std::this_thread::sleep_for(std::chrono::seconds(10));
    synth.stop();
}