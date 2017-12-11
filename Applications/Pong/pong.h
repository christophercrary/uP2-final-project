/************************************************************************************
 *  File name: pong.h
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary
 *  Purpose: Header file for Pong application, created by Brit Chesley and
 *           Chris Crary.
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////
#include <stdbool.h>
#include <stdint.h>
#include "G8RTOS.h"
#include "LCD.h"
/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef PONG_H_
#define PONG_H_

#include "Applications.h"

#define MAX_NUM_OF_PLAYERS  2
#define MAX_NUM_OF_BALLS    8
#define NUM_OF_PLAYERS_PLAYING 2

/* player number definitions */
#define HOST    0
#define CLIENT  1

/* screen boundaries of ILI3925 LCD */
#define MAX_SCREEN_X     320
#define MAX_SCREEN_Y     240
#define MIN_SCREEN_X     0
#define MIN_SCREEN_Y     0
#define SCREEN_SIZE      76800

/* boundaries for buttons used to select a player role */
#define SELECT_HOST_BUTTON_X_MIN 20
#define SELECT_HOST_BUTTON_X_MAX 150
#define SELECT_HOST_BUTTON_Y_MIN 170
#define SELECT_HOST_BUTTON_Y_MAX 220
#define SELECT_CLIENT_BUTTON_X_MIN 170
#define SELECT_CLIENT_BUTTON_X_MAX 300
#define SELECT_CLIENT_BUTTON_Y_MIN 170
#define SELECT_CLIENT_BUTTON_Y_MAX 220

/* Size of game arena */
#define ARENA_MIN_X                  40
#define ARENA_MAX_X                  280
#define ARENA_MIN_Y                  0
#define ARENA_MAX_Y                  240

#define ARENA_LEFT_BOUNDARY_WIDTH       1
#define ARENA_LEFT_BOUNDARY_HEIGHT      240
#define ARENA_LEFT_BOUNDARY_CENTER_X    (ARENA_LEFT_BOUNDARY_WIDTH>>1)
#define ARENA_LEFT_BOUNDARY_CENTER_Y    (ARENA_LEFT_BOUNDARY_HEIGHT>>1)
#define ARENA_RIGHT_BOUNDARY_WIDTH      1
#define ARENA_RIGHT_BOUNDARY_HEIGHT     240
#define ARENA_RIGHT_BOUNDARY_CENTER_X    (ARENA_RIGHT_BOUNDARY_WIDTH>>1)
#define ARENA_RIGHT_BOUNDARY_CENTER_Y    (ARENA_RIGHT_BOUNDARY_HEIGHT>>1)

/* location of each player's scoreboard */
#define PLAYER_RED_SCORE_TEXT_MIN_X     (MIN_SCREEN_X+4)
#define PLAYER_RED_SCORE_TEXT_MIN_Y     MIN_SCREEN_Y
#define PLAYER_RED_SCORE_NUM_MIN_X     (MIN_SCREEN_X+9)
#define PLAYER_RED_SCORE_NUM_MIN_Y     (MIN_SCREEN_Y+13)
#define PLAYER_BLUE_SCORE_TEXT_MIN_X     MIN_SCREEN_X
#define PLAYER_BLUE_SCORE_TEXT_MIN_Y    (MAX_SCREEN_Y-28)
#define PLAYER_BLUE_SCORE_NUM_MIN_X     (MIN_SCREEN_X+9)
#define PLAYER_BLUE_SCORE_NUM_MIN_Y     (MAX_SCREEN_Y-15)

/* Size of objects */
#define PADDLE_LEN                   64
#define PADDLE_LEN_D2                (PADDLE_LEN >> 1)
#define PADDLE_WID                   4
#define PADDLE_WID_D2                (PADDLE_WID >> 1)
#define BALL_SIZE                    4
#define BALL_SIZE_D2                 (BALL_SIZE >> 1)

/* Centers for paddles at the center of the sides */
#define PADDLE_X_CENTER              MAX_SCREEN_X >> 1

/* initial x-axis location of each player's paddle */
#define PADDLE_INITIAL_X_MIN            128
#define PADDLE_INITIAL_X_MAX            192

/* y-axis boundaries of each player's paddle */
#define TOP_PADDLE_Y_MIN        ARENA_MIN_Y
#define TOP_PADDLE_Y_MAX        (ARENA_MIN_Y+PADDLE_WID)
#define BOTTOM_PADDLE_Y_MIN     (ARENA_MAX_Y-PADDLE_WID)
#define BOTTOM_PADDLE_Y_MAX     ARENA_MAX_Y

