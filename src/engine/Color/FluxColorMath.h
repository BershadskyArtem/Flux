#pragma once
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/FluxMath.h"
#include "../infrastructure/InteropTypes.h"



class FluxColorMath {
private:
	

public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hue">Hue between -pi and +pi </param>
	/// <returns></returns>
	static inline pixel_t NormalizeOkHue(pixel_t hue) {
		//https://github.com/thomasp85/farver/blob/9bc85a6fd839dc6d2d919c772feee40740afe53d/src/Conversion.cpp#L632
		//God bless this shit.
		hue = hue / PI * 180;
		if (hue < 0) {
			hue += 360;
		}
		else if (hue >= 360) {
			hue -= 360;
		}
		return hue;
	}


	static inline pixel_t IsInsideHueChrominanceBoundary(pixel_t h, pixel_t c, ColorSelectionBorder &settings) {
		
		int hueBorderRadius = 0;
		
		pixel_t leftHue = settings.HueLeft;
		pixel_t rightHue = settings.HueRight;

		pixel_t targetHue = NormalizeOkHue(h);

		if (leftHue <= rightHue) {
			hueBorderRadius = rightHue - leftHue;
		}
		else {
			hueBorderRadius = std::abs(360 - leftHue) + rightHue;
		}

		
		pixel_t smoothness = settings.Smoothness;
		pixel_t resultingFactor = 1.0f;

		//Evaluate HUE
		//Basic case. We do not cross 360 hue point
		if (leftHue <= rightHue) {
			//pixel_t centerHuePoint = (leftHue + rightHue) / 2.0f;

			//If hue point is outside of target area
			if (!(leftHue <= targetHue) || !(targetHue <= rightHue)) {
				pixel_t difference = std::min(std::abs(leftHue - targetHue), std::abs(rightHue - targetHue));
				//Calculate how much we are away from smoothing area and normilize
				pixel_t hueTargetFactor = std::clamp(smoothness - difference, 0.0f, smoothness) / smoothness;
				resultingFactor *= hueTargetFactor;
			}

		}
		else {
			//The other way around
			if (targetHue < leftHue && targetHue > rightHue) {
				pixel_t difference = std::min(std::abs(targetHue - leftHue), std::abs(rightHue - targetHue));
				pixel_t hueTargetFactor = std::clamp(smoothness - difference, 0.0f, smoothness) / smoothness;
				resultingFactor *= hueTargetFactor;
			}
			
		}

		//Evaluate Chrominance
		pixel_t lowerChrominance = settings.SaturationLeft;
		pixel_t upperChrominance = settings.SaturationRight;

		if (!(lowerChrominance <= c) || !(c <= upperChrominance)) {
			pixel_t difference = std::min(std::abs(lowerChrominance - c), std::abs(upperChrominance - c));
			pixel_t chrominanceTargetFactor = std::clamp(smoothness - difference, 0.0f, smoothness) / smoothness;
			resultingFactor *= chrominanceTargetFactor;
		}

		return resultingFactor;
	}
};