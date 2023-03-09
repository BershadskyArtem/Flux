#pragma once
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
};

