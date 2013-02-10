#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <string.h>

template <typename T> class Matrix
{
	typedef unsigned int uint;

private:
	T *_mem;
	size_t _shift;
	size_t _w;
	size_t _h;
	size_t _elems;

public:
	Matrix() : _mem(NULL), _shift(0), _w(0), _h(0) {}
	Matrix(uint w, uint h) : _mem(NULL) { resize(w, h); }
	~Matrix() { delete [] _mem; }

	inline size_t width() const { return _w; }
	inline size_t height() const { return _h; }

	void clear()
	{
		_w = _h = _shift = 0;
		if(_mem)
		{
			delete [] _mem;
			_mem = NULL;
		}
	}

	void resize(size_t w, size_t h)
	{
		if(!(w && h))
		{
			clear();
			return;
		}
		_w = w;
		_h = h;
		size_t newsize = 1;
		size_t sh = 0;

		while(newsize < w)
		{
			newsize <<= 1;
			++sh;
		}

		_shift = sh;

		if(_mem)
			delete [] _mem;

		_elems = newsize * _h;
		_mem = new T[_elems];
	}

	inline T& operator()(size_t x, size_t y)
	{
		return _mem[(y << _shift) | x];
	}

	inline const T& operator()(size_t x, size_t y) const
	{
		return _mem[(y << _shift) | x];
	}

};


#endif
