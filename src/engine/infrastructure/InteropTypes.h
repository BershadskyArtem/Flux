#pragma once
#include <vector>
#include "FLXDefines.h"
#include "../Core/WaveletImage.h"


struct InternalImageData;
struct InternalLabImage;
struct ProcessSettings;
struct BaseProcessingSetting;
struct CropProcessingSettings;
struct ResizeProcessingSettings;
struct DenoiseProcessingSettings;
struct DehazeProcessingSettings;
struct ClarityProcessingSettings;
struct TextureProcessingSettings;
struct LUTProcessingSettings;
struct ColorHSL;
struct BasicColorProcessingSettings;
struct ToneMappingProcessingSettings;
struct ColorSelectionBorder;
struct HSLProcessingSettings;
struct HSLColorProcessingSettings;
struct LightProcessingSettings;
struct HDRProcessingSettings;
struct WBProcessingSettings;
struct ProcessSettingsLayer;
struct ProcessingCache;
struct ProcessingMask;
struct ProcessingLayerCache;
struct ProcessingCacheEntry;



enum PixelType : int {
    UnsignedByte = 0,
    Float = 1,
    Half = 2
};

struct GeneralMetadata {
    int Width;
    int Height;
};

struct GeneralMetadataInternal{
    int Width;
    int Height;
    /// <summary>
    /// This can be anything you want.
    /// </summary>
    int MiscI32v1;
    /// <summary>
   /// This can be anything you want.
   /// </summary>
    int MiscI32v2;
};

struct ExifMetadata {
    //Exit metadata
    char* ProcessingSoftware;
    short BitsPerSample;
    short Compression;
    char* ImageDescription;

    //Essential
    char* Make;
    //Essential
    char* Model;
    short Orientation;
    //Essential
    char* Software;
    //Essential
    char* DateTime;
    //Essential
    char* Artist;
    //Essential
    char* CameraOwnerName;
    char* Owner;

    //Essential
    char* Copyright;

    unsigned long ExposureTimeNumerator;
    unsigned long ExposureTimeDenominator;

    unsigned long FNumberNumerator;
    unsigned long FNumberDenominator;

    long ShutterSpeedValueNumerator;
    long ShutterSpeedValueDenominator;

    unsigned long ApertureValueNumerator;
    unsigned long ApertureValueDenominator;

    long ExposureBiasValueNumerator;
    long ExposureBiasValueDenominator;

    short ExposureProgram;
    unsigned short ISOSpeedRatings;
    short MeteringMode;
    short LightSource;
    short Flash;

    short ColorSpace;

    short ExposureMode;
    short WhiteBalance;

    unsigned long FocalLengthNumerator;
    unsigned long FocalLengthDenominator;
};

enum ProcessingStage : int {
    Input,
    Crop,
    Resize,
    WaveletDecompose,
    Denoise,
    Details,
    WaveletCompose,
    Lut
};


struct ColorHSL {
    float Hue;
    float Saturation;
    float Lightness;
};

struct IptcMetadata {
    //Iptc metadata
};

struct MakerMetadata {

};

struct FluxImage {
    int Width;
    int Height;
    int Stride;
    PixelType Type;
    void* Pixels;
    int ChannelsCount;
};

struct InternalImageData {
    pixel_t* RPixels;
    pixel_t* GPixels;
    pixel_t* BPixels;
    int Width;
    int Height;
};

struct InternalLabImage {
    pixel_t* LPixels;
    pixel_t* APixels;
    pixel_t* BPixels;
    int Width;
    int Height;
};

struct ProcessSettings {
    ProcessingStage ChangedStage;
    int ChangedLayer;
    int LayersCount;
    ProcessSettingsLayer* Layers;
};

struct BaseProcessingSetting {
    int Version = 0;
};

struct CropProcessingSettings {
    int LeftUpX;
    int LeftUpY;
    int RightDownX;
    int RightDownY;
    int Mode;
};

struct ResizeProcessingSettings {
    int ResizeToX;
    int ResizeToY;
    //Used if Mode == 1
    pixel_t Scale;
    int Mode;
};

