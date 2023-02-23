#include "Flux.h"
#include "../engine/Color/Test.h"
#include "../engine/infrastructure/FLXDefines.h"

using namespace std;

int main()
{
	int inc = xsimd::batch<pixel_t, SIMD_ARCH>::size;
	Test::LetThisShit();
	int c = 0;
	LibRaw* d = new LibRaw();
	d->open_file("");
	auto s = tjAlloc(100);
	cout << ((char*)s)[0] << endl;
	cout << c << endl;
	cout << inc << endl;



	cout << __AVX2__ << std::endl;
	

	d->recycle();
	delete d;
	return 0;
}
