/* SGE_Pixel.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * ===================================================================
 *
 * The SGE was developed by Joseph21 and is heavily inspired bij the Pixel Game Engine (PGE) by Javidx9
 * (see: https://github.com/OneLoneCoder/olcPixelGameEngine). It's interface is deliberately kept very
 * close to that of the PGE, so that programs can be ported from the one to the other quite easily.
 *
 * License
 * -------
 * This code is completely free to use, change, rewrite or get inspiration from. At the same time, there's
 * no warranty that this code is free of bugs. If you use (any part of) this code, you accept each and any
 * risk or consequence thereof.
 *
 * Although there is no obligation to mention or shout out to the creator, I wouldn't mind if you did :)
 *
 * Have fun with it!
 *
 * Joseph21
 * december 4, 2022
 */


#include "Pixel.h"

//                           +------------------+                            //
// --------------------------+ GLOBAL VARIABLES +--------------------------- //
//                           +------------------+                            //

// These global variables contain (after initialization) the proper mask, shift and loss values to decode
// or encode the RGBA components from a pixel that is modeled as a uint32_t. These values are set at game
// start up to match the pixelformat that is used for SDL_Surfaces and SDL_Textures throughout the engine.
// All sprites that are created or loaded are converted to the same PixelFormat.

// global masks for getting RGBA values out of an uint32_t encoded pixel
uint32_t glb_rmask,
         glb_gmask,
         glb_bmask,
         glb_amask;
// global shift values for getting RGBA values out of an uint32_t encoded pixel
short    glb_rshift,
         glb_gshift,
         glb_bshift,
         glb_ashift;
// global loss values for getting RGBA values out of an uint32_t encoded pixel
short    glb_rloss,
         glb_gloss,
         glb_bloss,
         glb_aloss;

//                              +------------+                               //
// -----------------------------+ FUNCTIONS  +------------------------------ //
//                              +------------+                               //

// primitive pixel getters
uint8_t unpackR( uint32_t encodedCol ) { return ((encodedCol & glb_rmask) >> glb_rshift) << glb_rloss; }
uint8_t unpackG( uint32_t encodedCol ) { return ((encodedCol & glb_gmask) >> glb_gshift) << glb_gloss; }
uint8_t unpackB( uint32_t encodedCol ) { return ((encodedCol & glb_bmask) >> glb_bshift) << glb_bloss; }
uint8_t unpackA( uint32_t encodedCol ) { return ((encodedCol & glb_amask) >> glb_ashift) << glb_aloss; }

// ==============================/ Class Pixel /==============================

//                           +-----------------+                             //
// --------------------------+ CONSTRUCTOR ETC +---------------------------- //
//                           +-----------------+                             //



// default constructor
Pixel::Pixel() {
    r = DEFAULT_R_NUM;
    g = DEFAULT_G_NUM;
    b = DEFAULT_B_NUM;
    a = DEFAULT_A_NUM;
}

// Pixel constructor - set RGBA values directly
Pixel::Pixel( uint8_t uR, uint8_t uG, uint8_t uB, uint8_t uA ) {
    r = uR;
    g = uG;
    b = uB;
    a = uA;
}

// Pixel constructor - set RGBA values directly - for compatibility with PGE
Pixel::Pixel( int nR, int nG, int nB, int nA ) {
    r = (uint8_t)nR;
    g = (uint8_t)nG;
    b = (uint8_t)nB;
    a = (uint8_t)nA;
}

// Pixel constructor - set RGBA values using normalized values (arguments must be in [0.0f, 1.0f])
Pixel::Pixel( float fR, float fG, float fB, float fA ) {
    r = (uint8_t)(fR * 255.0f);
    g = (uint8_t)(fG * 255.0f);
    b = (uint8_t)(fB * 255.0f);
    a = (uint8_t)(fA * 255.0f);
}

// Pixel constructor - decode the var encodedCol into R, G, B and A values
// and create a Pixel object from it.
Pixel::Pixel( uint32_t encodedCol ) {
    r = ((encodedCol & glb_rmask) >> glb_rshift) << glb_rloss;
    g = ((encodedCol & glb_gmask) >> glb_gshift) << glb_gloss;
    b = ((encodedCol & glb_bmask) >> glb_bshift) << glb_bloss;
    a = ((encodedCol & glb_amask) >> glb_ashift) << glb_aloss;
}

// default destructor
Pixel::~Pixel() {}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// set each of the RGB or A values seperately on existing Pixel
void Pixel::setR( uint8_t uR ) { r = uR; }
void Pixel::setG( uint8_t uG ) { g = uG; }
void Pixel::setB( uint8_t uB ) { b = uB; }
void Pixel::setA( uint8_t uA ) { a = uA; }

// get each of the RGB or A values seperately from existing Pixel
uint8_t Pixel::getR() { return r; }
uint8_t Pixel::getG() { return g; }
uint8_t Pixel::getB() { return b; }
uint8_t Pixel::getA() { return a; }

// return the colour value as a uint32_t, where the RGBA values are encoded into
uint32_t Pixel::Encode() {
    uint32_t col = ((r >> glb_rloss) << glb_rshift) |
                   ((g >> glb_gloss) << glb_gshift) |
                   ((b >> glb_bloss) << glb_bshift) |
                   ((a >> glb_aloss) << glb_ashift);
    return col;
}

// return the inverse colour of this pixel
//Pixel Pixel::inv() const {
//    return Pixel( (uint8_t)Clamp( uint8_t(255 - this->r), uint8_t(0), uint8_t(255) ),
//                  (uint8_t)Clamp( uint8_t(255 - this->g), uint8_t(0), uint8_t(255) ),
//                  (uint8_t)Clamp( uint8_t(255 - this->b), uint8_t(0), uint8_t(255) ) );
//}

// return the linear interpolation of p1 and p2 w.r.t. t
Pixel Pixel::PixelLerp( const Pixel& p1, const Pixel& p2, float t ) {
    return Pixel(
        (uint8_t)(p1.r * t + p2.r * (1.0f - t)),
        (uint8_t)(p1.g * t + p2.g * (1.0f - t)),
        (uint8_t)(p1.b * t + p2.b * (1.0f - t))
    );
}

// clamp the RGBA values of current pixel in [0, 255]
//void Pixel::PixelClamp() {
//    r = Clamp( (uint8_t)r, (uint8_t)0, (uint8_t)255 );
//    g = Clamp( (uint8_t)g, (uint8_t)0, (uint8_t)255 );
//    b = Clamp( (uint8_t)b, (uint8_t)0, (uint8_t)255 );
//    a = Clamp( (uint8_t)a, (uint8_t)0, (uint8_t)255 );
//}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
