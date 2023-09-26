//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// StyleAsCode exporter v2.0 - Style data exported as a values array            //
//                                                                              //
// USAGE: On init call: GuiLoadStyleJungle();                                   //
//                                                                              //
// more info and bugs-report:  github.com/raysan5/raygui                        //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2020-2023 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define JUNGLE_STYLE_PROPS_COUNT  17

// Custom style name: Jungle
static const GuiStyleProp jungleStyleProps[JUNGLE_STYLE_PROPS_COUNT] = {
    { 0, 0, 0x60827dff },    // DEFAULT_BORDER_COLOR_NORMAL 
    { 0, 1, 0x2c3334ff },    // DEFAULT_BASE_COLOR_NORMAL 
    { 0, 2, 0x82a29fff },    // DEFAULT_TEXT_COLOR_NORMAL 
    { 0, 3, 0x5f9aa8ff },    // DEFAULT_BORDER_COLOR_FOCUSED 
    { 0, 4, 0x334e57ff },    // DEFAULT_BASE_COLOR_FOCUSED 
    { 0, 5, 0x6aa9b8ff },    // DEFAULT_TEXT_COLOR_FOCUSED 
    { 0, 6, 0xa9cb8dff },    // DEFAULT_BORDER_COLOR_PRESSED 
    { 0, 7, 0x3b6357ff },    // DEFAULT_BASE_COLOR_PRESSED 
    { 0, 8, 0x97af81ff },    // DEFAULT_TEXT_COLOR_PRESSED 
    { 0, 9, 0x5b6462ff },    // DEFAULT_BORDER_COLOR_DISABLED 
    { 0, 10, 0x2c3334ff },    // DEFAULT_BASE_COLOR_DISABLED 
    { 0, 11, 0x666b69ff },    // DEFAULT_TEXT_COLOR_DISABLED 
    { 0, 16, 0x0000000c },    // DEFAULT_TEXT_SIZE 
    { 0, 17, 0x00000000 },    // DEFAULT_TEXT_SPACING 
    { 0, 18, 0x638465ff },    // DEFAULT_LINE_COLOR 
    { 0, 19, 0x2b3a3aff },    // DEFAULT_BACKGROUND_COLOR 
    { 0, 20, 0x00000012 },    // DEFAULT_TEXT_LINE_SPACING 
};

// WARNING: This style uses a custom font: "Pixel Intv.otf" (size: 12, spacing: 0)

#define JUNGLE_STYLE_FONT_ATLAS_COMP_SIZE 2030

