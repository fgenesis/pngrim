
/* This code is released into the public domain. */

#include <string>
#include <stdio.h>
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

void processImage(Image& img)
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

			const unsigned a = alpha(img(p.x, p.y));
			solid(p.x, p.y) = 1;
			img(p.x, p.y) =
			  ((r / c)      )
			| ((g / c) << 8 )
			| ((b / c) << 16)
			| ((a    ) << 24);
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


void processFile(const char *fn)
{
	Image img;
	if(!img.readPNG(fn))
	{
		printf("File not processed: %s\n", fn);
		return;
	}

	printf("Processing %s ... ", fn);
	fflush(stdout);
	processImage(img);
	printf("saving ... ");
	fflush(stdout);

	if(img.writePNG(fn))
		printf("OK\n");
	else
		printf("Failed to write!\n");
}


int main(int argc, char **argv)
{
	if(argc <= 1)
	{
		printf("Usage: ./pngrim file1.png [fileX.png ...]\n");
		printf("Warning: Modifies files in place!\n");
		return 2;
	}

	for(int i = 1; i < argc; ++i)
		processFile(argv[i]);

	return 0;
}

