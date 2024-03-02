#include <Keypad.h>

class KeypadManager {
private:
  
  char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  byte rowPins[4] = {40, 41, 42, 43}; //connect to the row pinouts of the keypad
  byte colPins[4] = {46, 47, 48, 49}; //connect to the column pinouts of the keypad
  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );

public:
  char getKey() {
    char key = keypad.getKey();
    return key;
    
  }

};