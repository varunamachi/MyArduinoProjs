#include <Arduino.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Wire.h>
#include <stdlib.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <ESP8266wifi.h>

#include "Constants.h"
#include "Data.h"

#define SW_SERIAL_RX_PIN  9 //  Connect this pin to TX on the esp8266
#define SW_SERIAL_TX_PIN  10 //  Connect this pin to RX on the esp8266
#define ESP8266_RESET_PIN 5 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

const int INDEX_READ = 5;
const int INDEX_WIFI = 60;

const char RST[] PROGMEM = "RST";
const char IDN[] PROGMEM = "*IDN?";

const float SEA_LEVEL = 101325.0f;
const int BH1750_address = 0x23; // i2c Addresse

// TCP Commands

LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 );
RTC_DS1307 rtc;
Adafruit_BMP085 bmp;
DHT dht(uint8_t( 6 ), DHT11);
SoftwareSerial swSerial( SW_SERIAL_RX_PIN, SW_SERIAL_TX_PIN );
ESP8266wifi wifi( swSerial, swSerial, ESP8266_RESET_PIN, Serial );

byte buff[ 2 ];
uint8_t wifi_started = false;
uint8_t counter = 0;

void wifiInit() {
    swSerial.begin(115200);
    while ( ! swSerial );
    Serial.println("Starting wifi");
    wifi.setTransportToTCP();// this is also default
    // wifi.endSendWithNewline(false);
    wifi_started = wifi.begin();
    if( wifi_started ) {
        wifi.connectToAP( Constants::SSID, Constants::KEY );
        wifi_started = wifi.connectToServer( Constants::SERVER_IP,
                                             Constants::SERVER_PORT );
        if( wifi_started ) {
            Serial.println( "Wifi Started!!" );
        }
        else {
            Serial.println( "Couldnt start wifi" );
        }
        //    wifi.startLocalServer(SERVER_PORT);
    }
    else {
        // ESP8266 isn't working..
    }
}


void BH1750_Init( int address )
{
    Wire.beginTransmission(address);
    Wire.write( 0x10 );
    Wire.endTransmission();
}

byte BH1750_Read( int address )
{
    byte i = 0;
    Wire.beginTransmission( address );
    Wire.requestFrom( address, 2 );
    while( Wire.available() ) {
        buff[ i ] = Wire.read();
        ++ i;
    }
    Wire.endTransmission();
    return i;
}

double readLightIntensity()
{
    double valf = 0;
    if( BH1750_Read( BH1750_address ) == 2 ) {
        valf= (( buff[ 0 ] << 8) | buff[ 1 ]) / 1.2;
        if( valf < 0 ) {
            Serial.print( "> 65535" );
        }
    }
    return valf;
}

void setup()
{
    delay(1000);

    dht.begin();
    if( ! bmp.begin() ) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    }

    rtc.begin();
    if ( ! rtc.isrunning() ) {
        Serial.println("RTC is NOT running!");
        rtc.adjust( DateTime( F( __DATE__ ), F( __TIME__ )));
    }

    Serial.begin( 9600 );           // Debugging only
    lcd.begin( 16, 2 );
    BH1750_Init(BH1750_address);
    delay( 2000 );
    wifiInit();
}


void showTime( bool onlyLcd )
{
    DateTime now = rtc.now();
    char buffer[16];
    sprintf( buffer,
             "%02d:%02d:%02d",
             now.hour(),
             now.minute(),
             now.second() );

    lcd.setCursor(0,0);
    lcd.print(buffer);

    if( ! onlyLcd ) {
        Serial.print( buffer );
    }
}


void printLCD( const Data::WeatherData &weatherData ) {
    static int quantityIndex = 0;
    lcd.setCursor( 0, 1 );
    lcd.print( "                " );
    double val = weatherData.temparature();
    String quantity = "Tempr: ";
    switch( quantityIndex ) {
    case 0:
        break;
    case 1:
        quantity  = "Hmdty: ";
        val = weatherData.humidity();
        break;
    case 2:
        quantity  = "Presr: ";
        val = weatherData.pressure();
        break;
    case 3:
        quantity  = "Altde: ";
        val = weatherData.altitude();
        break;
    case 4:
        quantity  = "Light: ";
        val = weatherData.light();
        break;
    }
    quantityIndex = ( quantityIndex + 1 ) % 5;
    lcd.setCursor( 0, 1 );
    lcd.print( quantity  );
    lcd.setCursor( 8, 1 );
    lcd.print( val, 2 );
}

void printSerial( const Data::WeatherData &weatherData ) {
    Serial.println( "Hit..." );
}

void sendData( const Data::WeatherData &weatherData ) {
    static WifiConnection *connections;
    static char buffer[ 80 ];
    sprintf( buffer,
             "temp=%0.2f"
             "&humidity=%02.f"
             "&light=%0.2f"
             "&pressure=%0.2f"
             "&altitude=%0.2f",
        weatherData.temparature(),
        weatherData.humidity(),
        weatherData.light(),
        weatherData.pressure(),
        weatherData.altitude()
    );
    // check connections if the ESP8266 is there
    if( wifi_started ) {
        wifi.checkConnections( &connections );
    }
    // check for messages if there is a connection
    for( int i = 0; i < MAX_CONNECTIONS; i++ ) {
        if( connections[i].connected ) {
            if( wifi.send( SERVER, buffer )) {
                Serial.println("Sent Data...");
            }
        }
    }
}

void loop()
{
    static Data::WeatherData weatherData;
    showTime( true );
    if( counter % INDEX_READ == 0 ) {
        weatherData.setTemperature( dht.readTemperature() );
        weatherData.setHumidity( dht.readHumidity() );
        weatherData.setPressure( bmp.readPressure() / 1000 );
        weatherData.setAltitude( bmp.readAltitude( SEA_LEVEL ));
        weatherData.setLight( readLightIntensity() );

        printLCD( weatherData );
        printSerial( weatherData );
    }
    if( counter == 0 ) {
        sendData( weatherData );
    }
    counter = ( counter + 1 ) % INDEX_WIFI;
    delay( 1000 );
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
