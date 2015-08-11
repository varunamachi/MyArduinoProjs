#include <Arduino.h>
#include <VirtualWire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Wire.h>
#include <stdlib.h>

#define RECEIVE_PIN            6


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RTC_DS1307 rtc;

void setup()
{
    delay(1000);

    Serial.begin( 9600 );           // Debugging only
    vw_set_rx_pin( RECEIVE_PIN );
    vw_set_ptt_inverted( true );  	// Required for DR3100
    vw_setup( 2000 );	 			// Bits per sec
    vw_rx_start();       			// Start the receiver PLL running
    lcd.begin( 16, 2 );
    delay( 2000 );
}

void showTime()
{
    DateTime now = rtc.now();

    String timeStr =
            String( "" )
            + int( now.hour() )
            + String( ":" )
            + int( now.minute() )
            + ':'
            + now.second();
    Serial.println( timeStr );
    lcd.setCursor( 0, 0 );
    lcd.print( timeStr );
//    Serial.println( "Hello" );
//    lcd.setCursor( 0, 0 );
//    lcd.print( "H E L L O" );
}

void loop()
{
    uint8_t buf[ VW_MAX_MESSAGE_LEN ];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    showTime();
    if( vw_get_message( buf, &buflen )) // Non-blocking
    {
        //        const char *fieldName  = "";
        buflen = buflen < ( VW_MAX_MESSAGE_LEN - 1)
                                        ? buflen
                                        : VW_MAX_MESSAGE_LEN - 1;
        buf[ buflen ] = '\0';
        String msg(reinterpret_cast<char *>(buf));
        String fieldValue = msg.substring(2);
        Serial.println( msg );
        lcd.setCursor( 0, 1 );
        if( msg.startsWith( "#T" )) {
            lcd.print( "Tmp: " + fieldValue );
        }
        else if( msg.startsWith( "#H" )) {
            lcd.print( "Hmd: " + fieldValue );
        }
        else if( msg.startsWith( "#P" )) {
            lcd.print( "Prs: " + fieldValue );
        }
        else if( msg.startsWith( "#A" )) {
            lcd.print( "Alt: " + fieldValue );
        }
        else if( msg.startsWith( "#L" )) {
            lcd.print( "Lgt: " + fieldValue );
        }
    }
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
