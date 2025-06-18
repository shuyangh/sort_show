#ifndef MIDI_PLAYER_H
#define MIDI_PLAYER_H

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

#include <vector>
#include <memory>

class MidiPlayer {
private:
    HMIDIOUT midi_out_handle = nullptr;
    bool is_initialized = false;
    int current_channel = 0;
    int current_instrument = 1;
    int base_note = 60;
    int note_range = 48;
    
public:
    MidiPlayer();
    ~MidiPlayer();
    
    bool initialize();
    void cleanup();
    
    void playNote(int value, int max_value, int duration_milliseconds = 200);
    void stopAllNotes();
    
    void setInstrument(int instrument);
    void setChannel(int channel);
    void setNoteRange(int base_note, int range);
    
    bool isInitialized() const { return is_initialized; }
    
private:
    void sendMidiMessage(unsigned char status, unsigned char data1, unsigned char data2 = 0);
    int mapValueToNote(int value, int max_value) const;
};

#endif
