/*
 ********************************
 * PRIVAT
 * CREATE MANDELBROT & JULIA MENGEN
 * (c) Thomas Dose
 * 2023
 ********************************
 */


#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

//#define DEBUG

#define MAX_X   800                                         // pixels in x direction
#define MAX_Y   800                                         // pixels in Y direction

#define MINUSX  -3                                          // left border of fractal
#define PLUSX    3                                          // right border of fractal
#define MINUSY  -3                                          // upper border of fractal
#define PLUSY    3                                          // lower border of fractal

#define COLORS  6                                           // number of r,g,b shades
#define MAX_DEPTH COLORS * COLORS * COLORS                  // total number of colors

using namespace cv;                                         // using opencv
using namespace std;                                        // using standard

//------------------------------------------------------------------------------------
// GLOBAL STRUCTURES
//------------------------------------------------------------------------------------
struct PIXEL {                                              // definition of one pixel
    uint8_t
    R ,                                                     // RED   Value {0..255}
    G ,                                                     // GREEN value {0..255}
    B;                                                      // BLUE  value {0..255}
};

struct FRAME {                                              // variable for real and imag
    float
    x_min,
    x_max,
    y_min,
    y_max;
    bool
    exit,                                                   // exit conditions
    mandel;                                                 // MAN JUL switch
};
//------------------------------------------------------------------------------------
// DECLARATION OF FUNCTIONS
//------------------------------------------------------------------------------------

Mat fractal(FRAME   frame,                                  // Real and emagi frame
            PIXEL*  Palette,                                // palette to be used
            Mat     image);                                  // image to paint on

PIXEL * make_pal ();                                        // creating a palette

FRAME get_key();                                            // ask for new values

void splash();                                              // initial welcome screen

char * make_string(FRAME    frame);                         // create the window string

//------------------------------------------------------------------------------------
// MAIN PROGARM
//------------------------------------------------------------------------------------
int main(int argc, char** argv)                             // main program
{
    // VARIABLEN -------------------------------------------------------------------
    
    PIXEL *Palette = make_pal();                            // create a palette
    
    Mat MANJUL(MAX_Y, MAX_X, CV_8UC3, Scalar(0,0,0));       // create empty image
    
    FRAME frame;                                            // define start frame
    
    // VARIABLEN -------------------------------------------------------------------

    frame.x_min   =   MINUSX,                               // fractal left border
    frame.x_max   =   PLUSX,                                // fractal right border
    frame.y_min   =   MINUSY,                               // fractal upper border
    frame.y_max   =   PLUSY;                                // fractal lower border
    
    frame.mandel  = true;                                   // start with Mandelbrot
    
    splash();                                               // welcome screen
    
    namedWindow("FRAKTALE",0);
    
    while(1)                                                // infinite loop
    {
        MANJUL = fractal (frame,
                           Palette,
                           MANJUL);
        
        setWindowTitle("FRAKTALE",make_string(frame));                   // change Name of Frame
        
        imshow("FRAKTALE", MANJUL);                         // show fractal
        
        frame = get_key();                                  // check key
        
        if (frame.exit == true) break;                      // was send exit
        
    }

    return 0;
}

//------------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------------
// create palette

PIXEL * make_pal (){
    
    // VARIABLEN START ---------------------------------------------------------------
    
    static PIXEL Palette [MAX_DEPTH] = {0};                 // prefilled pallete 8 * 8 * 8
    
    // VARIABLEN END  ----------------------------------------------------------------
    
    for (int rc = 0; rc < COLORS; rc++)                     // for all red
    {
        for (int gc = 0; gc < COLORS; gc++)                 // for all green
        {
            for (int bc = 0; bc < COLORS; bc++)             // for all blue
            {
                int pointer = rc +                          // calculate position
                gc * COLORS +
                bc * COLORS * COLORS;
                
                Palette[pointer].R =                        // set red value
                256 / (COLORS - rc);
                
                Palette[pointer].G =                        // set green value
                256 / (COLORS - gc);
                
                Palette[pointer].B =                        // set blue value
                256 / (COLORS - bc);
            }
        }
    }
    
    return Palette;                                         // send back filled palette
}