/* Edge limitations for player's center coordinate */
#define HORIZ_CENTER_MAX_PL          (ARENA_MAX_X - PADDLE_LEN_D2)
#define HORIZ_CENTER_MIN_PL          (ARENA_MIN_X + PADDLE_LEN_D2)

/* Constant centers of each player */
#define TOP_PLAYER_CENTER_Y          (ARENA_MIN_Y + PADDLE_WID_D2)
#define BOTTOM_PLAYER_CENTER_Y       (ARENA_MAX_Y - PADDLE_WID_D2)

/* Amount of allowable space for collisions with the sides of paddles */
#define WIGGLE_ROOM                  2

/* Value for velocities from contact with paddles */
#define _1_3_PADDLE                  11

/* Edge limitations for ball's center coordinate */
#define HORIZ_CENTER_MAX_BALL        (ARENA_MAX_X - BALL_SIZE_D2)
#define HORIZ_CENTER_MIN_BALL        (ARENA_MIN_X + BALL_SIZE_D2)
#define VERT_CENTER_MAX_BALL         (ARENA_MAX_Y - BALL_SIZE_D2)
#define VERT_CENTER_MIN_BALL         (ARENA_MIN_Y + BALL_SIZE_D2)

/* Maximum ball speed */
#define MAX_BALL_SPEED               6

/* arena background color */
#define ARENA_BACK_COLOR             LCD_BLACK

/* arena boundary color */
#define ARENA_BOUNDARY_COLOR         LCD_WHITE

/* initial ball color */
#define INITIAL_BALL_COLOR           LCD_WHITE

/* Offset for printing player to avoid blips from left behind ball */
#define PRINT_OFFSET                10

/* Used as status LEDs for Wi-Fi */
#define BLUE_LED BIT2
#define RED_LED BIT0

/* Enum for player identifier */
// this allows for the game to be expanded eventually
// to allow more players
typedef enum
{
    PLAYER1 = 0,        // host
    PLAYER2 = 1         // client
}playerNumber_t;

/* Enums for player colors */
typedef enum
{
    PLAYER_RED = LCD_RED,
    PLAYER_BLUE = LCD_BLUE
}playerColor_t;

/* Enums for player numbers */
typedef enum
{
    BOTTOM = 0,
    TOP = 1
}playerPosition_t;

/* typedef for color type */
typedef uint16_t color_t;

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////

extern semaphore_t LCDMutex;
extern semaphore_t CC_3100Mutex;
extern semaphore_t PlayerMutex;

////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

#pragma pack ( push, 1)

/*
 * Struct of a single 16-bit coordinate point's information
 */
typedef struct
{
    int16_t x;      // x-coordinate
    int16_t y;      // y-coordinate
}Point_t;

/*
 * Struct to be sent from the client to the host
 */

typedef struct
{
    uint32_t IP_address;    // client's dynamically chosen IP address
    int16_t displacement;   // ???
    playerNumber_t player;        // used to identify player number
    bool isReady;           // ???
    bool hasJoined;
    bool hasAcknowledged;
}ClientInfo_t;

/*
 * Player's (paddle) info to be used by both host and client
 * Client responsible for translation (???)
 */
typedef struct
{
    int16_t currentCenterX;     // paddle's y coordinate does not change
    playerColor_t color;
    playerPosition_t position;
}Player_t;

/*
 * Struct of all the balls, only changed by the host
 */
typedef struct
{
    Point_t center;     // ball's center point (x and y coordinate)
    color_t color;
    bool isAlive;
}Ball_t;

/*
 * Struct of important game information
 * to be sent from the host to the client
 */
typedef struct
{
    ClientInfo_t client;        // why do we need this??
    Player_t players[MAX_NUM_OF_PLAYERS];       // players' paddle info
    Ball_t balls[MAX_NUM_OF_BALLS];
    uint16_t ballCount;
    uint8_t winner;
    bool gameDone;
    uint16_t LEDScores[2];      // uint8_t or uint16_t??
    uint8_t overallScores[2];
} GameState_t;
#pragma pack ( pop )

/*
 * Struct of all the previous ball locations, only changed by self for drawing!
 */
typedef struct
{
    Point_t center;
}PrevBall_t;

