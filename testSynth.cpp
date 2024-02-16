#include "synth.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <thread>



int main(int argc, char* argv[]) {
    
    // Create Note objects
    //first a slow melody
    Note note1("A4", 1, 1);
    Note note2("C4", 1, 1);
    Note note3("E4", 1, 1);
    Note note4("G4", 1, 1);
    Note note5("A4", 1, 1);
    Note note6("C4", 1, 1);
    std::vector<Note> notes = {note1, note2, note3, note4, note5, note6};

    //then a fast melody
    Note note7("A4", 0.5, 1);
    Note note8("C4", 0.5, 1);
    Note note9("E4", 0.5, 1);
    Note note10("G4", 0.5, 1);
    Note note11("A4", 0.5, 1);
    Note note12("C4", 0.5, 1);
    Note note13("E4", 0.5, 1);
    Note note14("G4", 0.5, 1);
    Note note15("A4", 0.5, 1);
    Note note16("C4", 0.5, 1);
    Note note17("E4", 0.5, 1);
    Note note18("G4", 0.5, 1);
    std::vector<Note> notes2 = {note7, note8, note9, note10, note11, note12, note13, note14, note15, note16, note17, note18};

    // Create Sequence objects
    //first a slow melody
    Sequence slowMelody = Sequence(notes);
    //then a fast melody
    Sequence fastMelody = Sequence(notes2);
    
    // Create a sound output
    SoundOutputSFML soundOutput = SoundOutputSFML();

    //Create Synth objects
    Synth synth1 = Synth(slowMelody, soundOutput);
    Synth synth2 = Synth(fastMelody, soundOutput);

    std::cout << "Playing the sequences" << std::endl;

    // Play the sequences
    synth1.play();
    synth2.play();
    int sleepFor = (int)std::max(slowMelody.getDuration(), fastMelody.getDuration());
    std::this_thread::sleep_for(std::chrono::seconds(sleepFor));

    return 0;
}