//------------------------------------------------------------------------------------
// calculate fractals

Mat fractal(FRAME   frame,
            PIXEL*  Palette,                                // palette to be used
            Mat     image)                                  // image to paint on

{
    // VARIABLEN START ---------------------------------------------------------------
    
    float
    cx,                                                     // real
    cy,                                                     // imag
    zx,                                                     // working
    zy,                                                     // working
    tempx;                                                  // buffer variable
    
    int
    count;                                                  // counter
    
    // VARIABLEN END  ----------------------------------------------------------------
 
#ifdef DEBUG
    for (int c=0; c< MAX_DEPTH; c++)
    {
        printf("R: %d | G: %d | B: %d \n", Palette[c].R,Palette[c].G,Palette[c].B);
    }
#endif
    
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_Y; x++)
        {
            if (frame.mandel == true)
            {
                cx = (x * (frame.x_max - frame.x_min) /
                      MAX_X ) + frame.x_min;              // convert pixel to real
                
                cy = (y * (frame.y_max - frame.y_min) /
                      MAX_Y ) + frame.y_min;                    // convert pixel to imag
                
                zx  =   0.0;                                  // feed in original value
                
                zy  =   0.0;                                  // feed in original value
                
            } else {
                zx = (x * (frame.x_max - frame.x_min) /
                      MAX_X ) + frame.x_min;              // convert pixel to real
                
                zy = (y * (frame.y_max - frame.y_min) /
                      MAX_Y ) + frame.y_min;                    // convert pixel to imag
                
                cx  =  -0.5;                                  // feed in original value
                
                cy  =   0.6;                                  // feed in original value
                
            }
            count = 0;                                      // reset counter
 
            while ((zx * zx + zy * zy < 4) &&               // less then threshold
                   (count < MAX_DEPTH))                     // and less then max depth
            {

                tempx = zx * zx - zy * zy + cx;             // save privious value
                
                zy = 2 * zx * zy + cy;                      // calucalte
                
                zx = tempx;                                 // update temp

                count++;                                    // increment connt
                
            }
            
#ifdef DEBUG
            printf("zx: %f | zy: %f \n", zx,zy);
#endif
            
            
            if ((zx * zx + zy * zy > 4))                    // reason for break - threshold
            {
                image.at<Vec3b>(x, y)[0] =                  // set red for pixel
                Palette[count].R;
                
                image.at<Vec3b>(x, y)[1] =                  // set green for pixel
                Palette[count].G;
                
                image.at<Vec3b>(x, y)[2] =                  // set blue for pixel
                Palette[count].B;
                
            } else {                                        // max-count
                
                image.at<Vec3b>(x, y)[0] =  0;              // all black
                
                image.at<Vec3b>(x, y)[1] =  0;              // all black
                
                image.at<Vec3b>(x, y)[2] =  0;              // all black
            }
        }
    }
    return image;                                           // send back filled image
}
//------------------------------------------------------------------------------------
// returns value based on pressed key

