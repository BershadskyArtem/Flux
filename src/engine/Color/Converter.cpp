#include "Converter.h"


LUT Converter::cbrtLUT = LUT();
LUT Converter::LUTsRGBToRGBFloat = LUT();
LUT Converter::LUTRGBtosRGBFloat = LUT();

std::array<uint8_t, 256> Converter::RGB2sRGBLUT = {
0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,11,11,11,12,12,13,13,13,14,14,15,15,16,16,16,17,17,18,18,19,19,20,20,21,22,22,23,23,24,24,25,26,26,27,27,28,29,29,30,31,31,32,33,33,34,35,36,36,37,38,38,39,40,41,42,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,55,56,57,58,59,60,61,62,63,64,65,66,67,68,70,71,72,73,74,75,76,77,78,80,81,82,83,84,85,87,88,89,90,92,93,94,95,97,98,99,101,102,103,105,106,107,109,110,112,113,114,116,117,119,120,122,123,125,126,128,129,131,132,134,135,137,139,140,142,144,145,147,148,150,152,153,155,157,159,160,162,164,166,167,169,171,173,175,176,178,180,182,184,186,188,190,192,193,195,197,199,201,203,205,207,209,211,213,215,218,220,222,224,226,228,230,232,235,237,239,241,243,245,248,250,252,255
};

void Converter::sRGB2RGB(pixel_t r, pixel_t g, pixel_t b, pixel_t& rOut, pixel_t& gOut, pixel_t& bOut)
{
	rOut = Converter::LUTsRGBToRGBFloat.Get01(r);
	gOut = Converter::LUTsRGBToRGBFloat.Get01(g);
	bOut = Converter::LUTsRGBToRGBFloat.Get01(b);
}

void Converter::sRGB2RGB(pixel_t* r, pixel_t* g, pixel_t* b, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, int width, int height)
{

	int size = width * height;
	int inc = vfloat::size;

	vfloat zeroV = 0.f;
	vfloat oneV = 1.f;

#pragma omp parallel for
	for (int i = 0; i < size; i += inc)
	{
		auto rV = vfloat::load_aligned(&r[i]);
		auto gV = vfloat::load_aligned(&g[i]);
		auto bV = vfloat::load_aligned(&b[i]);

		rV = xsimd::clip(rV, zeroV, oneV);
		gV = xsimd::clip(gV, zeroV, oneV);
		bV = xsimd::clip(bV, zeroV, oneV);

		rV = Converter::LUTsRGBToRGBFloat.Get01(rV);
		gV = Converter::LUTsRGBToRGBFloat.Get01(gV);
		bV = Converter::LUTsRGBToRGBFloat.Get01(bV);

		rV.store_aligned(&rOut[i]);
		gV.store_aligned(&gOut[i]);
		bV.store_aligned(&bOut[i]);
	}
}

void Converter::sRGB2RGB(pixel_t* rgbIn, pixel_t* rgbOut, int width, int height)
{
	int size = width * height;
	int inc = vfloat::size;

	vfloat zeroV = 0.f;
	vfloat oneV = 1.f;

#pragma omp parallel for
	for (int i = 0; i < size - inc; i += inc)
	{
		auto rV = vfloat::load_aligned(&rgbIn[i]);
		rV = xsimd::clip(rV, zeroV, oneV);
		rV = Converter::LUTsRGBToRGBFloat.Get01(rV);
		rV.store_aligned(&rgbOut[i]);
	}

	for (int i = size - inc; i < size; i++) {
		auto rV = rgbIn[i];
		rV = std::clamp(rV, 0.f, 1.f);
		rV = Converter::LUTsRGBToRGBFloat.Get01(rV);
		rgbOut[i] = rV;
	}
}

void Converter::sRGB2RGB(byte_t r, byte_t g, byte_t b, byte_t& rOut, byte_t& gOut, byte_t& bOut)
{
	rOut = RGB2sRGBLUT[r];
	gOut = RGB2sRGBLUT[g];
	bOut = RGB2sRGBLUT[b];
}

