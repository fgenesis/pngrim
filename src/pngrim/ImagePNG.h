/****************************************************************************/
/* Copyright (c) 2009, Javor Kalojanov
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
/****************************************************************************/

#ifdef _MSC_VER
#pragma once
#endif

#ifndef IMAGEPNG_HPP_INCLUDED_9C30672E_ADD4_4B46_979D_DEA0709CACB0
#define IMAGEPNG_HPP_INCLUDED_9C30672E_ADD4_4B46_979D_DEA0709CACB0

#include <vector>


//An image class that can also be saved to a PNG
//Colors are represented as vec4f, with red mapped to .x, blue <-> .y, green <-> .z
//	.w is not used.
//Color components are in the range 0..1
class Image
{
	unsigned int m_width, m_height;
	std::vector<unsigned int> m_bits;

public:
	Image()
	{
		m_width = 0; m_height = 0;
	}

	Image(unsigned int _width, unsigned int _height);

	unsigned int width() const {return m_width;}
	unsigned int height() const {return m_height;}

	//An operator for accessing the image in the form img(x, y)
	//Example:
	//	Image img(800, 600);
	//	img(1, 2) = float3(1, 0, 0); //Set pixel x = 1, y = 2 to red (RGB: 1, 0, 0)
	//	float3 col = img(1, 2); //Get the color of pixel x = 1, y = 2
	unsigned int& operator() (unsigned int _x, unsigned int _y);

	inline const unsigned int *getData() const { return &m_bits[0]; }
	inline unsigned int *getData() { return &m_bits[0]; }

	//The same () operator as above, defined for constant objects, to be able to retrieve colors from them.
	//0xaabbggrr
	unsigned int operator() (unsigned int _x, unsigned int _y) const;

	void clear(unsigned int c);
	void free();

	//Writes the image to a file
	bool writePNG(const char* _fileName);

	//Read the image from a file
	bool readPNG(const char* _fileName);
};


#endif // IMAGEPNG_HPP_INCLUDED_9C30672E_ADD4_4B46_979D_DEA0709CACB0