FRAME get_key()                                             // ask for new values
{
    // VARIABLEN START ---------------------------------------------------------------
    
    FRAME frame;                                            // frame container
    
    // VARIABLEN END  ----------------------------------------------------------------
    
    char key = (char) waitKey(0);                       // get a key
    
    if (key == 27) frame.exit = true;                         // break if `esc' key was pressed.
    
    if ((key == 82) ||
        (key == 56) ||
        (key == 184))                                   // move up
    {
        frame.x_min =
        frame.x_min +
        abs(frame.x_max - frame.x_min) / 10;
        
        frame.x_max =
        frame.x_max +
        abs(frame.x_max - frame.x_min) / 10;
    }
 
    if ((key == 84) ||
        (key == 50) ||
        (key == 178))                                   // move down
    {
        frame.x_min =
        frame.x_min -
        abs(frame.x_max - frame.x_min) / 10;
        
        frame.x_max =
        frame.x_max -
        abs(frame.x_max - frame.x_min) / 10;
    }
  
    if ((key == 83) ||
        (key == 54) ||
        (key == 182))                                   // move right
    {
        frame.y_min =
        frame.y_min -
        abs(frame.y_max - frame.y_min) / 10;
        
        frame.y_max =
        frame.y_max -
        abs(frame.y_max - frame.y_min) / 10;
    }
   
    if ((key == 81) ||
        (key == 52) ||
        (key == 180))                                   // move left
    {
        frame.y_min =
        frame.y_min +
        abs(frame.y_max - frame.y_min) / 10;
        
        frame.y_max =
        frame.y_max +
        abs(frame.y_max - frame.y_min) / 10;
    }
  
   if ((key == 171) ||                                  // zoom in
       (key == 61 ))
   {
       frame.x_min =
       frame.x_min +
       abs(frame.x_max - frame.x_min) / 10;
       
       frame.x_max =
       frame.x_max -
       abs(frame.x_max - frame.x_min) / 10;
       
       frame.y_min =
       frame.y_min +
       abs(frame.y_max - frame.y_min) / 10;
       
       frame.y_max =
       frame.y_max -
       abs(frame.y_max - frame.y_min) / 10;
       
   }
    
    if ((key == 173) ||                                 // zoom out
        (key == 45 ))
    {
        frame.x_min =
        frame.x_min -
        abs(frame.x_max - frame.x_min) / 10;
        
        frame.x_max =
        frame.x_max +
        abs(frame.x_max - frame.x_min) / 10;
        
        frame.y_min =
        frame.y_min -
        abs(frame.y_max - frame.y_min) / 10;
        
        frame.y_max =
        frame.y_max +
        abs(frame.y_max - frame.y_min) / 10;
    }
    
    if ((key == 109) ||                                 // MAN - Jul switch
        (key == 77 ))
    {
        frame.mandel = !frame.mandel;
    }

    if ((key == 111) ||                                 // MAN - Jul switch
        (key == 79 ))
    {
        frame.x_min   =   MINUSX;                               // fractal left border
        frame.x_max   =   PLUSX;                                // fractal right border
        frame.y_min   =   MINUSY;                               // fractal upper border
        frame.y_max   =   PLUSY;                                // fractal lower border
    }

    return frame;
}
//------------------------------------------------------------------------------------
// displays welcome screen

void splash()
{
    printf("-----------------------------------------\n");
    printf("MANDELBROT & JULIA \n\n");
    printf("Move up   = 8 or ^ , Move down  = 2 or V \n");
    printf("Move left = 4 or < , Move righr = 6 or > \n");
    printf("Zoom in   = +      , Zoom out   = -      \n");
    printf("Toggle Mandelbrot/Julia         = m      \n");
    printf("Original values                 = o      \n");
    printf("-----------------------------------------\n");
}
//------------------------------------------------------------------------------------
// create the window string

char * make_string(FRAME    frame)
{
    // VARIABLEN START ---------------------------------------------------------------
    
    static char title[100];                                         // for the frame title
    
    // VARIABLEN END  ----------------------------------------------------------------
    
    if (frame.mandel == true)
    {
        
        sprintf(title,                                          // create initial title
        "MANDELBROT X: %.2f min %.2f max , Y: %.2f min %.2f max",
                frame.x_min, frame.x_max,frame.y_min, frame.y_max);
        
    } else
    {
        sprintf(title,                                          // create initial title
        "JULIA      X: %.2f min %.2f max , Y: %.2f min %.2f max",
                frame.x_min, frame.x_max,frame.y_min, frame.y_max);
    }
    
    return title;
}