// Font atlas image pixels data: DEFLATE compressed
static unsigned char jungleFontData[JUNGLE_STYLE_FONT_ATLAS_COMP_SIZE] = { 0xed,
    0x9d, 0x3d, 0x8e, 0x25, 0x35, 0x14, 0x85, 0x8d, 0xe9, 0x8c, 0x10, 0x11, 0x80, 0x84, 0x20, 0x41, 0x22, 0x21, 0x61, 0x34,
    0x12, 0x64, 0xb3, 0x00, 0x62, 0x84, 0x08, 0x10, 0x11, 0x09, 0xb0, 0x07, 0x66, 0x65, 0x93, 0x90, 0xb3, 0x01, 0x76, 0xc1,
    0x02, 0x2e, 0x9a, 0xbf, 0x9e, 0x9f, 0xae, 0xb2, 0x7d, 0x8f, 0xaf, 0xcb, 0xae, 0x7a, 0x5f, 0x7f, 0x9a, 0xd1, 0xcc, 0xf3,
    0x73, 0x95, 0xcb, 0xc7, 0xd7, 0x55, 0xaf, 0x7d, 0xde, 0xb5, 0x25, 0x00, 0x00, 0x00, 0x80, 0x07, 0x3c, 0xff, 0xd9, 0x7e,
    0x6d, 0xab, 0x24, 0xbd, 0x2a, 0x69, 0x3f, 0xd6, 0xeb, 0xd7, 0x5f, 0x96, 0xe4, 0xdd, 0x77, 0x6c, 0x1f, 0x31, 0xef, 0xb4,
    0x61, 0xaf, 0xd5, 0xfb, 0x2d, 0x4b, 0xae, 0x92, 0xfd, 0xe3, 0xef, 0xf7, 0x57, 0x72, 0x5c, 0xc3, 0xbb, 0x3f, 0xc9, 0x71,
    0x2d, 0xe5, 0x7a, 0xa3, 0xf5, 0x2f, 0xf7, 0xc1, 0x7e, 0xd9, 0xcb, 0x3f, 0xcf, 0x7b, 0x22, 0x3b, 0x8e, 0xb9, 0xff, 0xfe,
    0xe4, 0x1a, 0x81, 0xfe, 0x91, 0xe1, 0xeb, 0xdd, 0xd2, 0xbb, 0x6b, 0xd7, 0x60, 0xce, 0x16, 0xbc, 0x7e, 0x7d, 0xa4, 0xfe,
    0xe9, 0xfe, 0x1c, 0xed, 0xf1, 0x5f, 0x53, 0xc5, 0xec, 0x6e, 0xb3, 0x27, 0x52, 0x51, 0x9b, 0x2c, 0x9c, 0x29, 0x62, 0x34,
    0xa7, 0xa0, 0xde, 0xcd, 0xc5, 0x6b, 0x30, 0x41, 0x7f, 0x6d, 0x84, 0x46, 0x1d, 0x47, 0x8b, 0xff, 0xe7, 0xdc, 0xbd, 0x18,
    0x01, 0xe5, 0xa3, 0xf6, 0xb7, 0x38, 0x2a, 0xfe, 0xd3, 0x50, 0xf5, 0x7b, 0xf4, 0x3f, 0x62, 0xfe, 0x2f, 0xbf, 0xbf, 0xdc,
    0x67, 0xdb, 0xc7, 0xcb, 0xaf, 0xe2, 0xdf, 0x7f, 0xf7, 0x8c, 0xd0, 0xdf, 0x7f, 0xff, 0x1f, 0xab, 0x7e, 0xc4, 0xfc, 0xbf,
    0xe2, 0x73, 0x64, 0xed, 0x3e, 0xbe, 0x3d, 0x76, 0x6c, 0x78, 0xfc, 0xcf, 0xa0, 0xa4, 0x7e, 0xed, 0xf9, 0xcf, 0x3a, 0xe6,
    0xd9, 0xd1, 0xf1, 0xaf, 0x1c, 0xeb, 0x75, 0x4f, 0x98, 0x10, 0x9f, 0x67, 0xd5, 0xbf, 0xa4, 0x3e, 0x44, 0xcc, 0x35, 0x9e,
    0xbb, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x35, 0x57, 0x4c, 0xca, 0xeb, 0xef,
    0x91, 0x2b, 0x91, 0xa3, 0xca, 0xda, 0xdd, 0x06, 0x1e, 0xe7, 0x4a, 0x16, 0x7a, 0x6c, 0x56, 0xbf, 0x8c, 0x72, 0xfb, 0x25,
    0xb7, 0x37, 0xa3, 0xa5, 0x35, 0xea, 0xf9, 0x7a, 0xf4, 0xcf, 0xce, 0xb5, 0xd7, 0xb2, 0x43, 0x6f, 0xcf, 0xcf, 0xb8, 0xbf,
    0x72, 0xdd, 0x7b, 0x7d, 0xde, 0x32, 0xdf, 0x0c, 0x90, 0x0e, 0x89, 0xff, 0x36, 0x0f, 0xd3, 0x18, 0x77, 0xe3, 0xb6, 0xcf,
    0xac, 0xd4, 0xd6, 0x3b, 0xb7, 0xa3, 0xf0, 0xcd, 0x6a, 0xf3, 0xd8, 0xf1, 0x1d, 0xed, 0xfa, 0x54, 0x1d, 0x25, 0xca, 0x98,
    0x54, 0x1c, 0xac, 0x11, 0xf3, 0x7f, 0x4c, 0xfc, 0x97, 0xc6, 0xa9, 0xbd, 0xf7, 0x27, 0x66, 0x1e, 0x9f, 0xa5, 0xff, 0x88,
    0xb9, 0x3a, 0x35, 0xcc, 0x0a, 0x6a, 0x5b, 0xb4, 0xfb, 0xbf, 0xe2, 0xd4, 0x29, 0xb9, 0x95, 0xf4, 0xe7, 0xa9, 0x19, 0xf1,
    0xdf, 0x13, 0xe3, 0xda, 0xfc, 0x5f, 0xd6, 0x3f, 0xce, 0xdd, 0x66, 0x53, 0x9e, 0xa6, 0x5b, 0x62, 0x6c, 0xad, 0xfb, 0x3f,
    0x44, 0xfa, 0x19, 0xe7, 0x7d, 0x9e, 0x42, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0xeb, 0xc7, 0x3b, 0xca, 0x53, 0x62, 0x8d, 0x59, 0x7c, 0xf6, 0xfd, 0x5a, 0x47, 0xb9, 0xf5, 0x8e, 0xef, 0xef, 0x68,
    0xdf, 0x47, 0xc4, 0x9a, 0xb4, 0x92, 0xc1, 0xac, 0xdf, 0x8b, 0x68, 0x4b, 0x38, 0xf2, 0x46, 0xf4, 0xdb, 0x4c, 0xfd, 0x95,
    0xf6, 0xe8, 0xfa, 0x9b, 0x98, 0x61, 0xb0, 0x96, 0x19, 0x66, 0x2d, 0xfd, 0x15, 0xd7, 0x41, 0x8f, 0xfe, 0xf5, 0xb9, 0x2a,
    0x15, 0xf4, 0xf0, 0xcf, 0xf1, 0xaa, 0xfe, 0x26, 0xe6, 0x7d, 0x1c, 0x73, 0x17, 0x1b, 0xa3, 0xbf, 0xd2, 0x6f, 0x57, 0x99,
    0xff, 0xeb, 0xc7, 0xcc, 0xe2, 0xfc, 0x6f, 0xd5, 0x59, 0xc5, 0xc2, 0xdd, 0xa8, 0xfe, 0x98, 0x6a, 0xed, 0x9f, 0xdb, 0xd5,
    0x3f, 0x0d, 0xd1, 0x7f, 0xcc, 0x19, 0xcf, 0x73, 0xff, 0x57, 0xb2, 0x57, 0xcf, 0xf0, 0xb9, 0xf5, 0x79, 0xc3, 0x6b, 0xa5,
    0x47, 0xf4, 0xcc, 0xb8, 0xf9, 0x1f, 0x17, 0xe0, 0x79, 0x5c, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x47, 0xac, 0x51, 0xe8, 0x6b, 0x67, 0x23, 0x56, 0x00, 0x1f, 0xbe, 0x9e, 0x1b, 0xd6, 0x30, 0xdf, 0x7f,
    0x3d, 0xef, 0x38, 0x46, 0xcc, 0x99, 0xf3, 0x4f, 0xdb, 0x8f, 0xb7, 0x35, 0x0f, 0x9a, 0xee, 0x20, 0x8c, 0x58, 0x07, 0x3c,
    0x5e, 0xff, 0xd4, 0xb4, 0x1e, 0xfb, 0x50, 0xc9, 0x2c, 0x78, 0x86, 0xb2, 0x33, 0xe3, 0x53, 0x29, 0x43, 0x94, 0xdf, 0xcd,
    0x54, 0xef, 0x05, 0x25, 0xd7, 0x5d, 0xed, 0x68, 0xda, 0x0e, 0xc5, 0x25, 0x77, 0x4c, 0xcd, 0xcb, 0xe2, 0xbf, 0xf2, 0xec,
    0xbe, 0xb6, 0xdc, 0x30, 0x07, 0x8c, 0xf7, 0x48, 0x27, 0x77, 0x3c, 0xf6, 0xf9, 0xee, 0xc6, 0xeb, 0x9f, 0x1a, 0x3c, 0x7e,
    0xd1, 0x8e, 0x14, 0xab, 0xec, 0x7e, 0xab, 0x8c, 0x80, 0xd1, 0xfa, 0xd7, 0xae, 0x55, 0x77, 0x09, 0xa5, 0xd0, 0x71, 0x13,
    0xad, 0x7f, 0xcb, 0x38, 0x8d, 0x1e, 0xc1, 0xca, 0x08, 0x38, 0x26, 0xfe, 0xcd, 0xad, 0x7f, 0x79, 0x6e, 0x88, 0xef, 0x3d,
    0xc5, 0x05, 0xa4, 0xcd, 0x28, 0xbd, 0x6e, 0x35, 0x45, 0xc5, 0x2c, 0xcd, 0x1c, 0x47, 0x3c, 0x41, 0x6b, 0x7b, 0x55, 0x47,
    0x3b, 0x36, 0xb5, 0x9e, 0x38, 0x5e, 0x7f, 0x93, 0x77, 0xfc, 0xce, 0xcb, 0xed, 0x7e, 0xad, 0x45, 0x9d, 0xee, 0xd8, 0x8b,
    0x8f, 0xff, 0xf5, 0xbe, 0x8d, 0x76, 0x44, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0xdb, 0xcc, 0x51, 0xe0, 0xcf, 0x54, 0x53, 0xaf, 0xa1, 0xee, 0x9d, 0x75, 0xe4, 0xde, 0xb9, 0xa3, 0xf6, 0xb7, 0xd3, 0xf3,
    0x82, 0xd9, 0x90, 0x1d, 0xe0, 0xe2, 0xd7, 0xcd, 0xfa, 0xb2, 0x98, 0xd5, 0x7a, 0x48, 0xcd, 0xb8, 0xe5, 0xf5, 0x60, 0xf4,
    0xf8, 0x7a, 0x54, 0xcf, 0x47, 0xbc, 0x0f, 0x2f, 0xc6, 0xdb, 0xa6, 0xc5, 0xbf, 0xe6, 0x0b, 0x59, 0x4d, 0xff, 0x74, 0x90,
    0x3f, 0xaf, 0x47, 0xff, 0xde, 0xdd, 0x9f, 0x75, 0x47, 0x96, 0xbf, 0x57, 0xaf, 0xa0, 0x7f, 0x9b, 0x7b, 0x57, 0xf1, 0x11,
    0x46, 0xfb, 0xf0, 0x66, 0xe9, 0x6f, 0xd2, 0xde, 0xb9, 0x3d, 0xfa, 0xf7, 0xf9, 0xe5, 0x8f, 0x8b, 0x7f, 0xfd, 0x8c, 0x69,
    0xc8, 0xce, 0xe8, 0xbd, 0xd1, 0xea, 0xcb, 0xd4, 0x38, 0xc3, 0xa9, 0x15, 0xef, 0x97, 0x1b, 0xe5, 0xec, 0x9a, 0xeb, 0x5e,
    0x8c, 0xf2, 0xaf, 0xf6, 0x66, 0xff, 0x9e, 0xa1, 0x3f, 0xde, 0xb1, 0x6b, 0x8f, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x5b, 0x5b, 0xc7, 0x89, 0xdb, 0x8f, 0xca, 0x9b, 0xd5, 0xcd, 0x36, 0x33, 0x7a, 0xa8,
    0xb9, 0x63, 0xbc, 0xde, 0x8a, 0xd2, 0x5a, 0xe5, 0x77, 0xf6, 0xe5, 0x3d, 0x8f, 0xec, 0x13, 0x57, 0xef, 0xd4, 0xea, 0xe6,
    0xb7, 0xea, 0x66, 0x77, 0xed, 0xd7, 0x59, 0x50, 0xb2, 0x70, 0x66, 0xbf, 0x03, 0xc0, 0x97, 0xab, 0xcd, 0xf7, 0x6a, 0x2e,
    0x3a, 0x45, 0x62, 0x5c, 0x17, 0x6a, 0x86, 0xbe, 0xc7, 0xef, 0xf4, 0xc1, 0xa3, 0xe0, 0xba, 0xf9, 0x1d, 0x25, 0x7d, 0xb5,
    0xf3, 0x7d, 0x66, 0xc3, 0xec, 0xae, 0xeb, 0xd3, 0x3f, 0xbb, 0x73, 0xb5, 0xf9, 0xd5, 0x89, 0xf1, 0x15, 0x6a, 0x59, 0x71,
    0x4a, 0xc7, 0xfc, 0xb2, 0xf0, 0xbf, 0xda, 0x35, 0xd5, 0xeb, 0xbe, 0x3d, 0xf3, 0xfa, 0x6b, 0xdf, 0xdd, 0xff, 0xed, 0xad,
    0xeb, 0x75, 0xf2, 0x68, 0x2e, 0xae, 0xde, 0x11, 0x10, 0x99, 0x4d, 0x51, 0x75, 0x64, 0xb5, 0xe8, 0xbf, 0x77, 0xce, 0x36,
    0xfd, 0xdf, 0xfc, 0xdb, 0x57, 0xbb, 0x14, 0xff, 0x9a, 0xfe, 0x71, 0x3e, 0xf7, 0xb1, 0xf1, 0x3f, 0x22, 0x2b, 0xde, 0x5e,
    0x49, 0x6b, 0xfc, 0x6b, 0x63, 0xa7, 0x27, 0xfe, 0x4b, 0xf7, 0x7f, 0xbf, 0xfe, 0xaa, 0x0b, 0xee, 0x68, 0xfd, 0x93, 0xec,
    0x53, 0xd5, 0x9e, 0x1a, 0xeb, 0x3d, 0xd9, 0x33, 0x77, 0x94, 0xee, 0xff, 0xaa, 0x86, 0xd1, 0xf1, 0xbf, 0x9e, 0xfe, 0xa9,
    0xc3, 0x59, 0xeb, 0x1d, 0x53, 0xad, 0x3d, 0x69, 0xe2, 0x0c, 0xbe, 0xff, 0xfc, 0x7f, 0xac, 0xfe, 0xb1, 0xce, 0x4a, 0x2d,
    0xab, 0xe3, 0x8a, 0xfc, 0xf2, 0xd6, 0x27, 0xa9, 0xc7, 0xf6, 0xbd, 0x6b, 0xfe, 0xaf, 0xd5, 0xed, 0x3b, 0xf3, 0xa8, 0xba,
    0x10, 0x39, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xa3, 0xd7, 0x89, 0xf6, 0xe9,
    0xa6, 0x43, 0xa4, 0x56, 0x6f, 0x74, 0x79, 0xcd, 0xd3, 0xf6, 0x95, 0x7d, 0x63, 0x9f, 0x3f, 0x78, 0xf5, 0x99, 0x3d, 0xb3,
    0x5f, 0x27, 0xb4, 0x35, 0xef, 0xee, 0x70, 0xbc, 0xf5, 0xa3, 0xbc, 0xa7, 0xb4, 0x8a, 0xd1, 0xe3, 0x44, 0xfb, 0xc1, 0x9e,
    0xd8, 0x1f, 0x1b, 0x47, 0xad, 0xd5, 0x1b, 0x5d, 0x5e, 0xd2, 0xff, 0x33, 0xfb, 0xd7, 0xfe, 0xb1, 0xa7, 0xf6, 0xc5, 0x66,
    0xad, 0xbf, 0x0e, 0x6f, 0x6b, 0xc9, 0xcd, 0xe3, 0xd9, 0x47, 0x59, 0xcb, 0x03, 0xd5, 0xe7, 0x44, 0xf9, 0xd9, 0x3e, 0xb0,
    0x1f, 0x37, 0x7c, 0x68, 0xb5, 0x7a, 0xa3, 0xcb, 0x4b, 0xfa, 0xff, 0x69, 0x66, 0x7f, 0xdb, 0x6f, 0xf6, 0xe1, 0x83, 0x92,
    0x27, 0xf6, 0x64, 0x42, 0x5b, 0x93, 0xdd, 0xbd, 0x68, 0xe7, 0x5d, 0xe7, 0x3e, 0xda, 0x6a, 0xfc, 0xf7, 0x38, 0xd1, 0x3e,
    0xb6, 0x9f, 0xec, 0x5b, 0xc1, 0x85, 0x30, 0x53, 0xff, 0xaf, 0xed, 0xbf, 0x1d, 0xfd, 0x35, 0x47, 0x45, 0x6f, 0x79, 0x6b,
    0xfc, 0xdb, 0xa0, 0xfc, 0x6f, 0x3d, 0xfa, 0xcf, 0xea, 0xb3, 0x76, 0x6f, 0xe6, 0x56, 0x9f, 0x7c, 0x64, 0x4f, 0x17, 0xd2,
    0xbf, 0x76, 0xff, 0x4f, 0x8e, 0x8c, 0xb3, 0xd1, 0xf7, 0xff, 0x33, 0xeb, 0x5f, 0x6a, 0xdf, 0xef, 0x0b, 0xe9, 0xdf, 0xe2,
    0xb0, 0x1a, 0xe7, 0x31, 0x19, 0xe3, 0x44, 0x43, 0xff, 0x38, 0xfd, 0xf5, 0xd8, 0x1e, 0xef, 0x81, 0x53, 0xeb, 0x8d, 0x2e,
    0xaf, 0xe1, 0xd1, 0x7f, 0x76, 0x5b, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xb4,
    0xe4, 0x8b, 0x2b, 0xaf, 0x3e, 0xe7, 0xe2, 0xda, 0x62, 0x69, 0xd5, 0x31, 0x17, 0x4a, 0x14, 0xa7, 0x43, 0xdb, 0xce, 0xc2,
    0x26, 0xe6, 0x3a, 0x8c, 0xbd, 0xce, 0xfa, 0xb5, 0xc4, 0x9e, 0x6f, 0x0f, 0x7f, 0xbe, 0xb8, 0xd6, 0xb6, 0xa4, 0x62, 0xf6,
    0xb0, 0xbc, 0xab, 0x71, 0x4f, 0x9f, 0x99, 0xbc, 0x8a, 0x5e, 0x7b, 0x4f, 0xfc, 0x75, 0x1e, 0x7b, 0xbe, 0x9e, 0x7c, 0x31,
    0x67, 0xcf, 0x70, 0xe1, 0xd1, 0xff, 0xd6, 0x68, 0x75, 0x52, 0x79, 0x33, 0xa0, 0xac, 0x5a, 0xa6, 0xe6, 0x0f, 0x3d, 0xdb,
    0x75, 0x46, 0xea, 0xaf, 0xc4, 0xd5, 0x59, 0xca, 0x3c, 0xd7, 0x79, 0x54, 0xd9, 0xc8, 0xe3, 0xaa, 0xfa, 0x9f, 0x59, 0xe3,
    0xde, 0xfb, 0xff, 0xad, 0xeb, 0x7f, 0xf6, 0xb9, 0xaf, 0xc5, 0x21, 0x6f, 0xe2, 0x3e, 0xe1, 0x23, 0xca, 0x8e, 0xd4, 0xbf,
    0xc7, 0x9d, 0x76, 0xb6, 0x18, 0x6f, 0xbd, 0x9e, 0x5b, 0x8a, 0xff, 0x51, 0xcf, 0xd5, 0xe7, 0xd3, 0xdf, 0xd0, 0xff, 0xa6,
    0xf5, 0x4f, 0x37, 0xf9, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0xa5, 0xf6, 0x3b, 0x55,
    0xab, 0xec, 0x0c, 0x6a, 0x8b, 0xfb, 0x02, 0xdb, 0xae, 0x6f, 0x0d, 0xcf, 0x5f, 0x96, 0xf6, 0xce, 0xec, 0xfb, 0x9d, 0xaf,
    0xde, 0x77, 0x65, 0xaf, 0xe1, 0x3a, 0xbe, 0x40, 0x3d, 0x73, 0xd6, 0xd1, 0x9e, 0xbf, 0xd2, 0x4e, 0xbb, 0xca, 0x0e, 0xd6,
    0x31, 0xeb, 0x1e, 0xd7, 0xf1, 0xff, 0x6d, 0xbd, 0x9e, 0x4f, 0xb0, 0x5e, 0x32, 0x72, 0xdd, 0x4b, 0x1d, 0x5b, 0x56, 0xe8,
    0xdb, 0xb5, 0xd6, 0xbd, 0x6a, 0x25, 0xf9, 0x02, 0xde, 0xa6, 0xbe, 0x11, 0xac, 0xf8, 0xa6, 0x29, 0xbb, 0x1d, 0xdf, 0xcb,
    0x7e, 0xcc, 0x9d, 0xbb, 0x0f, 0xce, 0xa4, 0xf1, 0xc8, 0xfb, 0x7f, 0xdd, 0xdf, 0x1d, 0x79, 0xdf, 0x58, 0x4b, 0xff, 0x33,
    0x5c, 0x43, 0xdd, 0xef, 0x63, 0x43, 0xf4, 0xaf, 0xd7, 0x39, 0xb7, 0xfe, 0xab, 0xcd, 0x0d, 0xb3, 0xbc, 0x3b, 0xba, 0xfe,
    0xd7, 0xf5, 0x7e, 0xcd, 0x68, 0xe7, 0x2c, 0xed, 0x95, 0x71, 0x67, 0x85, 0xac, 0xd1, 0x57, 0xf0, 0xbd, 0xcd, 0x68, 0x27,
    0xbe, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x46, 0xf3, 0x39, 0xcd, 0xce, 0xe1, 0xb7, 0xba,
    0x9b, 0xef, 0xd8, 0xf3, 0xf5, 0x79, 0x00, 0x95, 0x96, 0x1e, 0x9b, 0xc3, 0xaf, 0x7f, 0x75, 0xe5, 0xec, 0x19, 0xfc, 0xea,
    0xab, 0x5c, 0x57, 0x5e, 0x3f, 0x68, 0x8b, 0xff, 0x5b, 0x76, 0x71, 0x6b, 0x33, 0xc0, 0xd1, 0xb9, 0x8d, 0x7a, 0xca, 0xfa,
    0xb2, 0xa2, 0xcc, 0x70, 0x1d, 0x7a, 0x33, 0xcd, 0xa8, 0x65, 0xbd, 0x2e, 0xc0, 0xf5, 0x33, 0xe3, 0xf5, 0xdd, 0xff, 0x57,
    0x5b, 0xcd, 0x8f, 0x2e, 0xd3, 0xf5, 0x3f, 0x4b, 0xfc, 0x47, 0x39, 0xd9, 0xd6, 0x58, 0xcd, 0x5f, 0x4b, 0xff, 0x24, 0xcc,
    0xb9, 0x47, 0x97, 0x79, 0xbe, 0xaf, 0x30, 0x7f, 0x0e, 0x9b, 0xa1, 0xbf, 0x75, 0xc6, 0xff, 0x95, 0xf3, 0x57, 0x5d, 0x25,
    0x8b, 0x25, 0xd9, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xcd, 0xfb, 0x67, 0x15,
    0x3f, 0xda, 0x3a, 0xde, 0x40, 0xc5, 0xfb, 0x12, 0x7d, 0x05, 0x3d, 0x59, 0xfc, 0x4c, 0xf0, 0xb5, 0x8d, 0x76, 0x0e, 0x95,
    0xf3, 0xf8, 0xad, 0xe4, 0x0d, 0xdc, 0xef, 0xbb, 0x1c, 0xee, 0xfb, 0x8c, 0xcf, 0xe2, 0x97, 0x96, 0xd4, 0xff, 0x7c, 0x0e,
    0xb7, 0xed, 0x92, 0x7c, 0x82, 0xbe, 0x5b, 0x71, 0xfd, 0xf6, 0xfc, 0x2b, 0xdc, 0x6a, 0x76, 0xbc, 0x51, 0x65, 0x4a, 0xcd,
    0x99, 0x99, 0x36, 0xcf, 0x93, 0xc7, 0x4d, 0xf3, 0x76, 0x1d, 0xeb, 0x52, 0x9a, 0xe5, 0x7d, 0xba, 0x05, 0xfd, 0xd3, 0xf2,
    0xee, 0x36, 0xf4, 0x47, 0x7f, 0x6b, 0xf0, 0x30, 0x9f, 0x47, 0xff, 0x73, 0x3b, 0xdc, 0xe6, 0x65, 0x1b, 0x36, 0xf9, 0x1b,
    0x0a, 0x3c, 0xeb, 0x5f, 0xe3, 0xd3, 0x8a, 0xf2, 0xfd, 0x2c, 0xf4, 0xbf, 0xee, 0xe7, 0xd4, 0xf3, 0x7d, 0x36, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0xb2,
    0x0c, 0xa2, 0x3f, 0xfa, 0xdf, 0xb8, 0xfe, 0xff, 0x03 };

