
/* This code is released into the public domain. */

#include <algorithm>
#include "ImagePNG.h"
#include "Matrix.h"


struct Pos
{
	Pos(unsigned xx, unsigned yy, unsigned n) : x(xx), y(yy), nb(n) {}
	unsigned x, y, nb;

	inline bool operator< (const Pos& p) const
	{
		return nb < p.nb;
	}
};

inline unsigned red  (unsigned c) { return (c      ) & 0xff; }
inline unsigned green(unsigned c) { return (c >> 8 ) & 0xff; }
inline unsigned blue (unsigned c) { return (c >> 16) & 0xff; }
inline unsigned alpha(unsigned c) { return (c >> 24) & 0xff; }

template<typename T> inline T vmin(T a, T b) { return a < b ? a : b; }
template<typename T> inline T vmax(T a, T b) { return a > b ? a : b; }

void pngrimAccurate(Image& img)
{
	const unsigned w = img.width();
	const unsigned h = img.height();
	Matrix<unsigned char> solid(w, h);
	std::vector<Pos> P, Q, R;

	for(unsigned y = 0; y < h; ++y)
		for(unsigned x = 0; x < w; ++x)
		{
			if(alpha(img(x, y)))
				solid(x, y) = 1;
			else
			{
				Pos p(x, y, 0);
				solid(x, y) = 0;
				for(int oy = -1; oy <= 1; ++oy)
					for(int ox = -1; ox <= 1; ++ox)
					{
						const unsigned xn = int(x) + ox;
						const unsigned yn = int(y) + oy;
						if(xn < w && yn < h && alpha(img(xn, yn)))
							++p.nb;
					}

					if(p.nb)
						P.push_back(p);
			}
		}

	while(P.size())
	{
		std::sort(P.begin(), P.end());
		P.swap(Q);

		while(Q.size())
		{
			unsigned r = 0;
			unsigned g = 0;
			unsigned b = 0;
			unsigned c = 0;
			Pos p = Q.back();
			Q.pop_back();
			if(solid(p.x, p.y))
				continue;

			for(int oy = -1; oy <= 1; ++oy)
			{
				const unsigned y = int(p.y) + oy;
				if(y < h)
				{
					for(int ox = -1; ox <= 1; ++ox)
					{
						if(oy || ox)
						{
							const unsigned x = int(p.x) + ox;
							if(x < w)
							{
								if(solid(x, y))
								{
									const unsigned pix = img(x, y);
									r +=   red(pix);
									g += green(pix);
									b +=  blue(pix);
									++c;
								}
								else
									R.push_back(Pos(x, y, 0));
							}
						}
					}
				}
			}

			solid(p.x, p.y) = 1;
			img(p.x, p.y) =
				  ((r / c)      )
				| ((g / c) << 8 )
				| ((b / c) << 16);
		}

		while(R.size())
		{
			Pos p = R.back();
			R.pop_back();
			if(solid(p.x, p.y))
				continue;

			for(int oy = -1; oy <= 1; ++oy)
				for(int ox = -1; ox <= 1; ++ox)
				{
					const unsigned xn = int(p.x) + ox;
					const unsigned yn = int(p.y) + oy;
					if(xn < w && yn < h && solid(xn, yn))
						++p.nb;
				}

				P.push_back(p);
		}
	}
}

void pngrimFast(Image& img)
{
	const unsigned w = img.width();
	const unsigned h = img.height();
	const unsigned inf = 0x7fffffff;
	Matrix<unsigned> dist(w, h);

	unsigned numtrans = 0;
	for(unsigned y = 0; y < h; ++y)
		for(unsigned x = 0; x < w; ++x)
		{
			const unsigned isTrans = !alpha(img(x, y));
			dist(x, y) = isTrans;
			numtrans += isTrans;
		}
	std::vector<Pos> todo;
	todo.reserve(numtrans);

	// distance transform, X direction
	for(unsigned y = 0; y < h; ++y)
	{
		unsigned * const row = &dist(0, y);
		unsigned d = inf;
		for(unsigned x = 0; x < w; ++x)
			if(row[x])
				row[x] = ++d;
			else
				d = 0;
		d = inf;
		for(unsigned x = w-1; x; --x)
			if(const unsigned val = row[x])
				row[x] = vmin(++d, val);
			else
				d = 0;
	}

	// distance transform, Y direction
	for(unsigned x = 0; x < w; ++x)
	{
		unsigned d = dist(x, 0);
		for(unsigned y = 0; y < h; ++y)
		{
			const unsigned val = dist(x, y);
			if(val >= d)
				dist(x, y) = vmin(++d, val);
			else
				d = val;
		}
		d = dist(x, h-1);
		for(unsigned y = h-1; y; --y)
		{
			const unsigned val = dist(x, y);
			if(val >= d)
				dist(x, y) = vmin(++d, val);
			else
				d = val;
		}
	}

	// Use distance as heuristic for pixel processing order
	for(unsigned y = 0; y < h; ++y)
		for(unsigned x = 0; x < w; ++x)
			if(unsigned d = dist(x, y))
				todo.push_back(Pos(x, y, d));
	std::sort(todo.begin(), todo.end());

	for(size_t i = 0; i < todo.size(); ++i)
	{
		Pos p = todo[i];
		unsigned r = 0, g = 0, b = 0, c = 0;

		for(int oy = -1; oy <= 1; ++oy)
		{
			const unsigned y = int(p.y) + oy;
			if(y < h)
			{
				for(int ox = -1; ox <= 1; ++ox)
				{
					const unsigned x = int(p.x) + ox;
					if(x < w)
					{
						if((ox || oy) && !dist(x, y))
						{
							const unsigned pix = img(x, y);
							r +=   red(pix);
							g += green(pix);
							b +=  blue(pix);
							++c;
						}
					}
				}
			}
		}

		dist(p.x, p.y) = 0;
		img(p.x, p.y) =
			  ((r / c)      )
			| ((g / c) << 8 )
			| ((b / c) << 16);
	}
}

