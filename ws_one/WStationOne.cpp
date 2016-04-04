#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <HardwareSerial.h>

#ifdef HAVE_LCD
    #include <LiquidCrystal.h>
    LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 );
#endif


const float SEA_LEVEL = 101325.0f;

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

#ifdef HAVE_LCD
    lcd.begin( 16, 2 );
#endif

    dht.begin();
    if (!bmp.begin()) {
        Serial.println("BMPErr");
    }

    rtc.begin();
    if ( ! rtc.isrunning() ) {
      Serial.println("RTCErr");
      rtc.adjust( DateTime( F( __DATE__ ), F( __TIME__ )));
    }

    Serial.begin( 9600 );           // Debugging only
    BH1750_Init(BH1750_address);
    delay( 2000 );
}


void showTime( bool onlyLcd )
{
    DateTime now = rtc.now();
#ifdef HAVE_LCD
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
#else
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.print(now.second());
#endif
}

void loop()
{
#ifdef HAVE_LCD
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
    delay( 1000 );
#else
    showTime( false );
    Serial.print( "Temparature: " );
    Serial.println( dht.readTemperature() );
    Serial.print( "Humidity: " );
    Serial.println( dht.readHumidity() );
    Serial.print( "Pressure: " );
    Serial.println( bmp.readPressure() / 1000 );
    Serial.print( "Light: " );
    Serial.println( readLightIntensity() );
    delay( 1000 );
#endif
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