// Font glyphs rectangles data (on atlas)
static const Rectangle jungleFontRecs[189] = {
    { 4, 4, 5 , 12 },
    { 17, 4, 2 , 7 },
    { 27, 4, 5 , 3 },
    { 40, 4, 5 , 5 },
    { 53, 4, 6 , 7 },
    { 67, 4, 7 , 7 },
    { 82, 4, 5 , 7 },
    { 95, 4, 3 , 3 },
    { 106, 4, 4 , 8 },
    { 118, 4, 4 , 8 },
    { 130, 4, 5 , 5 },
    { 143, 4, 5 , 5 },
    { 156, 4, 2 , 3 },
    { 166, 4, 5 , 1 },
    { 179, 4, 2 , 2 },
    { 189, 4, 7 , 7 },
    { 204, 4, 7 , 6 },
    { 219, 4, 6 , 6 },
    { 233, 4, 6 , 6 },
    { 4, 24, 6 , 6 },
    { 18, 24, 6 , 6 },
    { 32, 24, 6 , 6 },
    { 46, 24, 6 , 6 },
    { 60, 24, 6 , 6 },
    { 74, 24, 6 , 6 },
    { 88, 24, 6 , 6 },
    { 102, 24, 2 , 5 },
    { 112, 24, 2 , 6 },
    { 122, 24, 3 , 5 },
    { 133, 24, 5 , 3 },
    { 146, 24, 3 , 5 },
    { 157, 24, 6 , 7 },
    { 171, 24, 7 , 7 },
    { 186, 24, 6 , 7 },
    { 200, 24, 6 , 7 },
    { 214, 24, 6 , 7 },
    { 228, 24, 6 , 7 },
    { 4, 44, 6 , 7 },
    { 18, 44, 6 , 7 },
    { 32, 44, 6 , 7 },
    { 46, 44, 6 , 7 },
    { 60, 44, 6 , 7 },
    { 74, 44, 6 , 7 },
    { 88, 44, 6 , 7 },
    { 102, 44, 6 , 7 },
    { 116, 44, 7 , 7 },
    { 131, 44, 6 , 7 },
    { 145, 44, 6 , 7 },
    { 159, 44, 6 , 7 },
    { 173, 44, 7 , 8 },
    { 188, 44, 6 , 7 },
    { 202, 44, 6 , 7 },
    { 216, 44, 6 , 7 },
    { 230, 44, 6 , 7 },
    { 4, 64, 6 , 7 },
    { 18, 64, 7 , 7 },
    { 33, 64, 6 , 7 },
    { 47, 64, 6 , 7 },
    { 61, 64, 6 , 7 },
    { 75, 64, 4 , 8 },
    { 87, 64, 7 , 7 },
    { 102, 64, 4 , 8 },
    { 114, 64, 4 , 2 },
    { 126, 64, 6 , 1 },
    { 140, 64, 2 , 2 },
    { 150, 64, 6 , 5 },
    { 164, 64, 6 , 7 },
    { 178, 64, 6 , 5 },
    { 192, 64, 6 , 7 },
    { 206, 64, 6 , 5 },
    { 220, 64, 6 , 7 },
    { 234, 64, 6 , 7 },
    { 4, 84, 6 , 7 },
    { 18, 84, 6 , 7 },
    { 32, 84, 5 , 8 },
    { 45, 84, 6 , 7 },
    { 59, 84, 6 , 7 },
    { 73, 84, 7 , 5 },
    { 88, 84, 6 , 5 },
    { 102, 84, 6 , 5 },
    { 116, 84, 6 , 7 },
    { 130, 84, 6 , 7 },
    { 144, 84, 6 , 5 },
    { 158, 84, 6 , 5 },
    { 172, 84, 6 , 6 },
    { 186, 84, 6 , 5 },
    { 200, 84, 6 , 5 },
    { 214, 84, 7 , 5 },
    { 229, 84, 6 , 5 },
    { 4, 104, 6 , 7 },
    { 18, 104, 6 , 5 },
    { 32, 104, 4 , 8 },
    { 44, 104, 2 , 8 },
    { 54, 104, 4 , 8 },
    { 66, 104, 5 , 2 },
    { 79, 104, 2 , 7 },
    { 89, 104, 6 , 6 },
    { 103, 104, 6 , 7 },
    { 117, 104, 6 , 7 },
    { 131, 104, 6 , 7 },
    { 145, 104, 8 , 8 },
    { 161, 104, 6 , 9 },
    { 175, 104, 8 , 8 },
    { 191, 104, 7 , 7 },
    { 206, 104, 8 , 8 },
    { 222, 104, 6 , 5 },
    { 236, 104, 8 , 8 },
    { 4, 124, 7 , 7 },
    { 19, 124, 8 , 8 },
    { 35, 124, 4 , 4 },
    { 47, 124, 8 , 8 },
    { 63, 124, 8 , 8 },
    { 79, 124, 8 , 8 },
    { 95, 124, 8 , 8 },
    { 111, 124, 6 , 7 },
    { 125, 124, 5 , 8 },
    { 138, 124, 3 , 3 },
    { 149, 124, 8 , 8 },
    { 165, 124, 8 , 8 },
    { 181, 124, 8 , 8 },
    { 197, 124, 6 , 5 },
    { 211, 124, 10 , 7 },
    { 229, 124, 10 , 5 },
    { 4, 144, 8 , 8 },
    { 20, 144, 6 , 7 },
    { 34, 144, 6 , 10 },
    { 48, 144, 6 , 10 },
    { 62, 144, 6 , 10 },
    { 76, 144, 6 , 10 },
    { 90, 144, 6 , 10 },
    { 104, 144, 6 , 10 },
    { 118, 144, 10 , 7 },
    { 136, 144, 6 , 9 },
    { 150, 144, 6 , 10 },
    { 164, 144, 6 , 10 },
    { 178, 144, 6 , 10 },
    { 192, 144, 6 , 10 },
    { 206, 144, 6 , 10 },
    { 220, 144, 6 , 10 },
    { 234, 144, 6 , 10 },
    { 4, 164, 6 , 10 },
    { 18, 164, 6 , 7 },
    { 32, 164, 6 , 10 },
    { 46, 164, 6 , 10 },
    { 60, 164, 6 , 10 },
    { 74, 164, 6 , 10 },
    { 88, 164, 6 , 10 },
    { 102, 164, 6 , 10 },
    { 116, 164, 6 , 5 },
    { 130, 164, 6 , 7 },
    { 144, 164, 6 , 10 },
    { 158, 164, 6 , 10 },
    { 172, 164, 6 , 10 },
    { 186, 164, 6 , 10 },
    { 200, 164, 6 , 10 },
    { 214, 164, 6 , 7 },
    { 228, 164, 6 , 7 },
    { 4, 184, 6 , 8 },
    { 18, 184, 6 , 8 },
    { 32, 184, 6 , 8 },
    { 46, 184, 6 , 8 },
    { 60, 184, 6 , 8 },
    { 74, 184, 6 , 8 },
    { 88, 184, 9 , 5 },
    { 105, 184, 6 , 7 },
    { 119, 184, 6 , 8 },
    { 133, 184, 6 , 8 },
    { 147, 184, 6 , 8 },
    { 161, 184, 6 , 8 },
    { 175, 184, 6 , 8 },
    { 189, 184, 6 , 8 },
    { 203, 184, 6 , 8 },
    { 217, 184, 6 , 8 },
    { 231, 184, 6 , 7 },
    { 4, 204, 6 , 8 },
    { 18, 204, 6 , 8 },
    { 32, 204, 6 , 8 },
    { 46, 204, 6 , 8 },
    { 60, 204, 6 , 8 },
    { 74, 204, 6 , 8 },
    { 88, 204, 5 , 5 },
    { 101, 204, 6 , 5 },
    { 115, 204, 6 , 8 },
    { 129, 204, 6 , 8 },
    { 143, 204, 6 , 8 },
    { 157, 204, 6 , 8 },
    { 171, 204, 6 , 10 },
    { 185, 204, 6 , 9 },
    { 199, 204, 6 , 10 },
};

