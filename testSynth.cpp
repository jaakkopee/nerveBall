#include "synth.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>



int main(int argc, char* argv[]) {
    
    // Create Note objects
    //first a slow melody
    Note note1("A2", 1, 1);
    Note note2("C2", 1, 1);
    Note note3("E2", 1, 1);
    Note note4("G2", 1, 1);
    Note note5("A2", 1, 1);
    Note note6("C2", 1, 1);
    std::vector<Note> notes = {note1, note2, note3, note4, note5, note6};

    //then a fast melody
    Note note7("A3", 0.5, 1);
    Note note8("C3", 0.5, 1);
    Note note9("E3", 0.5, 1);
    Note note10("G3", 0.5, 1);
    Note note11("A3", 0.5, 1);
    Note note12("C3", 0.5, 1);
    Note note13("E3", 0.5, 1);
    Note note14("G3", 0.5, 1);
    Note note15("A3", 0.5, 1);
    Note note16("C3", 0.5, 1);
    Note note17("E3", 0.5, 1);
    Note note18("G3", 0.5, 1);
    std::vector<Note> notes2 = {note7, note8, note9, note10, note11, note12, note13, note14, note15, note16, note17, note18};

    //then a even faster melody
    Note note19("A5", 0.25, 1);
    Note note20("C5", 0.25, 1);
    Note note21("E5", 0.25, 1);
    Note note22("G5", 0.25, 1);
    Note note23("A5", 0.25, 1);
    Note note24("C5", 0.25, 1);
    Note note25("E5", 0.25, 1);
    Note note26("G5", 0.25, 1);
    Note note27("A5", 0.25, 1);
    Note note28("C5", 0.25, 1);
    Note note29("E5", 0.25, 1);
    Note note30("G5", 0.25, 1);
    Note note31("A5", 0.25, 1);
    Note note32("C5", 0.25, 1);
    Note note33("E5", 0.25, 1);
    Note note34("G5", 0.25, 1);
    Note note35("A5", 0.25, 1);
    Note note36("C5", 0.25, 1);
    std::vector<Note> notes3 = {note19, note20, note21, note22, note23, note24, note25, note26, note27, note28, note29, note30, note31, note32, note33, note34, note35, note36};

    // Create Sequence objects
    //first a slow melody
    Sequence slowMelody = Sequence(notes);
    //then a fast melody
    Sequence fastMelody = Sequence(notes2);
    //then a even faster melody
    Sequence fasterMelody = Sequence(notes3);

    // Create a sound output
    SoundOutputSFML soundOutput = SoundOutputSFML();

    //Create Synth objects
    Synth synth1 = Synth(slowMelody, soundOutput);
    Synth synth2 = Synth(fastMelody, soundOutput);
    Synth synth3 = Synth(fasterMelody, soundOutput);

    std::cout << "Playing the sequences" << std::endl;

    // Play the sequences (oh look, they're in threads and playing at the same time!)
    synth1.play();
    synth2.play();
    synth3.play();
    int sleepFor = (int)1000*std::max(slowMelody.getDuration(), std::max(fastMelody.getDuration(), fasterMelody.getDuration()));
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
    std::cout << "Finished playing the sequences" << std::endl;

    return 0;
}

