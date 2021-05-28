// include TFT and SPI libraries
#include <TFT.h>  
#include <SPI.h>

// pins
#define ECHO1 2
#define TRIG1 3

#define ECHO2 4
#define TRIG2 5

#define ECHO_OFFSET 0
#define TRIG_OFFSET 1

// pin definition for Arduino UNO
#define cs   10
#define dc   9
#define rst  8

// colors
#define COLOR_FULL 255
#define COLOR_EMPTY 0

// numbers - constants
#define NO_OF_SENSORS 2
#define NO_OF_MEASUREMENTS 5

// max distance = 40 cm??
#define INFINITY 40

// some magic defines for median of 5 elements
#define swap(a,b) (a) ^= (b); (b) ^= (a); (a) ^= (b);
#define sort(a,b) if((a)>(b)){ swap((a),(b)); }


int measurements[NO_OF_SENSORS][NO_OF_MEASUREMENTS];
int results[NO_OF_SENSORS];
int last_results[NO_OF_SENSORS];

int screen_width;
int screen_height;

unsigned int last_distance1 = 0;
unsigned int last_distance2 = 0;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
int x = 6;

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
        
        // this last one is obviously unnecessary for the median
        // sort(d,e);
}

// reads all the sensors five times and gets the median for each sensors
// output is stored in `int results[]`
void read_sensors()
{
        // for each of (5 times) cycle through each sensor
        for (int i = 0; i < NO_OF_MEASUREMENTS; i++) {
        
                // read each sensor back-to-back
                for (int j = 0; j < NO_OF_SENSORS; j++) {
                        // sensors are indexed from 1 to NO_OF_SENSORS
                        measurements[j][i] = sensor_distance(j + 1);
                }
        }
        
        char buff[100];

        // reoreder measurements
        for (int i = 0; i < NO_OF_SENSORS; i++) {
                median(measurements[i]);
                // store results
                results[i] = measurements[i][2];
        }

//  for (int i = 0; i < NO_OF_SENSORS; i++) {
//    median(measurements[i]);
//    sprintf(buff, "%c {%d} Valori citite : [%d, %d, %d, %d, %d] -> %d",
//      i == 0 ? '\t' : ' ',
//      i,
//      measurements[i][0],
//      measurements[i][1],
//      measurements[i][2],
//      measurements[i][3],
//      measurements[i][4],
//      measurements[i][2]);
//    Serial.println(buff);
//  }
}

void transfers_results_to_old()
{
        for (int i = 0; i < NO_OF_SENSORS; i++) {
                last_results[i] = results[i];
        }
}

void init_sensors_pins()
{
        // sensor number 1
        pinMode(ECHO1, INPUT);
        pinMode(TRIG1, OUTPUT);
        
        // sensor number 2
        pinMode(ECHO2, INPUT);
        pinMode(TRIG2, OUTPUT);
}

// sensor number i has to have trig pin = 2*i and echo pin 2*i+1
unsigned int sensor_distance(unsigned int sensor_number)
{
        unsigned int trig = sensor_number * 2 + TRIG_OFFSET;
        unsigned int echo = sensor_number * 2 + ECHO_OFFSET;
        
        unsigned int distance = 0;
        
        digitalWrite(trig, LOW);
        delayMicroseconds(2);
        digitalWrite(trig, LOW);
        delayMicroseconds(2);
        digitalWrite(trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig, LOW);
        
        unsigned long duration = pulseIn(echo, HIGH);
        distance = 2 * duration * 0.034; // instead of cm will be 1/4 of cm
        return distance;
}

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

void loop() {
        long t1 = micros();
        read_sensors();
        
        unsigned int distance1 = measurements[0][2];
        unsigned int distance2 = measurements[1][2];
//        draw_test_lines(distance1, distance2);
        //  delay(10);

        // drow them points
        TFTscreen.point(distance1, distance2);
        
        long t2 = micros();

        Serial.print(measurements[0][2]);
        Serial.print("    ");
        Serial.print(measurements[1][2]);
        Serial.print("    ");        
        Serial.println((t2 - t1) / 1000);
}
