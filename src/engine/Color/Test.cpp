#include "Test.h"

void Test::LetThisShit()
{
	int inc = xsimd::batch<float, xsimd::avx2>::size;
	int c = 0;
	LibRaw* d = new LibRaw();
	d->open_file("");
	auto s = tjAlloc(100);
	std::cout << ((char*)s)[0] << std::endl;
	std::cout << c << std::endl;
	std::cout << inc << std::endl;
	d->recycle();
	delete d;
}