struct DenoiseProcessingSettings {
    int Luminance = 0;
    int Chrominance = 0;
    int Details = 0;
    int ImpulseNoise = 0;
    int Mode = 0;
};

struct DehazeProcessingSettings {
    int Amount = 0;
    int ColorR = 0;
    int ColorG = 0;
    int ColorB = 0;
    int Mode;
};

struct ClarityProcessingSettings {
    int Radius = 0;
    int Amount = 0;
    int Threshold = 0;
    int Mode = 0;
};

struct TextureProcessingSettings {
    int Amount;
    int Mode;
};



struct WBProcessingSettings {
    int Temperature = 5200;
    int Tint = 0;
    BaseProcessingSetting Mode;
};

struct LightProcessingSettings {
    float Exposure = 0;
    int Contrast = 0;
    int Brightness = 0;
    BaseProcessingSetting Mode;
};

struct HDRProcessingSettings {
    int Whites = 0;
    int Highlights = 0;
    int Shadows = 0;
    int Blacks = 0;
    BaseProcessingSetting Mode;
};


struct ColorSelectionBorder {
    float HueLeft = 0;
    float HueRight = 0;

    float LightnessLeft = 0;
    float LightnessRight = 0;

    float SaturationLeft = 0;
    float SaturationRight = 0;

    bool IsSmallerBorder = false;

    float Smoothness = 0;

    BaseProcessingSetting Mode;
};

struct HSLColorProcessingSettings {
    ColorSelectionBorder SelectedColor;

    float HueShift = 0;
    int Lightness = 0;
    int Saturation = 0;
    int Uniformity = 0;
    

    BaseProcessingSetting Mode;
};

struct HSLProcessingSettings {
    HSLColorProcessingSettings Red;
    HSLColorProcessingSettings Orange;
    HSLColorProcessingSettings Yellow;
    HSLColorProcessingSettings Green;
    HSLColorProcessingSettings Aqua;
    HSLColorProcessingSettings Blue;
    HSLColorProcessingSettings Purple;
    HSLColorProcessingSettings Magenta;
    HSLColorProcessingSettings Skin;

    int CustomColorsCount = 0;
    HSLColorProcessingSettings* CustomColors;

    BaseProcessingSetting Mode;
};


struct ToneMappingProcessingSettings {
    ColorHSL MainTone;
    ColorHSL ShadowsTone;
    ColorHSL MidTone;
    ColorHSL HighlightsTone;
    int Balance;
};

struct BasicColorProcessingSettings {
    int Vibrance = 0;
    int Saturation = 0;
    BaseProcessingSetting Mode;
};

struct LUTProcessingSettings {
    WBProcessingSettings WhiteBalance;
    LightProcessingSettings LightSettings;
    HDRProcessingSettings HDRSettings;
    HSLProcessingSettings HSLSettings;
    ToneMappingProcessingSettings ToneSettings;
    BasicColorProcessingSettings BasicColorSettings;
    
    BaseProcessingSetting Mode;
};


















struct ProcessSettingsLayer {
    CropProcessingSettings Crop;
    ResizeProcessingSettings Resize;
    DenoiseProcessingSettings Denoise;
    DehazeProcessingSettings Dehaze;
    ClarityProcessingSettings Clarity;
    TextureProcessingSettings Texture;
    LUTProcessingSettings LUT;
};

struct ProcessingCache {
    int LayersCount;
    ProcessingLayerCache* Layers;
};

struct ProcessingMask {

};

struct ProcessingLayerCache {
    int CachesCount;
    ProcessingCacheEntry* Caches;
    ProcessingMask Mask;
};

struct ProcessingCacheEntry {
    ProcessingStage Stage;
    int CachesCount;
    void* Caches;
    int SettingsCount;
    void* PreviousSettings;
};

//template <typename T>
//struct DenoiseCache {
//    int WaveletDecCount;
//    T* WaveletDec;
//};


struct WaveletCache {
    int WaveletDecCount;
    std::vector<WaveletImage<pixel_t>>* LDec;
    std::vector<WaveletImage<pixel_t>>* ADec;
    std::vector<WaveletImage<pixel_t>>* BDec;
};

