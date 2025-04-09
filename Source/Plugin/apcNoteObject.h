#include <vector>
class apcNoteObject {
public:               // Integer for the note
    bool noteOn;                   // Boolean for note on/off status
    float length;                  // Float for note length
    float velocity;                // Float for velocity
    std::vector<int> noteData;     // Resizable array of integers

    // Constructor
    apcNoteObject( bool on = false, float len = 0.0f, float vel = 0.0f)
        :  noteOn(on), length(len), velocity(vel) {
        // noteData starts empty
    }
};