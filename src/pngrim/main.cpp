
/* This code is released into the public domain. */

#include <string>
#include <stdio.h>
#include <algorithm>
#include "ImagePNG.h"
#include "Matrix.h"

struct Pos
{
	Pos(unsigned int xx, unsigned int yy, unsigned int n) : x(xx), y(yy), nb(n) {}
	unsigned int x, y, nb;

	inline bool operator< (const Pos& p) const
	{
		return nb < p.nb;
	}
};

inline unsigned int red  (unsigned int c) { return (c      ) & 0xff; }
inline unsigned int green(unsigned int c) { return (c >> 8 ) & 0xff; }
inline unsigned int blue (unsigned int c) { return (c >> 16) & 0xff; }
inline unsigned int alpha(unsigned int c) { return (c >> 24) & 0xff; }

void processImage(Image& img)
{
	const unsigned int w = img.width();
	const unsigned int h = img.height();
	Matrix<unsigned char> solid(w, h);
	std::vector<Pos> P, Q, R;

	for(unsigned int y = 0; y < h; ++y)
		for(unsigned int x = 0; x < w; ++x)
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
						const unsigned int xn = int(x) + ox;
						const unsigned int yn = int(y) + oy;
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
			unsigned int r = 0;
			unsigned int g = 0;
			unsigned int b = 0;
			int c = 0;
			Pos p = Q.back();
			Q.pop_back();
			if(solid(p.x, p.y))
				continue;

			unsigned int a = alpha(img(p.x, p.y));

			for(int oy = -1; oy <= 1; ++oy)
			{
				const unsigned int y = int(p.y) + oy;
				if(y < h)
				{
					for(int ox = -1; ox <= 1; ++ox)
					{
						const unsigned int x = int(p.x) + ox;
						if(x < w)
						{
							if(solid(x, y))
							{
								const unsigned int pix = img(x, y);
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
			solid(p.x, p.y) = 1;
			float fc = float(c);
			img(p.x, p.y) =
			  ((unsigned int)(r / fc)      )
			| ((unsigned int)(g / fc) << 8 )
			| ((unsigned int)(b / fc) << 16)
			| (a << 24);
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
					const unsigned int xn = int(p.x) + ox;
					const unsigned int yn = int(p.y) + oy;
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

