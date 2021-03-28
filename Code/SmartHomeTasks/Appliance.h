
//////////////////////////////////Appliance PINS and Macros  /////////////////////////////////////////////////////////

#define ALARM_BUZZER PIND6      // ALARM Sound
#define ALARM_LED PIND7         // ALARM LIGHT
#define DOOR_LOCK  PINB1        // Door lock
#define AC_FAN PINB3            // Air Conditioner
#define HEATER PINB2            // Heater
#define DEVICE1 PINC0           // DEVICE1
#define DEVICE2 PINC1           // DEVICE2
#define LIGHT1 PINC2            // Light 1
#define LIGHT2 PINC3            // Light 2
#define LIGHT3 PINC4            // Light 3
#define LIGHT4 PINC5            // Light 4
#define LIGHT5 PINC6            // Light 5


#define PWM_FREQ_HZ 500         // PWM Freq by HZ config
#define ALARM_Delay 100         // Delay of the Buzzer
#define OUTPUT_Delay 500        // Delay of the OUTPUT

#define WARNING_UNSENT 0
#define WARNING_SENT 1
#define DOOR_CLOSED 0           //Door is close
#define DOOR_OPENED 1           //Door is Open

#define AC_FAN_OFF 0            //AC_FAN off
#define AC_FAN_ON 1             //AC_FAN on
#define HEATER_OFF 0            //heater off
#define HEATER_ON 1             //Heater on

#define DEVICE1_OFF 0           // DEVICE1 off
#define DEVICE1_ON 1           // DEVICE1 on
#define DEVICE2_OFF 0           // DEVICE2 off
#define DEVICE2_ON 1           // DEVICE2 on