// the following is not currently used!
/*
 * Struct of 2D rectangular object information (AABB)
 * This structure is used only to check collisions between AABBs
 * Note: AABB stands for axis-aligned bounded box
 */
typedef struct
{
    uint16_t length;
    uint16_t width;
    Point_t center;
}AABB_t;

/*
 * Struct of all the previous players locations, only changed by self for drawing
 */
typedef struct
{
    int16_t centerX;        // used to determine player's previous paddle x-coordinate
}PrevPlayer_t;

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////

////////////////////////////PUBLIC FUNCTION PROTOTYPES///////////////////////////////

/*
 * Returns either Host or Client depending on button press
 */
playerNumber_t GetPlayerRole();

/*
 * Draw players given center X center coordinate
 */
void DrawPlayer(Player_t *player);

/*
 * Updates player's paddle based on current and new center
 */
void UpdatePlayerOnScreen(PrevPlayer_t *prevPlayerIn, Player_t *outPlayer);

/*
 * Function updates ball position on screen
 */
void UpdateBallOnScreen(PrevBall_t *previousBall, Ball_t *currentBall, uint16_t outColor);

/*
 * Draw's entire ball and updates center
 */
void UpdateBall(Ball_t *ball);

/*
 * Initializes and prints initial game state
 */
void InitBoardState();

//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////


////////////////////////////////////THREADS//////////////////////////////////////////

/****************************** Aperiodic Threads **********************************/

/*
 * Aperiodic event thread that waits for the hosts button press to start a new game
 */
void Host_button_press();

/************************************************************************************
 * Name: aperiodic_select_player
 * Purpose: Aperiodic event created at the initial start of the Pong game. This
 *          thread relies on the user to one of the options presented on the LCD
 *          screen.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_select_player(void);

/****************************** Aperiodic Threads **********************************/


/******************************** Client Threads ************************************/
/*
 * Thread for client to join game
 */
void JoinGame();

/*
 * Thread that receives game state packets from host
 */
void ReceiveDataFromHost();

/*
 * Thread that sends UDP packets to host
 */
void SendDataToHost();

/*
 * Thread to read client's joystick
 */
void ReadJoystickClient();

/*
 * End of game for the client
 */
void EndOfGameClient();

/******************************** Client Threads ************************************/


/********************************* Host Threads *************************************/
/*
 * Thread for the host to create a game
 */
void CreateGame();

/*
 * Thread that sends game state to client
 */
void SendDataToClient();

/*
 * Thread that receives UDP packets from client
 */
void ReceiveDataFromClient();

/*
 * Generate Ball thread
 */
void GenerateBall();

/*
 * Thread to read host's joystick
 */
void ReadJoystickHost();

/*
 * Thread to create and move a single ball
 */
void Ball();

/*
 * End of game for the host
 */
void EndOfGameHost();

/********************************* Host Threads *************************************/


/******************************** Common Threads ************************************/

/************************************************************************************
 * Name: thread_start_game
 * Purpose: High priority thread to display initial game screen, and enable the
 *          aperiodic_select_player aperiodic event thread, allowing the user to
 *          select a player role by pressing the LCD screen.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_start_game(void);

/************************************************************************************
 * Name: thread_select_player
 * Purpose: High priority thread created from touch being made to LCD Touch Panel,
 *          at the initial loading of the game.
 *          Before entering this thread, the LCD's Touch Panel interrupt on
 *          port 4, pin 0 was disabled in the aperiodic thread aperiodic_select_player.
 *          This thread will choose a player type, based on the location of the
 *          user's press. If a valid selection was made,
 *          an additional thread either for the host or client will be made.
 *          If no valid choice was made (LCD press location was not valid),
 *          no additional thread is created, and this thread will sleep for 500ms, as to
 *          debounce the LCD Touch Panel. After the 500ms slumber, the Touch Panel
 *          interrupt on port 4, pin 0 will be rearmed, and the proper interrupt flag
 *          will be cleared.
 *          In either case, this thread will ultimately
 *          kill itself, as to allow the LCD touch panel screen to be pressed again,
 *          checking again if a player role was chosen.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_select_player(void);

/*
 * Idle thread
 */
void Idle();

/*
 * Thread to draw all the objects in the game
 */
void DrawObjects();

/*
 * Thread to update LEDs based on score
 */
void MoveLEDs();

/******************************** Common Threads ***********************************/

/////////////////////////////////END OF THREADS//////////////////////////////////////

#endif      // end of header guard

////////////////////////////////END OF pong.h////////////////////////////////////
