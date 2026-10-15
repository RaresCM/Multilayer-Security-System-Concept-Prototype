#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "mfrc522.h"

volatile uint8_t movementdetected = 0;
volatile uint8_t timerseconds = 0;
const uint8_t masteruid[4] = {0x0A, 0xF7, 0xF2, 0x06};
const char masterpin[5] = {'3','1','6','A','#'};
const char hexKeys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

void keyboardpinsinit(void) {
  DDRA = 0b00001111;//Pin setup for input and output
  PORTA = 0b11111111;//All set as output
}

char keyboardscan(void) {
  for(uint8_t col = 0; col < 4; col++) {
    PORTA = ~(1 << col) | 0b11110000;//Picks a column col and if a button is pressed it drives that pin to low creating a ground state

    _delay_us(10);

    uint8_t rowread = PINA & 0b11110000;//Rowread only looks at the row pins
    uint8_t r = 5;

    if(!(rowread & 0b00010000)) r = 0;//If a row in a column col goes low then that digit in the row in the low column is pressed as the input pullup resistor will drop to 0 and is reversed to a 1
    if(!(rowread & 0b00100000)) r = 1;
    if(!(rowread & 0b01000000)) r = 2;
    if(!(rowread & 0b10000000)) r = 3;

    PORTA = 0b11111111;//Resets the low col pin 
    if(r != 5) return hexKeys[r][col];//Returns a digit from the matrix for hex keys
  }

  PORTA = 0b11111111;//Resets the low col pin 
  return 0;
}

char getkey(void) { // Debounce for keypad switches and same key pressed down detection
  char currentkey = keyboardscan();

  if(currentkey == 0) {
    return 0;
  }

  _delay_ms(10);

  if(currentkey == keyboardscan()) {
    while(currentkey == keyboardscan()) {};
     _delay_ms(10);
    return currentkey;
  }
  return 0;
}

uint16_t adcread(void) {//Reads the analog input on pin A0
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC)) {}; 
  return ADC; 
}

ISR(INT4_vect) {//Detection using interrupt
  movementdetected = 1;
}

ISR(INT5_vect) {//Tilt detected
  movementdetected = 1;
}

void setup(void) {//Setup for all pins
  DDRE &= ~((1 << DDE4) | (1 << DDE5));//PIR D2 input | Tilt D3 Input
  DDRF &= ~(1 << DDF0);//A0 Mic input
  DDRE |= (1 << DDE3);//Buzzer output
  PORTE &= ~(1 << PE3);//Buzzer set low

  EICRB |= (1 << ISC41) | (1 << ISC40);//D2 Interrupt on rising edge
  EIMSK |= (1 << INT4);//D2 Interrupt Enable

  EICRB |= (1 << ISC51) | (1 << ISC50);//D3 rising edge detection
  EIMSK |= (1 << INT5);//D3 Interrupt enable

  ADMUX = (1 << REFS0);//ADC A0 initialisation for sound detector
  ADCSRB = 0x00;//ADC MUX5 registry made sure to be all 0
  ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADPS0);//ADC On and 125Mhz seleceted

  TCCR3A = 0;//Clears PWM mode and WGM in A
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);//Puts timer on CTC mode
  OCR3A = 125;//Sets freq of buzzer

  mfrc522_init();//RFID Initialise

  sei(); //Interrupts on
}

int main(void) {
  setup();
  keyboardpinsinit();
  uint8_t status;
  uint8_t buffer[16];
  uint8_t uid[5];
  uint8_t pinbuffer[5];
  uint8_t pinindex = 0;

  while(1) {
    char customkey = getkey();
    uint16_t adcvalue = adcread();

    if(movementdetected == 1 || (adcvalue < 524 || adcvalue > 529)) {//If a sensor detects something turn on alarm
      TCCR3A |= (1 << COM3A0);//Buzzer on
      movementdetected = 0;//Reset trigger
    }

    if (customkey >= '0' && customkey <= '9' || (customkey >= 'A' && customkey <= 'D') || customkey == '#') {//Places pressed digit into array to check against password
        if (pinindex < 5) {
          pinbuffer[pinindex] = customkey;
          pinindex++;
        }
    } else if(customkey == '*') {//Resets digits if wrong password is input or unknown amount of digits is input
        pinindex = 0;
    }

    if(pinindex == 5){//Compares the 5 digits against password
      uint8_t pinmatch = 1;

      for(uint8_t i = 0; i < 5; i++) {
        if(pinbuffer[i] != masterpin[i]) {
          pinmatch = 0;
          break;
        }
      }

      if(pinmatch == 1) {//If password is right turns off alarm
        TCCR3A &= ~(1 << COM3A0);
        PORTE &= ~(1 << PE3);
      }

      pinindex = 0;

    }

    status = mfrc522_request(PICC_REQIDL, buffer);

    if (status == MI_OK) {//RFID to turn off alarm
      uint8_t cardmatch = 1;
      status = mfrc522_get_card_id(uid);

      for(uint8_t i = 0; i < 4; i++) {//Compares UID scanned to master UID
        if(uid[i] != masteruid[i]) {
          cardmatch = 0;
          break;
        }
      }

      if(cardmatch == 1) {//If UID correct alarm off
        TCCR3A &= ~(1 << COM3A0);
        PORTE &= ~(1 << PE3);
      }
      _delay_ms(1000);
    }

  }

  return 0;

}