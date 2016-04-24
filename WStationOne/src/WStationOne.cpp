#include <Arduino.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Wire.h>
#include <stdlib.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <HardwareSerial.h>
#include <AltSoftSerial.h>

const float SEA_LEVEL = 101325.0f;

LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 );
RTC_DS1307 rtc;
Adafruit_BMP085 bmp;
DHT dht(uint8_t( 6 ), DHT11);
int BH1750_address = 0x23; // i2c Addresse
byte buff[ 2 ];

uint8_t count = 0;
uint8_t timeUpdateCounter = 0;

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
    if (!bmp.begin()) {
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

void loop()
{
    if( timeUpdateCounter != 0 ) {
        showTime( true );
    }
    else {
        lcd.setCursor( 0, 1 );
        lcd.print( "                " );
        showTime( false );
        double val = dht.readTemperature();
        String quantity = "Tempr: ";
        switch( count ) {
        case 0:
            break;
        case 1:
            quantity  = "Hmdty: ";
            val = dht.readHumidity();
            break;
        case 2:
            quantity  = "Presr: ";
            val = bmp.readPressure() / 1000;
            break;
        case 3:
            quantity  = "Altde: ";
            val = bmp.readAltitude( SEA_LEVEL );
            break;
        case 4:
            quantity  = "Light: ";
            val = readLightIntensity();
            break;
        }

        lcd.setCursor( 0, 1 );
        lcd.print( quantity  );
        lcd.setCursor( 8, 1 );
        lcd.print( val, 2 );

        Serial.print( quantity );
        Serial.println( val );

        count = ( count + 1 ) % 5;
    }
    timeUpdateCounter = ( timeUpdateCounter + 1 ) % 3;
    Serial.println( timeUpdateCounter );
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

