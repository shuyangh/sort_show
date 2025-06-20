#include "MidiPlayer.h"
#include <iostream>
#include <thread>
#include <chrono>

MidiPlayer::MidiPlayer() {
    initialize();
}

MidiPlayer::~MidiPlayer() {
    cleanup();
}

bool MidiPlayer::initialize() {
    MMRESULT result = midiOutOpen(&midi_out_handle, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
    if (result != MMSYSERR_NOERROR) {
        return false;
    }
    
    is_initialized = true;
    
    setInstrument(current_instrument);
    
    return true;
}

void MidiPlayer::cleanup() {
    if (is_initialized && midi_out_handle) {
        stopAllNotes();
        midiOutReset(midi_out_handle);
        midiOutClose(midi_out_handle);
        midi_out_handle = nullptr;
    }
    is_initialized = false;
}

void MidiPlayer::playNote(int value, int max_value, int duration_milliseconds) {
    if (!is_initialized) return;
    
    int note = mapValueToNote(value, max_value);
    int velocity = 80;
    
    sendMidiMessage(0x90 | current_channel, note, velocity);
    
    std::thread([this, note, duration_milliseconds]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_milliseconds));
        if (is_initialized) {
            sendMidiMessage(0x80 | current_channel, note, 0);
        }
    }).detach();
}

void MidiPlayer::stopAllNotes() {
    if (!is_initialized) return;
    
    for (int channel = 0; channel < 16; ++channel) {
        sendMidiMessage(0xB0 | channel, 123, 0);
        sendMidiMessage(0xB0 | channel, 120, 0);
    }
}

void MidiPlayer::setInstrument(int instrument) {
    if (!is_initialized) return;
    
    current_instrument = std::max(1, std::min(128, instrument));
    
    sendMidiMessage(0xC0 | current_channel, current_instrument - 1, 0);
}

void MidiPlayer::setChannel(int channel) {
    current_channel = std::max(0, std::min(15, channel));
}

void MidiPlayer::setNoteRange(int base_note_param, int range) {
    base_note = std::max(0, std::min(127, base_note_param));
    note_range = std::max(1, std::min(127 - base_note, range));
}

void MidiPlayer::sendMidiMessage(unsigned char status, unsigned char note, unsigned char velocity) {
    if (!is_initialized || !midi_out_handle) return;
    
    DWORD message = status | (note << 8) | (velocity << 16);
    midiOutShortMsg(midi_out_handle, message);
}

int MidiPlayer::mapValueToNote(int value, int max_value) const {
    if (max_value <= 0) return base_note;
    
    float ratio = static_cast<float>(value) / static_cast<float>(max_value);
    int note_offset = static_cast<int>(ratio * note_range);
    
    return base_note + note_offset;
}
