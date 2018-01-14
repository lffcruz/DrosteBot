#include "configs.h"
#include "Keypad.h"
#include "U8glib.h"
#include "string.h"
#include <EEPROM.h>

volatile S_CONFIGS configs;

// 0 for keypad, 1 for BT
boolean currentMode = 0;
// 0 for Stop, 1 for run
int runStatus = 0;
// holds the number of commanded steps
byte stepsCount = 0;
// buffer for the moves
char moves[56] = "";
// serial data status
const byte numChars = 60;
char receivedChars[numChars];
char buffer[numChars];
boolean newData = false;

byte rowPins[ROWS] = { KEY_PIN_3, KEY_PIN_4, KEY_PIN_5, KEY_PIN_6 };
byte colPins[COLS] = { KEY_PIN_2, KEY_PIN_1, KEY_PIN_0 };

Keypad customKeypad = Keypad( makeKeymap( hexaKeys ), rowPins, colPins, ROWS, COLS );
U8GLIB_SSD1306_128X64 u8g( U8G_I2C_OPT_NO_ACK );  // Display which does not send ACK

void setup()
{
    // Are eeprom configs valid? if not, set defaults and store them
    EEPROM.get( 0, configs );
    
    if ( 0 == configs.ticksPerRobot )
    {
        configs.ticksPerRobot = 20;

        EEPROM.put( 0, configs );
        EEPROM.get( 0, configs );
    }
    
    // Bluetooth speed
    Serial.begin( 9600 );

    // Set motors' pins
    pinMode( MOT_A_1_PIN, OUTPUT );
    pinMode( MOT_A_2_PIN, OUTPUT );
    pinMode( MOT_A_PWM_PIN, OUTPUT );

    pinMode( MOT_B_1_PIN, OUTPUT );
    pinMode( MOT_B_2_PIN, OUTPUT );
    pinMode( MOT_B_PWM_PIN, OUTPUT );

    clearOLED();
}

void loop()
{
    // Keypad stuff
    checkKeypad( customKeypad.getKey() );
    
    // LCD stuff
    u8g.firstPage();
    do {
        setGraphs();
    } while ( u8g.nextPage() );

    // Are we in bluetooth control mode?
    if ( 1 == currentMode && 
         0 == runStatus)
    {
        recvWithStartEndMarkers();
        showNewData();        
    }
}

void setGraphs()
{
    char tmp[15];
    
    // Clear the box
    u8g.setColorIndex( 0 );
    u8g.drawBox( 0, 0, 128, 64 );
    u8g.setColorIndex( 1 );

    // Top LCD section
    u8g.setFont( u8g_font_9x15B );
    u8g.setPrintPos( 0, 10 );
    u8g.print( "Mode " );
    
    u8g.setFont( u8g_font_9x15_78_79 );
    u8g.print( modeSymbols[currentMode] );

    u8g.setPrintPos( 110, 10 );
    u8g.setFont( u8g_font_9x15_67_75 );
    u8g.print( moveSymbols[NB_SYMBOL_MV + runStatus] );

    // draw arrays
    u8g.setPrintPos( 2, 28 );
    strncpy( tmp, &moves[0], 14 );
    tmp[15] = '\0';
    u8g.print( tmp );
  
    u8g.setPrintPos( 2, 40 );
    strncpy( tmp, &moves[14], 14 );
    tmp[15] = '\0';
    u8g.print( tmp );

    u8g.setPrintPos( 2, 52 );
    strncpy( tmp, &moves[28], 14 );
    tmp[15] = '\0';
    u8g.print( tmp );

    u8g.setPrintPos( 2, 64 );
    strncpy( tmp, &moves[42], 14 );
    tmp[15] = '\0';
    u8g.print( tmp );
    
}

// direction: 0 FW, 1 BW
void driveStraightDistance( int direction)
{
    //This will count up the total encoder ticks despite the fact that the encoders are constantly reset.
    int totalTicks = 0;
    
    while ( totalTicks < configs.ticksPerRobot )
    {
        //    motor control stuff
        //    Direction in {1;2}
        if ( 0 == direction )
        {
           // motors_moveFW(  );
        }
        else
        {
           // motors_moveBW(  );
        }

    }
}

// direction: 0 LT, 1 RT
void rotate90( int direction )
{
    //This will count up the total encoder ticks despite the fact that the encoders are constantly reset.
    int totalTicks = 0;

    //Monitor 'totalTicks', instead of the values of the encoders which are constantly reset.
    while ( abs( totalTicks ) < STEPS_ROT )
    {
        //    motor control stuff
        //    Direction in {0;1}
        if ( 0 == direction )
        {
           // motors_rotateLF( masterPower, slavePower );
        }
        else
        {
           // motors_rotateRT( masterPower, slavePower );
        }
    }
}

void runMoves()
{
    int currentStep = 0;
    // Toggle image as PLAY and update it
    runStatus = 1;

    u8g.firstPage();
    do {
        setGraphs();
    } while ( u8g.nextPage() );

    delay( 1000 );

    /* MOTOR CONTROL*/
    for ( currentStep = 0; currentStep < stepsCount; currentStep++ )
    {
        switch ( moves[currentStep] )
        {
            case 0x66: // LEFT
                rotate90( 0 );
                break;
            case 0x67: // FRONT
                driveStraightDistance( 0);
                break;
            case 0x68: // RIGHT
                rotate90( 1 );
                break;
            case 0x69: // BACK
                driveStraightDistance( 1 );
                break;
            default:
                break;
        }                
    }

    //toggle image as STOP
    runStatus = 0;

    if ( 1 == currentMode ) clearMoves();

    Serial.println( "DONE" );
    Serial.flush();
}

