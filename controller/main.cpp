#include "WProgram.h"
#include "Servo/Servo.h"
#include "EEPROM/EEPROM.h"

#define EEPROM_LOW  0
#define EEPROM_HIGH 4

extern "C" int main(void)
{

	// To use Teensy 3.0 without Arduino, simply put your code here.
	// For example:

	int status = 0;

	Serial.begin(9600);
	Servo myservo;

	//high is on
	pinMode(0, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(12, OUTPUT);
	//low is on
	pinMode(1, INPUT_PULLUP);
	pinMode(3, INPUT_PULLUP);
	pinMode(9, INPUT_PULLUP);
	pinMode(11, INPUT_PULLUP);


//	char lastpin[4] = {1};
	char newpin[4] = {1};
	char debounce[4] = {0};


	int flashtime = 0;


	int calib_low = 0;
	int calib_high = 0;
	//endianness lol
	calib_low |= EEPROM.read(EEPROM_LOW+0) << 0;
	calib_low |= EEPROM.read(EEPROM_LOW+1) << 1;
	calib_low |= EEPROM.read(EEPROM_LOW+2) << 2;
	calib_low |= EEPROM.read(EEPROM_LOW+3) << 3;

	calib_high |= EEPROM.read(EEPROM_HIGH+0) << 0;
	calib_high |= EEPROM.read(EEPROM_HIGH+1) << 1;
	calib_high |= EEPROM.read(EEPROM_HIGH+2) << 2;
	calib_high |= EEPROM.read(EEPROM_HIGH+3) << 3;
	int calib_delt = calib_high-calib_low;


	pinMode(13, OUTPUT);
	myservo.attach(23, 1000, 2000);
	int i;
	for(i = 0; 1; i++) {
		newpin[0] = digitalRead(1);
		newpin[1] = digitalRead(3);
		newpin[2] = digitalRead(9);
		newpin[3] = digitalRead(11);

		int input = analogRead(22);

		int val = ((input - calib_low) * 180) / calib_delt;
		if(val <0) val=0; else if(val > 180) val = 180;
		if(!(i%8192)){
			Serial.print("min is: ");
			Serial.println(calib_low);
			Serial.print("high is: ");
			Serial.println(calib_high);
			Serial.print("analog 22 is: ");
			Serial.println(input);
			Serial.print("pwm is: ");
			Serial.println(val);
		}
		digitalWriteFast(13, i&4096 ? HIGH : LOW);
		if(status == 1){
//			char estop = (!newpin[0] && lastpin[0]) || (!newpin[1] && lastpin[1]) || (!newpin[2] && lastpin[2]) || (!newpin[3] && lastpin[3]);
			char estop = (!newpin[0] && debounce[0]==255) || (!newpin[1] && debounce[1]==255) || (!newpin[2] && debounce[2]==255) || (!newpin[3] && debounce[3]==255);
			if(estop) val = status = 0;
			myservo.write(val);
	//		delay(250);
			digitalWriteFast(0, i&4096 ? HIGH : LOW);
			if(flashtime){
				flashtime--;
				digitalWriteFast(0, flashtime&8192 ? LOW : HIGH);
				digitalWriteFast(2, flashtime&8192 ? LOW : HIGH);
				digitalWriteFast(4, flashtime&8192 ? LOW : HIGH);
				digitalWriteFast(6, flashtime&8192 ? LOW : HIGH);
				digitalWriteFast(8, flashtime&8192 ? LOW : HIGH);
				digitalWriteFast(10,flashtime&8192 ? LOW : HIGH);
				digitalWriteFast(12,flashtime&8192 ? LOW : HIGH);
			} else {

				switch(val/20){
					default:
					case 6: digitalWriteFast(12, HIGH);
					case 5: digitalWriteFast(10, HIGH);
					case 4: digitalWriteFast(8, HIGH);
					case 3: digitalWriteFast(6, HIGH);
					case 2: digitalWriteFast(4, HIGH);
					case 1: digitalWriteFast(2, HIGH);
					case 0: digitalWriteFast(0, HIGH);
				}
				switch(val/20){
					case 0: digitalWriteFast(2, LOW);
					case 1: digitalWriteFast(4, LOW);
					case 2: digitalWriteFast(6, LOW);
					case 3: digitalWriteFast(8, LOW);
					case 4:	digitalWriteFast(10,LOW);
					case 5: digitalWriteFast(12, LOW);
				}
			}
		} else if(status == 2){
			if(flashtime){
				flashtime--;
				switch(flashtime/3000){
					default:
					case 6: digitalWriteFast(12, HIGH);
					case 5: digitalWriteFast(10, HIGH);
					case 4: digitalWriteFast(8, HIGH);
					case 3: digitalWriteFast(6, HIGH);
					case 2: digitalWriteFast(4, HIGH);
					case 1: digitalWriteFast(2, HIGH);
					case 0: digitalWriteFast(0, HIGH);
				}
				switch(flashtime/3000){
					case 0: digitalWriteFast(2, LOW);
					case 1: digitalWriteFast(4, LOW);
					case 2: digitalWriteFast(6, LOW);
					case 3: digitalWriteFast(8, LOW);
					case 4:	digitalWriteFast(10,LOW);
					case 5: digitalWriteFast(12, LOW);
				}
			} else status = 0;
		} else if(status == 3){
			if(flashtime){
				flashtime--;
				switch((24576-flashtime)/3000){
					default:
					case 6: digitalWriteFast(12, HIGH);
					case 5: digitalWriteFast(10, HIGH);
					case 4: digitalWriteFast(8, HIGH);
					case 3: digitalWriteFast(6, HIGH);
					case 2: digitalWriteFast(4, HIGH);
					case 1: digitalWriteFast(2, HIGH);
					case 0: digitalWriteFast(0, HIGH);
				}
				switch((24567-flashtime)/3000){
					case 0: digitalWriteFast(2, LOW);
					case 1: digitalWriteFast(4, LOW);
					case 2: digitalWriteFast(6, LOW);
					case 3: digitalWriteFast(8, LOW);
					case 4:	digitalWriteFast(10,LOW);
					case 5: digitalWriteFast(12, LOW);
				}
			} else status = 0;
		} else {
			if(val < 100 && !newpin[0] && debounce[0] == 255){
				status = 1;
				flashtime = 24576;
			}
			if(!newpin[1] && debounce[1] == 255){
				if((calib_low >> 0) & 0xff != (input >> 0) & 0xff)EEPROM.write(EEPROM_LOW + 0, (input >> 0) & 0xff);
				if((calib_low >> 1) & 0xff != (input >> 1) & 0xff)EEPROM.write(EEPROM_LOW + 1, (input >> 1) & 0xff);
				if((calib_low >> 2) & 0xff != (input >> 2) & 0xff)EEPROM.write(EEPROM_LOW + 2, (input >> 2) & 0xff);
				if((calib_low >> 3) & 0xff != (input >> 3) & 0xff)EEPROM.write(EEPROM_LOW + 3, (input >> 3) & 0xff);
				calib_low = input;
				calib_delt = calib_high - calib_low;
				status = 2;
				flashtime = 24576;
			}
			if(!newpin[2] && debounce[2] == 255){
				if((calib_high >> 0) & 0xff != (input >> 0) & 0xff)EEPROM.write(EEPROM_HIGH + 0, (input >> 0) & 0xff);
				if((calib_high >> 1) & 0xff != (input >> 1) & 0xff)EEPROM.write(EEPROM_HIGH + 1, (input >> 1) & 0xff);
				if((calib_high >> 2) & 0xff != (input >> 2) & 0xff)EEPROM.write(EEPROM_HIGH + 2, (input >> 2) & 0xff);
				if((calib_high >> 3) & 0xff != (input >> 3) & 0xff)EEPROM.write(EEPROM_HIGH + 3, (input >> 3) & 0xff);
				calib_high = input;
				calib_delt = calib_high - calib_low;
				status = 3;
				flashtime = 24576;
			}
			digitalWriteFast(0, i&65536 ? HIGH : LOW);
			digitalWriteFast(2, LOW);
			digitalWriteFast(4, LOW);
			digitalWriteFast(6, LOW);
			digitalWriteFast(8, LOW);
			digitalWriteFast(10, LOW);
			digitalWriteFast(12, LOW);
			myservo.write(0);
		}
		//counts how long the button has been in an off state
		debounce[0] = newpin[0] ? (debounce[0] < 255 ? debounce[0]+1 : 255) : 0;
		debounce[1] = newpin[1] ? (debounce[1] < 255 ? debounce[1]+1 : 255) : 0;
		debounce[2] = newpin[2] ? (debounce[2] < 255 ? debounce[2]+1 : 255) : 0;
		debounce[3] = newpin[3] ? (debounce[3] < 255 ? debounce[3]+1 : 255) : 0;

		//lastpin[0] = newpin[0];
//		lastpin[1] = newpin[1];
//		lastpin[2] = newpin[2];
//		lastpin[3] = newpin[3];
	}


}