void Converter::OKLab2RGB(pixel_t lIn, pixel_t aIn, pixel_t bIn, pixel_t& rOut, pixel_t& gOut, pixel_t& bOut)
{
	pixel_t l = lIn + 0.3963377774 * aIn + 0.2158037573 * bIn;
	pixel_t m = lIn - 0.1055613458 * aIn - 0.0638541728 * bIn;
	pixel_t s = lIn - 0.0894841775 * aIn - 1.2914855480 * bIn;

	l = l * l * l;
	m = m * m * m;
	s = s * s * s;

	pixel_t r = 4.0767245293 * l - 3.3072168827 * m + 0.2307590544 * s;
	pixel_t g = -1.2681437731 * l + 2.6093323231 * m - 0.3411344290 * s;
	pixel_t b = -0.0041119885 * l - 0.7034763098 * m + 1.7068625689 * s;

	rOut = r;
	gOut = g;
	bOut = b;
}

void Converter::OKLab2RGB(pixel_t* l, pixel_t* a, pixel_t* b, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, int width, int height)
{
	int size = width * height;
	int inc = vfloat::size;

	const pixel_t s12i = 0.3963377774f;
	const pixel_t s13i = 0.2158037573f;

	const pixel_t s22i = -0.1055613458f;
	const pixel_t s23i = -0.0638541728f;
	 
	const pixel_t s32i = -0.0894841775f;
	const pixel_t s33i = -1.2914855480f;
	 
	const pixel_t s11o = 4.0767416621f;
	const pixel_t s12o = -3.3077115913f;
	const pixel_t s13o = 0.2309699292f;
	 
	const pixel_t s21o = -1.2684380046f;
	const pixel_t s22o = 2.6097574011f;
	const pixel_t s23o = -0.3413193965f;
	 
	const pixel_t s31o = -0.0041960863f;
	const pixel_t s32o = -0.7034186147f;
	const pixel_t s33o = 1.7076147010f;


	//First step
	vfloat zeroV = 0.0f;
	vfloat oneV = 1.0f;

	vfloat v12i = 0.3963377774f;
	vfloat v13i = 0.2158037573f;
	
	vfloat v22i = -0.1055613458f;
	vfloat v23i = -0.0638541728f;
	
	vfloat v32i = -0.0894841775f;
	vfloat v33i = -1.2914855480f;

	//Second step

	vfloat v11o = 4.0767416621f;
	vfloat v12o = -3.3077115913f;
	vfloat v13o = 0.2309699292f;
	
	vfloat v21o = -1.2684380046f;
	vfloat v22o = 2.6097574011f;
	vfloat v23o = -0.3413193965f;
	
	vfloat v31o = -0.0041960863f;
	vfloat v32o = -0.7034186147f;
	vfloat v33o = 1.7076147010f;

//#pragma omp parallel for
	//for (int i = 0; i < size; i += inc)
	//{
	//	auto rV = vfloat::load_aligned(&l[i]);
	//	auto gV = vfloat::load_aligned(&a[i]);
	//	auto bV = vfloat::load_aligned(&b[i]);
	//
	//	auto l_ = rV + gV * v12i + bV * v13i;
	//	auto a_ = rV + gV * v22i + bV * v23i;
	//	auto b_ = rV + gV * v32i + bV * v33i;
	//
	//	l_ = l_ * l_ * l_;
	//	a_ = a_ * a_ * a_;
	//	b_ = b_ * b_ * b_;
	//
	//	auto rOutV = l_ * v11o + a_ * v12o + b_ * v13o;
	//	auto gOutV = l_ * v21o + a_ * v22o + b_ * v23o;
	//	auto bOutV = l_ * v31o + a_ * v32o + b_ * v33o;
	//
	//	rOutV = xsimd::clip(rOutV, zeroV, oneV);
	//	gOutV = xsimd::clip(gOutV, zeroV, oneV);
	//	bOutV = xsimd::clip(bOutV, zeroV, oneV);
	//
	//	rOutV.store_aligned(&rOut[i]);
	//	gOutV.store_aligned(&gOut[i]);
	//	bOutV.store_aligned(&bOut[i]);
	//}

	for (int i = 0; i < size; i++)
	{
		auto rV = l[i];
		auto gV = a[i];
		auto bV = b[i];
	
		auto l_ = rV + gV * s12i + bV * s13i;
		auto a_ = rV + gV * s22i + bV * s23i;
		auto b_ = rV + gV * s32i + bV * s33i;
	
		l_ = l_ * l_ * l_;
		a_ = a_ * a_ * a_;
		b_ = b_ * b_ * b_;
	
		auto rOutV = l_ * s11o + a_ * s12o + b_ * s13o;
		auto gOutV = l_ * s21o + a_ * s22o + b_ * s23o;
		auto bOutV = l_ * s31o + a_ * s32o + b_ * s33o;
	
		rOutV = std::clamp(rOutV, 0.0f, 1.0f);
		gOutV = std::clamp(gOutV, 0.0f, 1.0f);
		bOutV = std::clamp(bOutV, 0.0f, 1.0f);
	
		rOut[i] = rOutV;
		gOut[i] = gOutV;
		bOut[i] = bOutV;
	}
}