// Font glyphs info data
// NOTE: No glyphs.image data provided
static const GlyphInfo jungleFontGlyphs[189] = {
    { 32, 0, 9, 5, { 0 }},
    { 33, 0, 2, 3, { 0 }},
    { 34, 0, 2, 6, { 0 }},
    { 35, 0, 3, 6, { 0 }},
    { 36, 0, 2, 7, { 0 }},
    { 37, 0, 2, 8, { 0 }},
    { 38, 0, 2, 6, { 0 }},
    { 39, 0, 2, 4, { 0 }},
    { 40, 0, 2, 5, { 0 }},
    { 41, 0, 2, 5, { 0 }},
    { 42, 0, 2, 6, { 0 }},
    { 43, 0, 3, 6, { 0 }},
    { 44, 0, 7, 3, { 0 }},
    { 45, 0, 5, 6, { 0 }},
    { 46, 0, 7, 3, { 0 }},
    { 47, 0, 2, 8, { 0 }},
    { 48, 0, 3, 8, { 0 }},
    { 49, 0, 3, 7, { 0 }},
    { 50, 0, 3, 7, { 0 }},
    { 51, 0, 3, 7, { 0 }},
    { 52, 0, 3, 7, { 0 }},
    { 53, 0, 3, 7, { 0 }},
    { 54, 0, 3, 7, { 0 }},
    { 55, 0, 3, 7, { 0 }},
    { 56, 0, 3, 7, { 0 }},
    { 57, 0, 3, 7, { 0 }},
    { 58, 0, 4, 3, { 0 }},
    { 59, 0, 4, 3, { 0 }},
    { 60, 0, 3, 4, { 0 }},
    { 61, 0, 4, 6, { 0 }},
    { 62, 0, 3, 4, { 0 }},
    { 63, 0, 2, 7, { 0 }},
    { 64, 0, 2, 8, { 0 }},
    { 65, 0, 2, 7, { 0 }},
    { 66, 0, 2, 7, { 0 }},
    { 67, 0, 2, 7, { 0 }},
    { 68, 0, 2, 7, { 0 }},
    { 69, 0, 2, 7, { 0 }},
    { 70, 0, 2, 7, { 0 }},
    { 71, 0, 2, 7, { 0 }},
    { 72, 0, 2, 7, { 0 }},
    { 73, 0, 2, 7, { 0 }},
    { 74, 0, 2, 7, { 0 }},
    { 75, 0, 2, 7, { 0 }},
    { 76, 0, 2, 7, { 0 }},
    { 77, 0, 2, 8, { 0 }},
    { 78, 0, 2, 7, { 0 }},
    { 79, 0, 2, 7, { 0 }},
    { 80, 0, 2, 7, { 0 }},
    { 81, 0, 2, 7, { 0 }},
    { 82, 0, 2, 7, { 0 }},
    { 83, 0, 2, 7, { 0 }},
    { 84, 0, 2, 7, { 0 }},
    { 85, 0, 2, 7, { 0 }},
    { 86, 0, 2, 7, { 0 }},
    { 87, 0, 2, 8, { 0 }},
    { 88, 0, 2, 7, { 0 }},
    { 89, 0, 2, 7, { 0 }},
    { 90, 0, 2, 7, { 0 }},
    { 91, 0, 2, 5, { 0 }},
    { 92, 0, 2, 8, { 0 }},
    { 93, 0, 2, 5, { 0 }},
    { 94, 0, -1, 5, { 0 }},
    { 95, 0, 10, 7, { 0 }},
    { 96, 0, -1, 3, { 0 }},
    { 97, 0, 4, 7, { 0 }},
    { 98, 0, 2, 7, { 0 }},
    { 99, 0, 4, 7, { 0 }},
    { 100, 0, 2, 7, { 0 }},
    { 101, 0, 4, 7, { 0 }},
    { 102, 0, 2, 7, { 0 }},
    { 103, 0, 4, 7, { 0 }},
    { 104, 0, 2, 7, { 0 }},
    { 105, 0, 2, 7, { 0 }},
    { 106, 0, 2, 6, { 0 }},
    { 107, 0, 2, 7, { 0 }},
    { 108, 0, 2, 7, { 0 }},
    { 109, 0, 4, 8, { 0 }},
    { 110, 0, 4, 7, { 0 }},
    { 111, 0, 4, 7, { 0 }},
    { 112, 0, 4, 7, { 0 }},
    { 113, 0, 4, 7, { 0 }},
    { 114, 0, 4, 7, { 0 }},
    { 115, 0, 4, 7, { 0 }},
    { 116, 0, 3, 7, { 0 }},
    { 117, 0, 4, 7, { 0 }},
    { 118, 0, 4, 7, { 0 }},
    { 119, 0, 4, 8, { 0 }},
    { 120, 0, 4, 7, { 0 }},
    { 121, 0, 4, 7, { 0 }},
    { 122, 0, 4, 7, { 0 }},
    { 123, 0, 2, 5, { 0 }},
    { 124, 0, 2, 3, { 0 }},
    { 125, 0, 2, 5, { 0 }},
    { 126, 0, -1, 6, { 0 }},
    { 161, 0, 2, 3, { 0 }},
    { 162, 0, 3, 7, { 0 }},
    { 163, 0, 2, 7, { 0 }},
    { 8364, 0, 2, 7, { 0 }},
    { 165, 0, 2, 7, { 0 }},
    { 352, 0, 1, 8, { 0 }},
    { 167, 0, 1, 7, { 0 }},
    { 353, 0, 1, 8, { 0 }},
    { 169, 0, 2, 8, { 0 }},
    { 170, 0, 1, 8, { 0 }},
    { 171, 0, 3, 7, { 0 }},
    { 172, 0, 1, 8, { 0 }},
    { 174, 0, 2, 8, { 0 }},
    { 175, 0, 1, 8, { 0 }},
    { 176, 0, 1, 2, { 0 }},
    { 177, 0, 1, 8, { 0 }},
    { 178, 0, 1, 8, { 0 }},
    { 179, 0, 1, 8, { 0 }},
    { 381, 0, 1, 8, { 0 }},
    { 181, 0, 4, 7, { 0 }},
    { 182, 0, 1, 4, { 0 }},
    { 183, 0, 4, 4, { 0 }},
    { 382, 0, 1, 8, { 0 }},
    { 185, 0, 1, 8, { 0 }},
    { 186, 0, 1, 8, { 0 }},
    { 187, 0, 3, 7, { 0 }},
    { 338, 0, 2, 11, { 0 }},
    { 339, 0, 4, 11, { 0 }},
    { 376, 0, 1, 8, { 0 }},
    { 191, 0, 2, 7, { 0 }},
    { 192, 0, -1, 7, { 0 }},
    { 193, 0, -1, 7, { 0 }},
    { 194, 0, -1, 7, { 0 }},
    { 195, 0, -1, 7, { 0 }},
    { 196, 0, -1, 7, { 0 }},
    { 197, 0, -1, 7, { 0 }},
    { 198, 0, 2, 11, { 0 }},
    { 199, 0, 2, 7, { 0 }},
    { 200, 0, -1, 7, { 0 }},
    { 201, 0, -1, 7, { 0 }},
    { 202, 0, -1, 7, { 0 }},
    { 203, 0, -1, 7, { 0 }},
    { 204, 0, -1, 7, { 0 }},
    { 205, 0, -1, 7, { 0 }},
    { 206, 0, -1, 7, { 0 }},
    { 207, 0, -1, 7, { 0 }},
    { 208, 0, 2, 7, { 0 }},
    { 209, 0, -1, 7, { 0 }},
    { 210, 0, -1, 7, { 0 }},
    { 211, 0, -1, 7, { 0 }},
    { 212, 0, -1, 7, { 0 }},
    { 213, 0, -1, 7, { 0 }},
    { 214, 0, -1, 7, { 0 }},
    { 215, 0, 3, 7, { 0 }},
    { 216, 0, 2, 7, { 0 }},
    { 217, 0, -1, 7, { 0 }},
    { 218, 0, -1, 7, { 0 }},
    { 219, 0, -1, 7, { 0 }},
    { 220, 0, -1, 7, { 0 }},
    { 221, 0, -1, 7, { 0 }},
    { 222, 0, 2, 7, { 0 }},
    { 223, 0, 2, 7, { 0 }},
    { 224, 0, 1, 7, { 0 }},
    { 225, 0, 1, 7, { 0 }},
    { 226, 0, 1, 7, { 0 }},
    { 227, 0, 1, 7, { 0 }},
    { 228, 0, 1, 7, { 0 }},
    { 229, 0, 1, 7, { 0 }},
    { 230, 0, 4, 10, { 0 }},
    { 231, 0, 4, 7, { 0 }},
    { 232, 0, 1, 7, { 0 }},
    { 233, 0, 1, 7, { 0 }},
    { 234, 0, 1, 7, { 0 }},
    { 235, 0, 1, 7, { 0 }},
    { 236, 0, 1, 7, { 0 }},
    { 237, 0, 1, 7, { 0 }},
    { 238, 0, 1, 7, { 0 }},
    { 239, 0, 1, 7, { 0 }},
    { 240, 0, 2, 7, { 0 }},
    { 241, 0, 1, 7, { 0 }},
    { 242, 0, 1, 7, { 0 }},
    { 243, 0, 1, 7, { 0 }},
    { 244, 0, 1, 7, { 0 }},
    { 245, 0, 1, 7, { 0 }},
    { 246, 0, 1, 7, { 0 }},
    { 247, 0, 3, 6, { 0 }},
    { 248, 0, 4, 7, { 0 }},
    { 249, 0, 1, 7, { 0 }},
    { 250, 0, 1, 7, { 0 }},
    { 251, 0, 1, 7, { 0 }},
    { 252, 0, 1, 7, { 0 }},
    { 253, 0, 1, 7, { 0 }},
    { 254, 0, 2, 7, { 0 }},
    { 255, 0, 1, 7, { 0 }},
};

