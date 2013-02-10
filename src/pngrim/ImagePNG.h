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

#ifndef IMAGEPNG_H
#define IMAGEPNG_H

#include <vector>

class Image
{
	unsigned int m_width, m_height;
	std::vector<unsigned int> m_bits;

public:
	Image() : m_width(0), m_height(0) {}
	Image(unsigned int _width, unsigned int _height);

	unsigned int width() const {return m_width;}
	unsigned int height() const {return m_height;}

	// 0xAABBGGRR
	inline unsigned int& operator() (unsigned int _x, unsigned int _y)
	{
		return m_bits[_y * m_width + _x];
	}
	inline unsigned int operator() (unsigned int _x, unsigned int _y) const
	{
		return m_bits[_y * m_width + _x];
	}

	bool writePNG(const char* _fileName);
	bool readPNG(const char* _fileName);
};


#endif
