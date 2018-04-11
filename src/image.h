/*** image.h  Javi Agenjo (javi.agenjo@gmail.com) 2013
	This file defines the class Image that allows to manipulate images.
	It defines all the need operators for Color and Image.
	It has a TGA loader and saver.
***/

#ifndef IMAGE_H
#define IMAGE_H

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "framework.h"

//remove unsafe warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

//Class Image: to store a matrix of pixels
class Image
{
	//a general struct to store all the information about a TGA file
	typedef struct sTGAInfo 
	{
		unsigned int width;
		unsigned int height;
		unsigned int bpp; //bits per pixel
		unsigned char* data; //bytes with the pixel information
	} TGAInfo;

public:
	unsigned int width;
	unsigned int height;
	Color* pixels;

	// CONSTRUCTORS 
	Image();
	Image(unsigned int width, unsigned int height);
	Image(const Image& c);
	Image& operator = (const Image& c); //assign operator

	//destructor
	~Image();

	//get the pixel at position x,y
	Color getPixel(unsigned int x, unsigned int y) const { return pixels[ y * width + x ]; }
	Color& getPixelRef(unsigned int x, unsigned int y)	{ return pixels[ y * width + x ]; }
	Color getPixelSafe(unsigned int x, unsigned int y) const {	
		x = clamp((unsigned int)x, 0, width-1); 
		y = clamp((unsigned int)y, 0, height-1); 
		return pixels[ y * width + x ]; 
	}

	//set the pixel at position x,y with value C
	inline void setPixel(unsigned int x, unsigned int y, const Color& c) const { pixels[y * width + x] = c; }
	inline void setPixelSafe(unsigned int x, unsigned int y, const Color& c) const { x = clamp(x, 0, width - 1); y = clamp(y, 0, height - 1); setPixel(x, y, c); }
	inline void blendPixel(unsigned int x, unsigned int y, const Color& c) const { Color& d = pixels[y * width + x]; d = blendColors(c, d); } //using the alpha to blend colors

	void resize(unsigned int width, unsigned int height); //resizes the canvas but keeping the data in the corner
	void scale(unsigned int width, unsigned int height); //stretches the image to fit the new size 
	void crop(unsigned int x, unsigned int y, unsigned int width, unsigned int height); //crops the image to a given area
	Image getArea(unsigned int x, unsigned int y, unsigned int width, unsigned int height); //makes a copy of an area and returns it as an image

	//draw
	void drawImage(const Image& img, int x, int y);
	void drawImage(const Image& img, int x, int y, int imgx, int imgy, int imgw, int imgh); //draws only a part of the image
	void drawImage(const Image& img, int x, int y, Rect rect) { drawImage(img, x, y, rect.x, rect.y, rect.w, rect.h); }//draws only a part of the image
	void drawLine( int x0, int y0, int x1, int y1, const Color& c);
	void drawText( std::string text, int x, int y, const Image& bitmapfont, int font_w = 7, int font_h = 9, int first_char = 32);

	void applyAlpha(const Color& alpha_color); //every pixel with the given color will be set to transparent

	void flipY(); //flip the image top-down
	void flipX(); //flip the image left-right

	//fill the image with the color C
	void fill(const Color& c) { for(unsigned int pos = 0; pos < width*height; ++pos) pixels[pos] = c; }

	void quantize(int levels); //reduce color palette quantizing every channel
	Rect getRect( int index, int w, int h) const;

	//save or load images from the hard drive
	bool loadTGA(const char* filename);
	bool saveTGA(const char* filename);

	//manager to load several images
	static Image* Get( std::string name );
	static std::map<std::string, Image*> s_loaded_images;

	//used to easy code
	#ifndef IGNORE_LAMBDAS

	//applies an algorithm to every pixel in an image
	// you can use lambda sintax:   img.forEachPixel( [](Color c) { return c*2; });
	// or callback sintax:   img.forEachPixel( mycallback ); //the callback has to be Color mycallback(Color c) { ... }
	template <typename F>
	Image& forEachPixel( F callback )
	{
		for(unsigned int pos = 0; pos < width*height; ++pos)
			pixels[pos] = callback(pixels[pos]);
		return *this;
	}

	#endif

};

inline Color lerp(const Color& a, const Color& b, float f);
inline Color blendColors(const Color& a, const Color& b);
inline Color operator * (const Color& c, float v);
inline Color operator * (float v, const Color& c);

#endif