void Converter::OkLab2OkLCh(pixel_t* l, pixel_t* a, pixel_t* b, pixel_t* lOut, pixel_t* cOut, pixel_t* hOut, int width, int height)
{

	int size = width * height;
	int inc = vfloat::size;

	vfloat epsilon = 1e-10f;

#pragma omp parallel for
	for (int i = 0; i < size; i += inc)
	{
		auto lV = vfloat::load_aligned(&l[i]);
		auto aV = vfloat::load_aligned(&a[i]);
		auto bV = vfloat::load_aligned(&b[i]);

		auto cV = xsimd::sqrt(aV * aV + bV * bV);
		auto hV = xsimd::atan2(bV + epsilon, aV);

		lV.store_aligned(&lOut[i]);
		cV.store_aligned(&cOut[i]);
		hV.store_aligned(&hOut[i]);
	}

}

void Converter::OkLCh2OkLab(pixel_t* l, pixel_t* c, pixel_t* h, pixel_t* lOut, pixel_t* aOut, pixel_t* bOut, int width, int height)
{
	int size = width * height;
	int inc = vfloat::size;

#pragma omp parallel for
	for (int i = 0; i < size; i += inc)
	{
		auto lV = vfloat::load_aligned(&l[i]);
		auto cV = vfloat::load_aligned(&c[i]);
		auto hV = vfloat::load_aligned(&h[i]);

		auto aV = cV * xsimd::cos(hV);
		auto bV = cV * xsimd::sin(hV);

		lV.store_aligned(&lOut[i]);
		aV.store_aligned(&aOut[i]);
		bV.store_aligned(&bOut[i]);
	}
}

void Converter::OkLab2OkLCh(pixel_t l, pixel_t a, pixel_t b, pixel_t& lOut, pixel_t& cOut, pixel_t& hOut)
{
	auto c = std::sqrtf(a * a + b * b);
	auto h = 0.f;

	if (b > 0.01f) {
		h = std::atan2f(b, a);
	}

	lOut = l;
	cOut = c;
	hOut = h;
}

void Converter::OkLCh2OkLab(pixel_t l, pixel_t c, pixel_t h, pixel_t& lOut, pixel_t& aOut, pixel_t& bOut)
{
	auto aP = c * std::cosf(h);
	auto bP = c * std::sinf(h);
	
	lOut = l;
	aOut = aP;
	bOut = bP;
}

pixel_t Converter::RGB2sRGB(pixel_t in)
{
	return Converter::LUTRGBtosRGBFloat.Get01(in);
}