void checkKeypad(char customKey)
{
    switch ( customKey )
    {
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
        if ( 0 == currentMode )
        {
            addMove( customKey );
        }
        break;
    case '5':

        if ( 0 == currentMode )
        {
            //start run
            runMoves();
        }
        break;
    case '*':
        // toggle mode
        currentMode = currentMode != 1;
        if ( stepsCount ) clearMoves();
        break;
    case '+':
        if ( 0 == currentMode )
        {
            // clear all moves
            clearMoves();
        }
        break;
    case '#':
        if ( 0 == currentMode )
        {
            deleteMove();
        }
        break;
    default:
        break;
    }
}

void checkBTData( char customKey )
{
    switch ( customKey )
    {
    case 'F':
        addMove( 0x67 );
        break;
    case 'B':
        addMove( 0x69 );
        break;
    case 'L':
        addMove( 0x66 );
        break;
    case 'R':
        addMove( 0x68 );
        break;
    case '5':
        //start run
        Serial.println( "GO" );
        Serial.flush();
        runMoves();
        break;
    default:
        break;
    }
}

void clearOLED()
{
    u8g.firstPage();
    do {
    } while ( u8g.nextPage() );
}

void clearMoves()
{
    memset(moves, '\0', sizeof(moves));
    stepsCount = 0;
}

void addMove( int move )
{
    if ( stepsCount < 56 )
    {
        moves[stepsCount] = move;
        stepsCount++;
    }
}

void deleteMove()
{
    if ( stepsCount > 0 )
    {
        stepsCount--;
        moves[stepsCount] = '\0';        
    }
}

void recvWithStartEndMarkers()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while ( Serial.available() > 0 && newData == false ) {
        rc = Serial.read();

        if ( recvInProgress == true ) 
        {
            if ( rc != endMarker ) 
            {
                receivedChars[ndx] = rc;
                ndx++;
                if ( ndx >= numChars ) 
                {
                    ndx = numChars - 1;
                }
            }
            else 
            {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }
        else if ( rc == startMarker ) 
        {
            recvInProgress = true;
        }
    }
}

void showNewData()
{
    if ( newData == true ) 
    {
        if ( 0 == strcmp( receivedChars, "BOT" ) )
        {
            Serial.println("BOTOK");
            Serial.flush();
        }
        else if ( 0 == strncmp( receivedChars, "CFG", 3 ) )
        {
            strncpy( buffer, receivedChars, numChars );
            setConfig(&buffer[3]);
        }
        else if ( 0 == strcmp( receivedChars, "GETCFG" ) )
        {
            transmitConfig();
        }
        else
        {
            int counter = 0;
            int length = strlen( receivedChars );
            // this is a commands buffer
            // process commands: add them to the buffer (append the run command to the end of them and call checkBTData for each char)
            for ( counter = 0; counter < length; counter++ )
            {
                checkBTData( receivedChars[counter] );
            }

            if ( strlen( moves ) > 0 )
            {
                // Append run command
                checkBTData( '5' );
            }
        }

        newData = false;
    }
}

/* MOTOR ROUTINES*/
void motors_moveBW( int speedA, int speedB )
{
    digitalWrite( MOT_A_1_PIN, HIGH );
    digitalWrite( MOT_A_2_PIN, LOW );
    digitalWrite( MOT_B_1_PIN, HIGH );
    digitalWrite( MOT_B_2_PIN, LOW );

    analogWrite( MOT_A_PWM_PIN, speedA );
    analogWrite( MOT_B_PWM_PIN, speedB );
}

void motors_moveFW( int speedA, int speedB )
{
    digitalWrite( MOT_A_1_PIN, LOW );
    digitalWrite( MOT_A_2_PIN, HIGH );
    digitalWrite( MOT_B_1_PIN, LOW );
    digitalWrite( MOT_B_2_PIN, HIGH );

    analogWrite( MOT_A_PWM_PIN, speedA );
    analogWrite( MOT_B_PWM_PIN, speedB );
}

void motors_rotateRT( int speedA, int speedB )
{
    digitalWrite( MOT_A_1_PIN, LOW );
    digitalWrite( MOT_A_2_PIN, HIGH );
    digitalWrite( MOT_B_1_PIN, HIGH );
    digitalWrite( MOT_B_2_PIN, LOW );

    analogWrite( MOT_A_PWM_PIN, speedA );
    analogWrite( MOT_B_PWM_PIN, speedB );
}

void motors_rotateLF( int speedA, int speedB )
{
    digitalWrite( MOT_A_1_PIN, HIGH );
    digitalWrite( MOT_A_2_PIN, LOW );
    digitalWrite( MOT_B_1_PIN, LOW );
    digitalWrite( MOT_B_2_PIN, HIGH );

    analogWrite( MOT_A_PWM_PIN, speedA );
    analogWrite( MOT_B_PWM_PIN, speedB );
}

void motors_neutral()
{
    digitalWrite( MOT_A_1_PIN, LOW );
    digitalWrite( MOT_A_2_PIN, LOW );
    digitalWrite( MOT_B_1_PIN, LOW );
    digitalWrite( MOT_B_2_PIN, LOW );

    digitalWrite( MOT_A_PWM_PIN, HIGH );
    digitalWrite( MOT_B_PWM_PIN, HIGH );
}

void setConfig(char* buff)
{
    configs.ticksPerRobot = atoi( buff );

    EEPROM.put( 0, configs );
    EEPROM.get( 0, configs );

    Serial.println( "OKCFG" );
    Serial.flush();
}

void transmitConfig()
{
    Serial.print("CFG");
    Serial.print( configs.ticksPerRobot );
}
