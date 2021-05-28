// include TFT and SPI libraries
#include <TFT.h>  
#include <SPI.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// sensor pins

// sensor 0
#define ECHO0 2
#define TRIG0 3

// sensor 1
#define ECHO1 5
#define TRIG1 4

// sensor 2
#define ECHO2 6
#define TRIG2 7

// sensor 3
#define ECHO3 A1
#define TRIG3 A0

// sensor 4
#define ECHO4 A2
#define TRIG4 A3

// pin definition for Arduino UNO
#define cs   10
#define dc   9
#define rst  8

// colors
#define COLOR_FULL 255
#define COLOR_EMPTY 0

// numbers - constants
#define NO_OF_SENSORS 5
#define NO_OF_MEASUREMENTS 5

// max distance = 40 cm??
#define INFINITY 40

// some magic defines for median of 5 elements
#define swap(a,b) (a) ^= (b); (b) ^= (a); (a) ^= (b);
#define sort(a,b) if((a)>(b)){ swap((a),(b)); }

#define MIN_2(a, b) (a) < (b) ? (a) : (b)

#define MIN_3(a, b, c) (a) < (b) ? \
                        ((a) < (c) ? (a) : (c)) : \
                        ((b) < (c) ? (b) : (c))


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// echo pins
int echo_pins[] = {ECHO0, ECHO1, ECHO2, ECHO3, ECHO4};

// trig pins
int trig_pins[] = {TRIG0, TRIG1, TRIG2, TRIG3, TRIG4};

int measurements[NO_OF_SENSORS][NO_OF_MEASUREMENTS];
int results[NO_OF_SENSORS];
int last_results[NO_OF_SENSORS];

int history_x[NO_OF_MEASUREMENTS];
int history_y[NO_OF_MEASUREMENTS];

int crt_history = 0;

int screen_width;
int screen_height;

int crt_x;
int crt_y;

int last_x;
int last_y;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ SET UP FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void init_sensors_pins()
{
        for (int i = 0; i < NO_OF_SENSORS; i++) {
                pinMode(echo_pins[i], INPUT);
                pinMode(trig_pins[i], OUTPUT);
        }

}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ MEASUREMENT FUCNTIONS ~~~~~~~~~~~~~~~~~~~~~~~~ */

void clear_measurements()
{
        for (int i = 0; i < NO_OF_SENSORS; i++) {
                for (int j = 0; j < NO_OF_MEASUREMENTS; j++) {
                        measurements[i][j] = INFINITY;
                }
                results[i] = INFINITY;
                last_results[i] = INFINITY;
        }
}

void median(int arr[])
{
        sort(arr[0], arr[1]);
        sort(arr[1], arr[4]);  
        sort(arr[0], arr[2]);
        sort(arr[1], arr[2]);
        sort(arr[0], arr[3]);  
        sort(arr[2], arr[3]);
        sort(arr[1], arr[4]);
        sort(arr[1], arr[2]);
}

// reads all the sensors five times and gets the median for each sensors
// output is stored in `int results[]`
void read_sensors()
{
        // for each of (5 times) cycle through each sensor
        for (int i = 0; i < NO_OF_MEASUREMENTS; i++) {
        
                // read each sensor back-to-back
                for (int j = 0; j < NO_OF_SENSORS; j++) {
                        // sensors are indexed from 0 to NO_OF_SENSORS - 1
                        measurements[j][i] = sensor_distance(j);
                }
        }
        
        char buff[100];

        // reoreder measurements
        for (int i = 0; i < NO_OF_SENSORS; i++) {
                median(measurements[i]);
                // store results
                results[i] = measurements[i][2];
        }
}

void transfers_results_to_old()
{
        for (int i = 0; i < NO_OF_SENSORS; i++) {
                last_results[i] = results[i];
        }
}

// sensor number i has to have trig pin = 2*i and echo pin 2*i+1
unsigned int sensor_distance(unsigned int sensor_number)
{
        unsigned int trig = trig_pins[sensor_number];
        unsigned int echo = echo_pins[sensor_number];
        
        unsigned int distance = 0;
        
        digitalWrite(trig, LOW);
        delayMicroseconds(2);
        digitalWrite(trig, LOW);
        delayMicroseconds(2);
        digitalWrite(trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig, LOW);
        
        unsigned long duration = pulseIn(echo, HIGH);
        // TODO: fine tune me to get distance in pixels, not in quarters
        // of centimeters
        distance = 2 * duration * 0.034; // instead of cm will be 1/4 of cm
        return distance;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ DARWING FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
void draw_test_lines(unsigned int distance1, unsigned int distance2)
{
        if (last_distance1 == distance1) {
//               return;
        } else {
                TFTscreen.stroke(0, 0, 0);
                TFTscreen.line(last_distance1, 0, last_distance1, screen_height);
                last_distance1 = distance1;
                
                TFTscreen.stroke(0, 0, 255);
                TFTscreen.line(distance1, 0, distance1, screen_height);
        }
        if (last_distance2 == distance2) {
        //       return;
        } else {
                TFTscreen.stroke(0, 0, 0);
                TFTscreen.line(last_distance2, 0, last_distance2, screen_height);
                last_distance2 = distance2;
                
                TFTscreen.stroke(0, 255, 0);
                TFTscreen.line(distance2, 0, distance2, screen_height);
        }
}

*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUG FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void print_measurements()
{

        char buff[100];
        sprintf(buff, "s[%d] = [%d] \t"
                "s[%d] = [%d] \t"
                "s[%d] = [%d] \t"
                "s[%d] = [%d] \t"
                "s[%d] = [%d] \t",
                0, results[0],
                1, results[1],
                2, results[2],
                3, results[3],
                4, results[4]);
        Serial.println(buff);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void setup() {

        // pin mode and other stuff
        init_sensors_pins();
        
        // measurements
        clear_measurements();
        
        //initialize the library
        TFTscreen.begin();
        
        // clear the screen with a black background
        TFTscreen.background(0, 0, 0);
        //set the text size
        TFTscreen.setTextSize(2);
        
        screen_width = TFTscreen.width();
        screen_height = TFTscreen.height();
        
        
        // set a random font color
        TFTscreen.stroke(0, 0, 255);
        
        Serial.begin(9600);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOOP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void loop() {
        long t1 = micros();
        read_sensors();
        crt_x = MIN_3(results[2], results[3], results[4]);
        crt_y = MIN_2(results[0], results[1]);
        TFTscreen.circle(crt_x, crt_y, 3);
        long t2 = micros();

//        Serial.println((t2 - t1) / 1000);

        print_measurements();
//        Serial.println((t2 - t1) / 1000);
//        Serial.println(MIN_3(1,2,3));
//        Serial.println(MIN_3(2,3,1));
//        Serial.println(MIN_3(3,1,2));
}

/* EOF */
