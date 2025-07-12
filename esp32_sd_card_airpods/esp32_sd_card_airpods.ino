#include "BluetoothA2DPSource.h"
#include "SD.h"

BluetoothA2DPSource a2dp_source;

#define SD_CS_PIN 5  // Replace with your SD module's CS pin
#define AUDIO_FILE "/audio.pcm"  // Ensure the audio file is in PCM format (16-bit, 44.1kHz, stereo)

File audioFile;

// Fetch PCM data from the SD card
int32_t get_data_frames(Frame *frame, int32_t frame_count) {
    if (!audioFile || audioFile.available() <= 0) {
        audioFile.seek(0);  // Restart playback if the end is reached
    }

    for (int i = 0; i < frame_count; ++i) {
        if (audioFile.available() >= 4) {  // Each sample requires 4 bytes (2 channels, 16-bit)
            frame[i].channel1 = audioFile.read() | (audioFile.read() << 8);  // Read 16-bit sample
            frame[i].channel2 = audioFile.read() | (audioFile.read() << 8);  // Read second channel
        } else {
            // Handle file underrun
            frame[i].channel1 = 0;
            frame[i].channel2 = 0;
        }
    }

    // Prevent watchdog timeout
    delay(1);

    return frame_count;
}

void setup() {
    Serial.begin(115200);

    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed!");
        while (true) {}  // Halt the program
    }
    
    audioFile = SD.open(AUDIO_FILE, FILE_READ);
    if (!audioFile) {
        Serial.println("Failed to open audio file!");
        while (true) {}  // Halt the program
    }

    a2dp_source.set_auto_reconnect(false);
    a2dp_source.start("Noise Buds N1", get_data_frames);  
    a2dp_source.set_volume(30);
}

void loop() {
    // Prevent watchdog timeout
    delay(1000);
}