void Converter::Init()
{
	Converter::cbrtLUT.Init(65535);
	Converter::LUTsRGBToRGBFloat.Init(65535);
	Converter::LUTRGBtosRGBFloat.Init(65535);

	for (int i = 0; i < 65535; i++)
	{
		float v = cbrtf(i / 65535.f);
		Converter::cbrtLUT.SetValue(v, i);
	}

	for (int i = 0; i < 65535; i++)
	{
		float v = i / 65535.f;
		if (v <= 0.0031308) {
			Converter::LUTRGBtosRGBFloat.SetValue(v * 12.92f, i);
		}
		else {
			Converter::LUTRGBtosRGBFloat.SetValue(1.055 * std::pow(v, 1.0 / 2.4) - 0.055, i);
		}
	}
	for (int i = 0; i < 65535; i++)
	{
		float v = i / 65535.f;
		float vR = 0.f;
		if (v <= 0.04045f) {
			vR = v / 12.92f;
		}
		else {
			vR = std::powf((v + 0.055f) / 1.055f, 2.4f);
		}
		Converter::LUTsRGBToRGBFloat.SetValue(vR, i);
	}
}

void Converter::RGB2OKLab(pixel_t* r, pixel_t* g, pixel_t* b, pixel_t* lOut, pixel_t* aOut, pixel_t* bOut, int width, int height)
{
	int size = width * height;
	int inc = vfloat::size;

	//First step

	vfloat v11i = 0.4122214708f;
	vfloat v12i = 0.5363325363f;
	vfloat v13i = 0.0514459929f;
	
	vfloat v21i = 0.2119034982f;
	vfloat v22i = 0.6806995451f;
	vfloat v23i = 0.1073969566f;
	
	vfloat v31i = 0.0883024619f;
	vfloat v32i = 0.2817188376f;
	vfloat v33i = 0.6299787005f;

	//Second step

	vfloat v11o = 0.2104542553f;
	vfloat v12o = 0.7936177850f;
	vfloat v13o = -0.0040720468f;

	vfloat v21o = 1.9779984951f;
	vfloat v22o = -2.4285922050f;
	vfloat v23o = 0.4505937099f;

	vfloat v31o = 0.0259040371f;
	vfloat v32o = 0.7827717662f;
	vfloat v33o = -0.8086757660f;

	vfloat zeroThreeV = 1.f / 3.f;

#pragma omp parallel for
	for (int i = 0; i < size; i += inc)
	{
		auto rV = vfloat::load_aligned(&r[i]);
		auto gV = vfloat::load_aligned(&g[i]);
		auto bV = vfloat::load_aligned(&b[i]);

		auto l = rV * v11i + gV * v12i + bV * v13i;
		auto a = rV * v21i + gV * v22i + bV * v23i;
		auto b = rV * v31i + gV * v32i + bV * v33i;

		l = Converter::cbrtLUT.Get01(l);
		a = Converter::cbrtLUT.Get01(a);
		b = Converter::cbrtLUT.Get01(b);

		auto lOutV = l * v11o + a * v12o + b * v13o;
		auto aOutV = l * v21o + a * v22o + b * v23o;
		auto bOutV = l * v31o + a * v32o + b * v33o;

		lOutV.store_aligned(&lOut[i]);
		aOutV.store_aligned(&aOut[i]);
		bOutV.store_aligned(&bOut[i]);
	}
}

void Converter::RGB2OKLab(pixel_t rIn, pixel_t gIn, pixel_t bIn, pixel_t& lOut, pixel_t& aOut, pixel_t& bOut)
{
	float l = 0.4121656120 * rIn + 0.5362752080 * gIn + 0.0514575653 * bIn;
	float m = 0.2118591070 * rIn + 0.6807189584 * gIn + 0.1074065790 * bIn;
	float s = 0.0883097947 * rIn + 0.2818474174 * gIn + 0.6302613616 * bIn;

	l = Converter::cbrtLUT.Get01(l);
	m = Converter::cbrtLUT.Get01(m);
	s = Converter::cbrtLUT.Get01(s);

	lOut = 0.2104542553 * l + 0.7936177850 * m - 0.0040720468 * s;
	aOut = 1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s;
	bOut = 0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s;
}
