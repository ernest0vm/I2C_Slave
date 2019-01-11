#include <Wire.h>

//#define __NCbtn__
#define __NObtn__
#define __AutoON__
//#define __AutoOFF__

#define SLAVE_ADDRESS 0x08

#define PUSH       	P2_2
#define SDTBY_LED  	P2_0
#define PWRON_LED  	P2_1
#define PIN_EN_RASP	P2_3

#define NONE		0x00
#define ON			0x53
#define OFF			0x54
#define SHUTDOWN	0x55

byte Mode;
int counter = 0;
boolean shutdowning = false;


void state(byte bMode){

	Mode = bMode;

	switch (Mode) {
	case ON:
		digitalWrite(PIN_EN_RASP, 1);
		digitalWrite(PWRON_LED, 1);
		digitalWrite(SDTBY_LED, 0);
		break;
	case OFF:
		digitalWrite(PIN_EN_RASP, 0);
		digitalWrite(PWRON_LED, 0);
		digitalWrite(SDTBY_LED, 1);
		break;
	case SHUTDOWN:
		digitalWrite(PWRON_LED, 1);
		digitalWrite(SDTBY_LED, 1);
		break;
	default:
		break;
	}
}

void setup() {

	pinMode(PUSH, INPUT);
	pinMode(SDTBY_LED, OUTPUT);
	pinMode(PWRON_LED, OUTPUT);
	pinMode(PIN_EN_RASP, OUTPUT);

	Wire.begin(SLAVE_ADDRESS);

	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);

//autoON function
#ifdef __AutoON__
	state(ON);
#else if __AutoOFF__
	state(OFF);
#endif

}

void loop() {

#ifdef __NCbtn__
	if (digitalRead(PUSH) == HIGH){
#else if __NObtn__
	if (digitalRead(PUSH) == LOW){
#endif

		  state(ON);
		  delay(2000);
		  counter++;

		  if(counter >= 4){		//10 seconds to controlled shutdown
			  shutdowning = true;
		  }
	  }else{
		  counter = 0;
	  }

	if(shutdowning == true){


		for(int a = 0;a<=20;a++){	//60 seconds for auto OFF
				state(SHUTDOWN);
				delay(2500);
#ifdef __NCbtn__
				if (digitalRead(PUSH) == HIGH){counter++;}
#else if __NObtn__
				if (digitalRead(PUSH) == LOW){counter++;}
#endif
				if(counter >= 8){		//20 seconds to forced shutdown
					state(OFF);
					delay(2000);
					counter=0;
					goto fc_sh;
				}
		}
fc_sh:
		state(OFF);
		shutdowning = false;
		counter=0;
		delay(2000);
	}

}

// callback for receiving data
void receiveData(int numBytes){
	Mode = Wire.read();

	if(Mode == SHUTDOWN){
		shutdowning = true;
	}
}

// callback for sending data
void sendData(){
	Wire.write(Mode);
	Mode = NONE;
}