// Style loading function: Jungle
static void GuiLoadStyleJungle(void)
{
    // Load style properties provided
    // NOTE: Default properties are propagated
    for (int i = 0; i < JUNGLE_STYLE_PROPS_COUNT; i++)
    {
        GuiSetStyle(jungleStyleProps[i].controlId, jungleStyleProps[i].propertyId, jungleStyleProps[i].propertyValue);
    }

    // Custom font loading
    // NOTE: Compressed font image data (DEFLATE), it requires DecompressData() function
    int jungleFontDataSize = 0;
    unsigned char *data = DecompressData(jungleFontData, JUNGLE_STYLE_FONT_ATLAS_COMP_SIZE, &jungleFontDataSize);
    Image imFont = { data, 256, 256, 1, 2 };

    Font font = { 0 };
    font.baseSize = 12;
    font.glyphCount = 189;

    // Load texture from image
    font.texture = LoadTextureFromImage(imFont);
    UnloadImage(imFont);  // Uncompressed image data can be unloaded from memory

    // Copy char recs data from global fontRecs
    // NOTE: Required to avoid issues if trying to free font
    font.recs = (Rectangle *)RAYGUI_MALLOC(font.glyphCount*sizeof(Rectangle));
    memcpy(font.recs, jungleFontRecs, font.glyphCount*sizeof(Rectangle));

    // Copy font char info data from global fontChars
    // NOTE: Required to avoid issues if trying to free font
    font.glyphs = (GlyphInfo *)RAYGUI_MALLOC(font.glyphCount*sizeof(GlyphInfo));
    memcpy(font.glyphs, jungleFontGlyphs, font.glyphCount*sizeof(GlyphInfo));

    GuiSetFont(font);

    // Setup a white rectangle on the font to be used on shapes drawing,
    // it makes possible to draw shapes and text (full UI) in a single draw call
    Rectangle fontWhiteRec = { 254, 254, 1, 1 };
    SetShapesTexture(font.texture, fontWhiteRec);

    //-----------------------------------------------------------------

    // TODO: Custom user style setup: Set specific properties here (if required)
    // i.e. Controls specific BORDER_WIDTH, TEXT_PADDING, TEXT_ALIGNMENT
}
