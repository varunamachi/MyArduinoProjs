#include <VirtualWire.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <Arduino.h>



Adafruit_BMP085 bmp;
DHT dht(uint8_t( 6 ), DHT11);

const uint8_t TX_PIN = 3;

const float SEA_LEVEL = 101325.0f;

int count = 0;



void setup(){
    Serial.begin(9600);
    delay(2000);//Wait before accessing Sensor
    dht.begin();
    if (!bmp.begin()) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    }
    // Initialize the IO and ISR
    vw_set_tx_pin( TX_PIN );
    vw_set_ptt_inverted( true ); // Required for DR3100
    vw_setup( 2000 );	 // Bits per sec
}


void send( const String &data )
{
    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send((uint8_t *)data.c_str(), data.length() );
    Serial.println( data );
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite( 13, false );
}

float readLightIntensity()
{
    return 0;
}


void loop(){
//    Serial.print("Humidity: ");
//    Serial.println(dht.readHumidity());
//    Serial.print("Temperature: ");
//    Serial.println(dht.readTemperature());
//    Serial.print("Pressure: ");
//    Serial.println(bmp.readPressure() / 1000);
//    Serial.print("Altitude: ");
//    Serial.println(bmp.readAltitude(SEA_LEVEL));
//    Serial.print("Temperature: ");
//    Serial.println(bmp.readTemperature());
//    Serial.println("----------------------------------");


    float val = dht.readTemperature();
    String prefix = "#T";
    switch( count ) {
    case 0:
        break;
    case 1:
        prefix = "#H";
        val = dht.readHumidity();
        break;
    case 2:
        prefix = "#P";
        val = bmp.readPressure() / 1000;
        break;
    case 3:
        prefix = "#A";
        val = bmp.readAltitude( SEA_LEVEL );
        break;
    case 4:
        prefix = "#L";
        val = readLightIntensity();
        break;
    }
    char *buf = new char[ 7 ];
    memset( buf, 0, 7 );
    dtostrf( val, 7, 2, buf );
    send( prefix + buf );
    delete buf;
    delay(2000);
    count = ( count + 1 ) % 5;
}

int main()
{
    init();
    setup();
    while( true ) {
        loop();
    }
    return 0;
}
