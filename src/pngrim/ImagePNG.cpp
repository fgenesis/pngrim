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

#include "ImagePNG.h"

#include <string>
#include <fstream>
#include <assert.h>

#include <png.h>

//////////////////////////////////////////////////////////////////////////

Image::Image(unsigned int _width, unsigned int _height)
: m_width(_width), m_height(_height)
{
	m_bits.resize(_width * _height);
}

unsigned int& Image::operator() (unsigned int _x, unsigned int _y)
{
	_ASSERT(_x < m_width && _y < m_height);
	return m_bits[_y * m_width + _x];
}

unsigned int Image::operator() (unsigned int _x, unsigned int _y) const
{
	_ASSERT(_x < m_width && _y < m_height);
	return m_bits[_y * m_width + _x];
}

void Image::clear(unsigned int _color)
{
	for(unsigned int i = 0; i < m_width * m_height; i++)
		m_bits[i] = _color;
}

void Image::free()
{
	// swap trick to really reclaim memory
	m_bits.clear();
	std::vector<unsigned int>().swap(m_bits);
	m_width = m_height = 0;
}

//////////////////////////////////////////////////////////////////////////

bool Image::writePNG(const char* aFileName)
{
	std::string _fileName(aFileName);

	std::vector<png_byte> byteData (m_bits.size() * 4);
	std::vector<png_byte>::iterator ptr = byteData.begin();
	for(std::vector<unsigned int>::const_iterator it = m_bits.begin(); it != m_bits.end(); ++it)
	{
		const unsigned int v = *it;
		*ptr++ = v & 0xff; // R
		*ptr++ = (v >> 8) & 0xff; // G
		*ptr++ = (v >> 16) & 0xff; // B
		*ptr++ = (v >> 24) & 0xff; // A
	}

	std::vector<png_byte*> rowData(m_height);
	for(unsigned int i = 0; i < m_height; i++)
		rowData[i] = i * m_width * 4 + &byteData.front();

	/* create file */
	FILE *fp = fopen(_fileName.c_str(), "wb");
	if (!fp) {
		printf("[write_png_file] File %s could not be opened for writing\n", _fileName.c_str());
		return false;
	}

	bool success = true;

	/* initialize stuff */
	png_structp png_ptr;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		printf("[write_png_file] png_create_write_struct failed\n");
		success = false;
		goto end;
	}

	png_infop info_ptr;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		printf("[write_png_file] png_create_info_struct failed\n");
		success = false;
		goto end;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("[write_png_file] Error during init_io\n");
		success = false;
		goto end;
	}


	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("[write_png_file] Error during writing header\n");
		success = false;
		goto end;
	}

	png_set_IHDR(png_ptr, info_ptr, m_width, m_height,
			8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("[write_png_file] Error during writing bytes\n");
		success = false;
		goto end;
	}

	png_write_image(png_ptr, (png_byte**)&rowData.front());


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("[write_png_file] Error during end of write\n");
		success = false;
		goto end;
	}

	png_write_end(png_ptr, NULL);

end:
	if(fp)
		fclose(fp);
	return success;
}


bool Image::readPNG(const char* aFileName)
{
	std::string _fileName(aFileName);

	png_byte header[8];	// 8 is the maximum size that can be checked
	std::vector<unsigned int>::iterator it;
	std::vector<png_byte> byteData;
	std::vector<png_byte*> rowData;
	png_infop info_ptr = 0;
	png_structp png_ptr = 0;

	/* open file and test for it being a png */
	FILE *fp = fopen(_fileName.c_str(), "rb");
	if (!fp)
	{
		printf("[read_png_file] File %s could not be opened for reading\n", _fileName.c_str());
		return false;
	}
	bool success = true;
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		printf("[read_png_file] File %s is not recognized as a PNG file\n", _fileName.c_str());
		success = false;
		goto end;
	}


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		printf("[read_png_file] png_create_read_struct failed\n");
		success = false;
		goto end;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("[read_png_file] png_create_info_struct failed\n");
		success = false;
		goto end;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during init_io\n");
		success = false;
		goto end;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	m_width = png_get_image_width(png_ptr, info_ptr);
	m_height = png_get_image_height(png_ptr, info_ptr);
	/*color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;*/

	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	byteData.resize(png_get_rowbytes(png_ptr, info_ptr) * m_height);
	rowData.resize(m_height);
	for(unsigned int i = 0; i < m_height; i++)
		rowData[i] = i * png_get_rowbytes(png_ptr, info_ptr) + &byteData.front();

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during read_image\n");
		success = false;
		goto end;
	}

	png_read_image(png_ptr, &rowData.front());

	m_bits.resize(m_width * m_height);
	it = m_bits.begin();
	const unsigned char channels = png_get_channels(png_ptr, info_ptr);
	for(size_t y = 0; y < m_height; y++)
	{
		png_byte *b = rowData[y];
		for(size_t x = 0; x < m_width; x++)
		{
			unsigned int v = 0;
			v |= *b++; // R
			v |= *b++ << 8; // G
			v |= *b++ << 16; // B
			if(channels == 4)
				v |= *b++ << 24; // A

			*it++ = v;
		}
	}

end:
	if(fp)
		fclose(fp);
	return success;

}
