#include "configs.h"
#include "string.h"
#include <SoftwareSerial.h>
#include <Servo.h>

typedef struct cmdPair
{
    char command;
    int value;
};

Servo myservo;

// holds the number of commanded steps
byte stepsCount = 0;

// buffer for the moves
cmdPair moves[56];

// serial data status
const byte numChars = 350;
char receivedChars[numChars];
boolean newData = false;

SoftwareSerial blueSerial( BT_TX_PIN, BT_RX_PIN);

void setup()
{    
    // Bluetooth speed
    blueSerial.begin( 9600 );

    // Debug info
    Serial.begin( 9600 );           // set up Serial library at 9600 bps

    // Set motors' pins
    pinMode( MOT_A_1_PIN, OUTPUT );
    pinMode( MOT_A_PWM_PIN, OUTPUT );

    pinMode( MOT_B_1_PIN, OUTPUT );
    pinMode( MOT_B_PWM_PIN, OUTPUT );

    myservo.attach( SRV_PIN );

    memset(moves, 0, sizeof(moves));
}

void loop()
{
    // check UART and get data if available
    recvWithStartEndMarkers();
    // process the received data
    showNewData();
    // give it some rest
    delay( 100 );
}

void recvWithStartEndMarkers()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while ( blueSerial.available() > 0 && newData == false ) {

        rc = blueSerial.read();

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

        Serial.print( "showNewData() data string: " ); 
        Serial.println( receivedChars );

        if ( 0 == strcmp( receivedChars, "BOT" ) )
        {
            blueSerial.println( "BOTOK" );
            blueSerial.flush();
        }
        else
        {
            // Read each command pair 
            char* command = strtok( receivedChars, "&" );
            while ( command != 0 )
            {
                String cmd( command );

                int sepIndex = cmd.indexOf( ':' );
                
                int firstValue = (int)(cmd.substring( 0, sepIndex ).charAt(0));
                int secondValue = cmd.substring( sepIndex + 1 ).toInt();
                
                checkBTData( firstValue, secondValue );
                
                // Find the next command in input string
                command = strtok( 0, "&" );
            }

            if ( stepsCount > 0 )
            {
                // Append run command
                checkBTData( '5', 0 );
            }
        }

        newData = false;
    }
}

void checkBTData( int command, int value )
{
    switch ( command )
    {
    case 'F':
    case 'B':
    case 'L':
    case 'R':
    case 'U':
    case 'D':
        addMove( command, value );
        break;
    case '5':
        //start run
        blueSerial.println( "GO" );
        blueSerial.flush();
        runMoves();
        break;
    default:
        break;
    }
}

void clearMoves()
{
    memset( moves, 0, sizeof( moves ) );
    stepsCount = 0;
}

void addMove( int move, int value )
{
    if ( stepsCount < 56 )
    {
        moves[stepsCount].command = move;
        moves[stepsCount].value = value;

        stepsCount++;
    }
}

/* MOTOR ROUTINES*/
void motors_moveFW( int value )
{
    int pulse = 0;

    digitalWrite( MOT_A_1_PIN, LOW ); 
    digitalWrite( MOT_B_1_PIN, HIGH ); 

    for ( pulse = 0; pulse < value; pulse++ ) 
    {
        digitalWrite( MOT_A_PWM_PIN, HIGH ); //Trigger one step
        digitalWrite( MOT_B_PWM_PIN, HIGH ); //Trigger one step
        delay( 1 );
        digitalWrite( MOT_A_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        digitalWrite( MOT_B_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        delay( 1 );
    }
}

void motors_moveBW( int value )
{
    int pulse = 0;

    digitalWrite( MOT_B_1_PIN, LOW );
    digitalWrite( MOT_A_1_PIN, HIGH );

    for ( pulse = 0; pulse < value; pulse++ )  //Loop the forward stepping enough times for motion to be visible
    {
        digitalWrite( MOT_A_PWM_PIN, HIGH ); //Trigger one step
        digitalWrite( MOT_B_PWM_PIN, HIGH ); //Trigger one step
        delay( 1 );
        digitalWrite( MOT_A_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        digitalWrite( MOT_B_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        delay( 1 );
    }
}

void motors_rotateRT( int value )
{
    int pulse = 0;

    digitalWrite( MOT_A_1_PIN, LOW );
    digitalWrite( MOT_B_1_PIN, LOW );

    for ( pulse = 0; pulse < value; pulse++ )  //Loop the forward stepping enough times for motion to be visible
    {
        digitalWrite( MOT_A_PWM_PIN, HIGH ); //Trigger one step
        digitalWrite( MOT_B_PWM_PIN, HIGH ); //Trigger one step
        delay( 1 );
        digitalWrite( MOT_A_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        digitalWrite( MOT_B_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        delay( 1 );
    }
}

void motors_rotateLF( int value )
{
    int pulse = 0;

    digitalWrite( MOT_A_1_PIN, HIGH );
    digitalWrite( MOT_B_1_PIN, HIGH );

    for ( pulse = 0; pulse < value; pulse++ )  //Loop the forward stepping enough times for motion to be visible
    {
        digitalWrite( MOT_A_PWM_PIN, HIGH ); //Trigger one step
        digitalWrite( MOT_B_PWM_PIN, HIGH ); //Trigger one step
        delay( 1 );
        digitalWrite( MOT_A_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        digitalWrite( MOT_B_PWM_PIN, LOW ); //Pull step pin low so it can be triggered again
        delay( 1 );
    }
}

void motors_neutral()
{
    digitalWrite( MOT_A_PWM_PIN, LOW );
    digitalWrite( MOT_B_PWM_PIN, LOW );
}

void pen_up()
{
    myservo.write( SRV_DW_ANGLE );     // tell servo to go to position SRV_DW_ANGLE
    delay( 15 );                       // waits 15ms for the servo to reach the position
}

void pen_down()
{
    myservo.write( SRV_UP_ANGLE );     // tell servo to go to position SRV_UP_ANGLE
    delay( 15 );
}

void runMoves()
{
    int currentStep = 0;

    /* MOTOR CONTROL*/
    for ( currentStep = 0; currentStep < stepsCount; currentStep++ )
    {
        switch ( moves[currentStep].command )
        {
            case 'F': 
                motors_moveFW( moves[currentStep].value * STEPS_CM );
                break;
            case 'B': 
                motors_moveBW( moves[currentStep].value * STEPS_CM );
                break;
            case 'R': 
                motors_rotateRT( moves[currentStep].value * STEPS_DEG );
                break;
            case 'L': 
                motors_rotateLF( moves[currentStep].value * STEPS_DEG );
                break;
            case 'U':
                pen_up();
                break;
            case 'D':
                pen_down();
                break;
            default:
                break;
        }                
    }

    clearMoves();

    blueSerial.println( "DONE" );
    blueSerial.flush();
}
