/*******************************************************************************************
*
*   rGuiIcons v1.0 - A simple and easy-to-use minimalist icons creator
*
*   CONFIGURATION:
*
*   #define VERSION_ONE
*       Enable PRO features for the tool. Usually command-line and export options related.
*
*   #define CUSTOM_MODAL_DIALOGS
*       Use custom raygui generated modal dialogs instead of native OS ones
*       NOTE: Avoids including tinyfiledialogs depencency library
*
*   DEPENDENCIES:
*       raylib 2.6-dev          - Windowing/input management and drawing.
*       raygui 2.6              - Immediate-mode GUI controls.
*       tinyfiledialogs 3.3.9   - Open/save file dialogs, it requires linkage with comdlg32 and ole32 libs.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o rguiicons.exe rguiicons.c external/tinyfiledialogs.c -s rguiicons.rc.data -Iexternal /
*           -lraylib -lopengl32 -lgdi32 -lcomdlg32 -lole32 -std=c99 -Wall
*
*   COMPILATION (Linux - GCC):
*       gcc -o rguiicons rguiicons.c external/tinyfiledialogs.c -s -Iexternal -no-pie -D_DEFAULT_SOURCE /
*           -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
*
*   DEVELOPERS:
*       Ramon Santamaria (@raysan5):   Developer, supervisor, updater and maintainer.
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2019 raylib technologies (@raylibtech). All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"            // Required for: IMGUI controls

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define GUI_WINDOW_ABOUT_IMPLEMENTATION
#include "gui_window_about.h"           // GUI: About window

#include "external/tinyfiledialogs.h"   // Required for: Native open/save file dialogs

#include <stdio.h>                      // Required for: fopen(), fclose(), fread()...
#include <stdlib.h>                     // Required for: malloc(), free()
#include <string.h>                     // Required for: strcmp(), strlen()
#include <stdio.h>                      // Required for: FILE, fopen(), fread(), fwrite(), ftell(), fseek() fclose()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//#define COMMAND_LINE_ONLY             // Compile tool oly for command line usage

#if (!defined(DEBUG) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)))
bool __stdcall FreeConsole(void);       // Close console from code (kernel32.lib)
#endif

// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
  #define LOG(...) printf(__VA_ARGS__)
#else
  #define LOG(...)
#endif

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

#define MAX_ICONS_X     16
#define MAX_ICONS_Y     13
#define RICON_SIZE      16
#define ICONS_PADDING    2

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Icon generation parameters
typedef struct rIcon {
    int size;                   // Icon size (squared)
    int borderSize;             // Icon border size, usually size/16
    Color color;                // Icon base color
    
    char text[4];               // Icon text, max 4 characters, usually 3: rXX
    int textSize;               // Icon text height
    Rectangle textRec;          // Icon text position and size

    bool useTextPixels;         // Use icon text as pixels instead of text
    Color *textPixels;          // In case text is not enough... useful for 32x32, 24x24, 16x16
} rIcon;

typedef struct ToolData {
    int id;
} ToolData;

// One image entry for ico
typedef struct {
    int size;                   // Icon size (squared)
    int valid;                  // Icon valid image generated/loaded
    Image image;                // Icon image
    //Texture texture;            // Icon texture
} IconPackEntry;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char *toolName = "rToolName";
const char *toolVersion = "1.0";
const char *toolDescription = "A simple and easy-to-use <what tool does>";

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
#if defined(VERSION_ONE)
static void ShowCommandLineInfo(void);                      // Show command line usage info
static void ProcessCommandLine(int argc, char *argv[]);     // Process command line input
#endif

// Load/Save/Export data functions

static unsigned char *ImageToBits(Image image);
static Image ImageFromBits(unsigned char *bytes, int width, int height, Color color);
static Image GetIconsImage(unsigned int *values, int iconsCount, int iconsPerLine);
static void GenIconsHeaderFromImage(Image image, const char *headerFileName);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#if !defined(DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messsages
#endif
#if defined(COMMAND_LINE_ONLY)
    ProcessCommandLine(argc, argv);
#else
    char inFileName[256] = { 0 };       // Input file name (required in case of drag & drop over executable)

    // Command-line usage mode
    //--------------------------------------------------------------------------------------
    if (argc > 1)
    {
        if ((argc == 2) &&
            (strcmp(argv[1], "-h") != 0) &&
            (strcmp(argv[1], "--help") != 0))       // One argument (file dropped over executable?)
        {
            if (IsFileExtension(argv[1], ".xx1") ||
                IsFileExtension(argv[1], ".xx2"))
            {
                strcpy(inFileName, argv[1]);        // Read input filename to open with gui interface
            }
        }
#if defined(VERSION_ONE)
        else
        {
            ProcessCommandLine(argc, argv);
            return 0;
        }
#endif      // VERSION_ONE
    }
    
#if (defined(VERSION_ONE) && !defined(DEBUG) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)))
    // WARNING (Windows): If program is compiled as Window application (instead of console),
    // no console is available to show output info... solution is compiling a console application
    // and closing console (FreeConsole()) when changing to GUI interface
    FreeConsole();
#endif

    // GUI usage mode - Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 640;
    const int screenHeight = 320;

    InitWindow(screenWidth, screenHeight, FormatText("%s v%s - %s", toolName, toolVersion, toolDescription));
    SetExitKey(0);

    // Background noise texture generation
    Image imNoise = GenImageWhiteNoise(128, 128, 0.1f);
    ImageColorTint(&imNoise, GRAY);
    Texture2D texNoise = LoadTextureFromImage(imNoise);

    // General pourpose variables
    Vector2 mousePoint = { 0.0f, 0.0f };
    int framesCounter = 0;

    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(512, 512);
    SetTextureFilter(target.texture, FILTER_POINT);
    
    // Initialize rTool icons for drawing
    rIcon rToolPack[8] = { 0 };

    for (int i = 0; i < 8; i++)
    {
        rToolPack[i].size = 16*i;
        rToolPack[i].borderSize = (int)ceil((float)rToolPack[i].size/16.0f);
        strcpy(rToolPack[i].text, "rIP\0");
        rToolPack[i].textSize = 50/(i + 1);         // TODO: Find a working formula > 50, 30, 20, 20, 10, 10, 10, 6?
        rToolPack[i].textRec.width = MeasureText(rToolPack[i].text, rToolPack[i].textSize);
        rToolPack[i].textRec.height = rToolPack[i].textSize;
        rToolPack[i].textRec.x = rToolPack[i].size - 2*rToolPack[i].borderSize - rToolPack[i].textRec.width;
        rToolPack[i].textRec.y = rToolPack[i].size - 2*rToolPack[i].borderSize - rToolPack[i].textRec.height;
        //rToolPack[i].proVersion = false;
        rToolPack[i].color = DARKGRAY;

        // Color array to fill
        rToolPack[i].textPixels = (Color *)malloc(rToolPack[i].size*rToolPack[i].size*sizeof(Color));
        for (int p = 0; p < rToolPack[i].size*rToolPack[i].size; p++) rToolPack[i].textPixels[p] = BLANK;
    }

    bool iconEditMode = true;      // Icon pixel edition mode
    int iconEditText = 0;           // 0 - Move/Scale text, 1 - Text pixels edit mode
    float iconEditScale = 1.0f;     // Icon edit mode scale
    float iconEditOffset = 0.0f;    // Icon edit offset inside box, allow icon movement

    Vector2 cell = { -1, -1 };      // Grid cell mouse position

    //RenderTexture2D iconTarget = LoadRenderTexture(256, 256);     // To draw icon and retrieve it?
    //SetTextureFilter(iconTarget.texture, FILTER_POINT);

    //Image icon = GetTextureData(iconTarget.texture);
    //UpdateTexture(iconTarget.texture, const void *pixels);
    
    Vector2 anchor01 = { 10, 10 };

    // GUI: About Window
    //-----------------------------------------------------------------------------------
    GuiWindowAboutState windowAboutState = InitGuiWindowAbout();
    //-----------------------------------------------------------------------------------
    
    // GUI: Exit Window
    //-----------------------------------------------------------------------------------
    bool exitWindow = false;
    bool windowExitActive = false;
    //-----------------------------------------------------------------------------------

    // Check if an input file has been provided on command line
    if (inFileName[0] != '\0')
    {
        // TODO: Do something with loaded data
    }
    else
    {
        // TODO: Set some default values
    }

    SetTargetFPS(60);
    //------------------------------------------------------------

    // Main game loop
    while (!exitWindow)             // Detect window close button
    {
        // Dropped files logic
        //----------------------------------------------------------------------------------
        if (IsFileDropped())
        {
            int dropsCount = 0;
            char **droppedFiles = GetDroppedFiles(&dropsCount);

            if (IsFileExtension(droppedFiles[0], ".ex1") ||
                IsFileExtension(droppedFiles[0], ".ex2"))
            {
                // TODO: Do something with recognized fileformats

                SetWindowTitle(FormatText("%s v%s - %s", toolName, toolVersion, inFileName));
            }

            ClearDroppedFiles();
        }
        //----------------------------------------------------------------------------------

        // Keyboard shortcuts
        //------------------------------------------------------------------------------------
        //if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) DialogSaveToolData();       // Show dialog: save tool data (.ex1)
        //if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) DialogLoadToolData();       // Show dialog: load tool data (.ex1)
        //if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_E)) DialogExportToolData(data); // Show dialog: export tool data (.ex1)

        if (IsKeyPressed(KEY_F1)) windowAboutState.windowAboutActive = !windowAboutState.windowAboutActive;
        //----------------------------------------------------------------------------------

        // Basic program flow logic
        //----------------------------------------------------------------------------------
        framesCounter++;                    // General usage frames counter
        mousePoint = GetMousePosition();    // Get mouse position each frame
        if (WindowShouldClose()) exitWindow = true;

        // Show closing window on ESC
        if (IsKeyPressed(KEY_ESCAPE))
        {
            // TODO: Define KEY_ESCAPE custom logic (i.e. Show save dialog)
            exitWindow = true;
        }

        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) ToggleFullscreen();
        
        if (IsKeyPressed(KEY_SPACE)) iconEditMode = !iconEditMode;

        if (iconEditMode)
        {
            // Choose text edit mode, edit text or edit pixels
            if (IsKeyPressed(KEY_E)) iconEditText = !iconEditText;
            
            iconEditScale += (float)GetMouseWheelMove()/10.0f;
            if (iconEditScale < 0.2f) iconEditScale = 0.2f;

            // Edit selected rTool icon text position and size
            if (IsKeyPressed(KEY_RIGHT)) rToolPack[2].textRec.x++;
            else if (IsKeyPressed(KEY_LEFT)) rToolPack[2].textRec.x--;
            else if (IsKeyPressed(KEY_UP)) rToolPack[2].textRec.y--;
            else if (IsKeyPressed(KEY_DOWN)) rToolPack[2].textRec.y++;

            if (IsKeyDown(KEY_LEFT_CONTROL))
            {
                if (IsKeyPressed(KEY_UP)) rToolPack[2].textSize++;
                else if (IsKeyPressed(KEY_DOWN)) rToolPack[2].textSize--;
            }

            if (iconEditText == 1)
            {
                // Pixels edit mode
                if ((cell.x >= 0) && (cell.y >= 0))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    {
                        rToolPack[3 - 1].textPixels[(int)cell.x + (int)cell.y*rToolPack[3 - 1].size] = rToolPack[3 - 1].color;
                    }
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                    {
                        rToolPack[3 - 1].textPixels[(int)cell.x + (int)cell.y*rToolPack[3 - 1].size] = BLANK;
                    }

                    // TODO: Update icon texture...
                    // ISSUE: We are not drawing textures now, icon is just draw with basic shapes!
                    //if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) UpdateTexture(texIcon, rToolPack[3 - 1].textPixels);
                }
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);     // Clear default framebuffer

            // Draw background noise details
            for (int y = 0; y < (GetScreenHeight()/texNoise.height) + 1; y++)
                for (int x = 0; x < (GetScreenWidth()/texNoise.width) + 1; x++)
                    DrawTexture(texNoise, texNoise.width*x, texNoise.height*y, Fade(WHITE, 0.06f));

            // Draw background vignette to focus in loaded image
            DrawRectangleGradientV(0, 0, GetScreenWidth(), 200, Fade(BLACK, 0.3f), BLANK);
            DrawRectangleGradientV(0, GetScreenHeight() - 200, GetScreenWidth(), 200, BLANK, Fade(BLACK, 0.3f));

            // Draw texture to render target
            BeginTextureMode(target);

                ClearBackground(BLANK);     // Clear render target

                // Draw checked board for alpha images
                /*
                for (int y = 0; y < image.height/texChecked.height + 1; y++)
                    for (int x = 0; x < image.width/texChecked.width + 1; x++)
                        DrawTextureEx(texChecked, (Vector2){ texChecked.width*x, texChecked.height*y }, 0.0f, 1.0f, Fade(WHITE, 0.5f));
                */
                
                //GuiToggleGroup()

                if (iconEditMode)
                {
                    if (3 == 0)
                    {
                        // NOTE: On custom rTool icon edit mode, we don't need all icons drawn...
                    }
                    else if (3 > 0)
                    {
                        if (iconEditText == 0)       // Edit text position
                        {
                            // Draw rTool generated icon
                            DrawRectangle(anchor01.x + 135 + 128 - rToolPack[3 - 1].size/2,
                                          anchor01.y + 10 + 128 - rToolPack[3 - 1].size/2,
                                          rToolPack[3 - 1].size, rToolPack[3 - 1].size, RAYWHITE);
                            DrawRectangleLinesEx((Rectangle){ anchor01.x + 135 + 128 - rToolPack[3 - 1].size/2,
                                                              anchor01.y + 10 + 128 - rToolPack[3 - 1].size/2,
                                                              rToolPack[3 - 1].size, rToolPack[3 - 1].size },
                                                 rToolPack[3 - 1].borderSize, rToolPack[3 - 1].color);
                            DrawText(rToolPack[3 - 1].text,
                                     anchor01.x + 135 + 128 - rToolPack[3 - 1].size/2 + rToolPack[3 - 1].textRec.x,
                                     anchor01.y + 10 + 128 - rToolPack[3 - 1].size/2 + rToolPack[3 - 1].textRec.y,
                                     rToolPack[3 - 1].textSize, rToolPack[3 - 1].color);
                        }
                        else if (iconEditText == 1)     // Edit text pixels painting
                        {
                            int size = rToolPack[3 - 1].size;
                            float scaleFactor = 1.0f;

                            switch (size)
                            {
                                case 16:
                                case 24:
                                case 32: scaleFactor = 8.0f; break;
                                case 48: scaleFactor = 5.0f; break;
                                case 64: scaleFactor = 4.0f; break;
                                case 96: 
                                case 128: scaleFactor = 2.0f; break;
                                case 256: break;
                                default: break;
                            }
                            
                            float scaledSize = size*scaleFactor;
                            
                            // TODO: Use render target to apply iconEditScale
                            
                            BeginScissorMode(anchor01.x + 135, anchor01.y + 10, 256, 256);

                            // Draw icon scaled for painting
                            DrawRectangle(anchor01.x + 135 + 128 - scaledSize/2, anchor01.y + 10 + 128 - scaledSize/2, scaledSize, scaledSize, RAYWHITE);
                            DrawRectangleLinesEx((Rectangle){ anchor01.x + 135 + 128 - scaledSize/2, anchor01.y + 10 + 128 - scaledSize/2, scaledSize, scaledSize },
                                                 rToolPack[3 - 1].borderSize*scaleFactor, rToolPack[3 - 1].color);

                            DrawText(rToolPack[3 - 1].text,
                                 anchor01.x + 135 + 128 - scaledSize/2 + rToolPack[3 - 1].textRec.x*scaleFactor,
                                 anchor01.y + 10 + 128 - scaledSize/2 + rToolPack[3 - 1].textRec.y*scaleFactor,
                                 rToolPack[3 - 1].textSize*10, Fade(rToolPack[3 - 1].color, 0.4f));

                            // Draw grid (returns selected cell)
                            cell = GuiGrid((Rectangle){ anchor01.x + 135 + 128 - scaledSize/2, anchor01.y + 10 + 128 - scaledSize/2, scaledSize, scaledSize }, scaleFactor, 1);

                            // Draw selected cell lines
                            if ((cell.x >= 0) && (cell.y >= 0)) DrawRectangleLinesEx((Rectangle){ anchor01.x + 135 + 128 - scaledSize/2 + cell.x*scaleFactor, 
                                                                                                  anchor01.y + 10 + 128 - scaledSize/2 + cell.y*scaleFactor, 
                                                                                                  scaleFactor, scaleFactor }, 1, RED);

                            // Draw pixel rectangles
                            for (int y = 0; y < size; y++)
                            {
                                for (int x = 0; x < size; x++)
                                {
                                    DrawRectangleRec((Rectangle){ anchor01.x + 135 + 128 - scaledSize/2 + x*scaleFactor,
                                                                  anchor01.y + 10 + 128 - scaledSize/2 + y*scaleFactor, 
                                                                  scaleFactor, scaleFactor }, rToolPack[3 - 1].textPixels[y*size + x]);
                                }
                            }
                            
                            EndScissorMode();
                        }

                        DrawText(FormatText("%i", rToolPack[3 - 1].textSize), GetScreenWidth() - 50, 35, 10, RED);
                    }
                }

            EndTextureMode();
            
            DrawTextureRec(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Vector2){ 0, 0 }, WHITE);

            // GUI: About Window
            //--------------------------------------------------------------------------------
            GuiWindowAbout(&windowAboutState);
            //--------------------------------------------------------------------------------
            
            // GUI: Exit Window
            //--------------------------------------------------------------------------------
            if (windowExitActive)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.85f));
                int message = GuiMessageBox((Rectangle){ GetScreenWidth()/2 - 125, GetScreenHeight()/2 - 50, 250, 100 }, 
                                            FormatText("#159#Closing %s", toolName), "Do you really want to exit?", "Yes;No"); 
            
                if ((message == 0) || (message == 2)) windowExitActive = false;
                else if (message == 1) exitWindow = true;
            }
            //--------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texNoise);    // Unload background noise texture

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

#endif          // COMMAND_LINE_ONLY
    return 0;
}

//--------------------------------------------------------------------------------------------
// Module functions
//--------------------------------------------------------------------------------------------

#if defined(VERSION_ONE)
// Show command line usage info
static void ShowCommandLineInfo(void)
{
    printf("\n//////////////////////////////////////////////////////////////////////////////////\n");
    printf("//                                                                              //\n");
    printf("// %s v%s ONE - %s           //\n", toolName, toolVersion, toolDescription);
    printf("// powered by raylib v2.5 (www.raylib.com) and raygui v2.0                      //\n");
    printf("// more info and bugs-report: github.com/raysan5/rfxgen                         //\n");
    printf("//                                                                              //\n");
    printf("// Copyright (c) 2019 raylib technologies (@raylibtech)                         //\n");
    printf("//                                                                              //\n");
    printf("//////////////////////////////////////////////////////////////////////////////////\n\n");

    printf("USAGE:\n\n");
    printf("    > rguiicons [--help] --input <filename.ext> [--output <filename.ext>]\n");
    printf("                    [--format <value>]\n");

    printf("\nOPTIONS:\n\n");
    printf("    -h, --help                      : Show tool version and command line usage help\n");
    printf("    -i, --input <filename.ext>      : Define input file.\n");
    printf("                                      Supported extensions: .rfx, .sfs, .wav\n");
    printf("    -o, --output <filename.ext>     : Define output file.\n");
    printf("                                      Supported extensions: .wav, .h\n");
    printf("                                      NOTE: If not specified, defaults to: output.wav\n\n");
    printf("    -f, --format <value>            : Format output file.\n");
    printf("                                      Supported values:\n");
    printf("                                          Sample rate:      22050, 44100\n");
    printf("                                          Sample size:      8, 16, 32\n");
    printf("                                          Channels:         1 (mono), 2 (stereo)\n");
    printf("                                      NOTE: If not specified, defaults to: 44100, 16, 1\n\n");

    printf("\nEXAMPLES:\n\n");
    printf("    > rguiicons --input sound.rfx --output jump.wav\n");
    printf("        Process <sound.rfx> to generate <sound.wav> at 44100 Hz, 32 bit, Mono\n\n");
    printf("    > rguiicons --input sound.rfx --output jump.wav --format 22050 16 2\n");
    printf("        Process <sound.rfx> to generate <jump.wav> at 22050 Hz, 16 bit, Stereo\n\n");
    printf("    > rguiicons --input sound.rfx --play\n");
    printf("        Plays <sound.rfx>, wave data is generated internally but not saved\n\n");
}

// Process command line input
static void ProcessCommandLine(int argc, char *argv[])
{
    // CLI required variables
    bool showUsageInfo = false;     // Toggle command line usage info

    char inFileName[256] = { 0 };   // Input file name
    char outFileName[256] = { 0 };  // Output file name
    int outputFormat = 0;           // Supported output formats

    // Process command line arguments
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            showUsageInfo = true;
        }
        else if ((strcmp(argv[i], "-i") == 0) || (strcmp(argv[i], "--input") == 0))
        {
            // Check for valid upcoming argument
            if (((i + 1) < argc) && (argv[i + 1][0] != '-'))
            {
                // Check for valid file extension: input
                if (IsFileExtension(argv[i + 1], ".xx1") ||
                    IsFileExtension(argv[i + 1], ".xx2"))
                {
                    strcpy(inFileName, argv[i + 1]);    // Read input file
                }
                else printf("WARNING: Input file extension not recognized.\n");
                
                i++;
            }
            else printf("WARNING: No input file provided\n");
        }
        else if ((strcmp(argv[i], "-o") == 0) || (strcmp(argv[i], "--output") == 0))
        {
            // Check for valid upcoming argumment and valid file extension: output
            if (((i + 1) < argc) && (argv[i + 1][0] != '-'))
            {
                // Check for valid file extension: output
                if (IsFileExtension(argv[i + 1], ".out1") ||
                    IsFileExtension(argv[i + 1], ".out2"))
                {
                    strcpy(outFileName, argv[i + 1]);   // Read output filename
                }
                else printf("WARNING: Output file extension not recognized.\n");
                
                i++;
            }
            else printf("WARNING: No output file provided\n");
        }
        else if ((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--format") == 0))
        {
            // Check for valid argument and valid parameters
            if (((i + 1) < argc) && (argv[i + 1][0] != '-'))
            {
                // TODO: Read provided values
            }
            else printf("WARNING: Format parameters provided not valid\n");
        }
    }

    // Process input file if provided
    if (inFileName[0] != '\0')
    {
        if (outFileName[0] == '\0') strcpy(outFileName, "output.out");  // Set a default name for output in case not provided

        printf("\nInput file:       %s", inFileName);
        printf("\nOutput file:      %s", outFileName);
        printf("\nOutput format:    %i\n\n", 0);

        // TODO: Process input --> output
    }

    if (showUsageInfo) ShowCommandLineInfo();
}
#endif      // VERSION_ONE

//--------------------------------------------------------------------------------------------
// Load/Save/Export functions
//--------------------------------------------------------------------------------------------
// ...

//--------------------------------------------------------------------------------------------
// Auxiliar functions
//--------------------------------------------------------------------------------------------

// Converts an image to bits array considering only no-alpha-pixel vs alpha-pixel
// Very useful to store 1bit color images in an efficient (and quite secure) way
// NOTE: Image size MUST be multiple of 8 for correct fit
static unsigned char *ImageToBits(Image image)
{
    Color *pixels = GetImageData(image);

    // Calculate number of bytes required
    int size = image.width*image.height/8;

    unsigned char *bytes = (unsigned char *)calloc(size, sizeof(unsigned char));

    for (int i = 0; i < size; i++)
    {
        for (int k = 0; k < 8; k++)
        {
            if (ColorToInt(pixels[i]) >= 0x000000ff) BIT_SET(bytes[i + k], k);
        }
    }

    free(pixels);

    return bytes;
}

// Generate color-alpha image from and array of bits, stored in bytes
// NOTE: 0-BLANK, 1-color
static Image ImageFromBits(unsigned char *bytes, int width, int height, Color color)
{
    Image image = { 0 };

    image.width = width;
    image.height = height;
    image.mipmaps = 1;
    image.format = UNCOMPRESSED_R8G8B8A8;
    image.data = (Color *)calloc(image.width*image.height, sizeof(Color));

    for (int i = 0; i < width*height/8; i++)
    {
        for (int k = 0; k < 8; k++)
        {
            if (BIT_CHECK(bytes[i], k)) ((Color *)image.data)[i + k] = color;
        }
    }

    return image;
}

// Generate color-alpha image from and array of bits, stored as int
// NOTE: 0-BLANK, 1-WHITE
static Image GetIconsImage(unsigned int *values, int iconsCount, int iconsPerLine)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    Image image = { 0 };

    image.width = RICON_SIZE*iconsPerLine;
    image.height = (RICON_SIZE*(1 + (iconsCount/iconsPerLine)));
    image.mipmaps = 1;
    image.format = UNCOMPRESSED_GRAY_ALPHA;
    image.data = (unsigned char *)calloc(image.width*image.height, 2);      // All pixels BLANK by default
    
    printf("Image size: [%i, %i]\n", image.width, image.height);

    int pixelX = 0;
    int pixelY = 0;

    for (int n = 0; n < iconsCount; n++)
    {
        for (int i = 0, y = 0; i < RICON_SIZE*RICON_SIZE/32; i++)
        {
            for (int k = 0; k < 32; k++)
            {
                pixelX = (n%iconsPerLine)*RICON_SIZE + (k%RICON_SIZE);
                pixelY = (n/iconsPerLine)*RICON_SIZE + y;

                if (BIT_CHECK(values[8*n + i], k)) ((unsigned short *)image.data)[pixelY*image.width + pixelX] = 0xffff;    // Draw pixel WHITE

                if ((k == 15) || (k == 31)) y++;
            }
        }
    }

    return image;
}

void GenIconsHeaderFromImage(Image image, const char *headerFileName)
{
    //Image ricons = LoadImage(riconFile);    //ricons.png
    Color *pixels = GetImageData(image);
    
    Rectangle icorec = { 0, 0, RICON_SIZE, RICON_SIZE };
    Color pixel = BLACK;
    
    // Calculate number of bytes required
    int size = MAX_ICONS_X*MAX_ICONS_Y*RICON_SIZE*RICON_SIZE/32;
    unsigned int *values = (unsigned int *)calloc(size, sizeof(unsigned int));
    
    int n = 0;      // Icons counter
    int k = 0;      // Bit counter
    
    for (int y = 0; y < MAX_ICONS_Y; y++)
    {
        for (int x = 0; x < MAX_ICONS_X; x++)
        {
            // Get icon start pixel position within image (top-left corner)
            icorec.x = ICONS_PADDING + x*(RICON_SIZE + 2*ICONS_PADDING);
            icorec.y = ICONS_PADDING + y*(RICON_SIZE + 2*ICONS_PADDING);

            // Move along pixels within each icon area
            for (int p = 0; p < RICON_SIZE*RICON_SIZE; p++)
            {
                pixel = pixels[((int)icorec.y + p/RICON_SIZE)*(MAX_ICONS_X*(RICON_SIZE + 2*ICONS_PADDING)) + ((int)icorec.x + p%RICON_SIZE)];
                
                if (ColorToInt(pixel) == 0xffffffff) BIT_SET(values[n*(RICON_SIZE*RICON_SIZE/32) + p/32], k);
                
                k++;
                if (k == 32) k = 0;
            }
            
            n++;
        }
    }

    // Save binary values into header file
    FILE *riconsFile = fopen(headerFileName, "wt");
    
    fprintf(riconsFile, "static unsigned int RICONS[%i] = {\n", size);
    for (int i = 0; i < size; i++) fprintf(riconsFile, (((i + 1)%8 == 0)? "0x%08x,\n" : "0x%08x, "), values[i]);
    fprintf(riconsFile, "};\n");
    
    fclose(riconsFile);
    
    free(pixels);
    UnloadImage(image);
}

#if 0
void DrawIcon(int iconId, Vector2 position, int pixelSize, Color color)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))
    
    for (int i = 0, y = 0; i < RICON_SIZE*RICON_SIZE/32; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            if (BIT_CHECK(RICONS[8*iconId + i], k)) DrawRectangle(position.x + (k%RICON_SIZE)*pixelSize, position.y + y*pixelSize, pixelSize, pixelSize, color);
            if ((k == 15) || (k == 31)) y++;
        }
    }
}
#endif

/*
#define ICO_DATA_READER
#if defined(ICO_DATA_READER)
    if (IsFileExtension(argv[1], ".ico"))
    {
        // Icon data reader
        FILE *icoFile = fopen(argv[1], "rb");

        // NOTE: raylib.ico, in the way it was generated, 256x256 PNG is embedded directly while the
        // resto of image sizes seem to be embedded in an uncompressed form (BMP?)

        if (icoFile == NULL) return -1;

        // Load .ico information
        IcoHeader icoHeader = { 0 };
        fread(&icoHeader, 1, sizeof(IcoHeader), icoFile);

        printf("icoHeader.imageType: %i\n", icoHeader.imageType);
        printf("icoHeader.icoPackCount: %i\n", icoHeader.icoPackCount);

        IcoDirEntry *icoDirEntry = (IcoDirEntry *)calloc(icoHeader.icoPackCount, sizeof(IcoDirEntry));
        unsigned char *icoData[icoHeader.icoPackCount];

        for (int i = 0; i < icoHeader.icoPackCount; i++)
        {
            fread(&icoDirEntry[i], 1, sizeof(IcoDirEntry), icoFile);

            printf("%ix%i@%i - %i - %i\n", icoDirEntry[i].width, icoDirEntry[i].height,
                   icoDirEntry[i].bpp, icoDirEntry[i].size, icoDirEntry[i].offset);
        }

        for (int i = 0; i < icoHeader.icoPackCount; i++)
        {
            icoData[i] = (unsigned char *)malloc(icoDirEntry[i].size);
            fread(icoData[i], icoDirEntry[i].size, 1, icoFile);         // Read icon png data

            Image image = { 0 };

            // Reading png data from memory buffer
            int channels;
            image.data = stbi_load_from_memory(icoData[i], icoDirEntry[i].size, &image.width, &image.height, &channels, 0);

            printf("Read image data from PNG in memory: %ix%i @ %ibpp\n", image.width, image.height, channels*8);

            image.mipmaps =  1;

            if (channels == 1) image.format = UNCOMPRESSED_GRAYSCALE;
            else if (channels == 2) image.format = UNCOMPRESSED_GRAY_ALPHA;
            else if (channels == 3) image.format = UNCOMPRESSED_R8G8B8;
            else if (channels == 4) image.format = UNCOMPRESSED_R8G8B8A8;
            else printf("WARNING: Number of data channels not supported");

            // TODO: Extract icon name from dropped file

            ExportImage(image, FormatText("icon_%ix%i.png", image.width, image.height));

            UnloadImage(image);
        }

        fclose(icoFile);

        for (int i = 0; i < icoHeader.icoPackCount; i++)
        {
            free(icoDirEntry);
            free(icoData[i]);
        }
    }
#endif

#if defined(ICO_DATA_WRITTER)
    // Generate ICO file manually
    char *icoFileNames[MAX_DEFAULT_ICONS];

    // Generate file names to load from expected sizes
    for (int i = 0; i < MAX_DEFAULT_ICONS; i++)
    {
        icoFileNames[i] = (char *)calloc(128, 1);
        sprintf(icoFileNames[i], "%s_%ix%i.png", argv[1], icoSizesPlatform[i], icoSizesPlatform[i]);
    }

    FILE *icoFile = fopen(FormatText("%s.ico", argv[1]), "wb");

    IcoHeader icoHeader = { .reserved = 0, .imageType = 1, .icoPackCount = MAX_DEFAULT_ICONS };
    fwrite(&icoHeader, 1, sizeof(IcoHeader), icoFile);

    IcoDirEntry *icoDirEntry = (IcoDirEntry *)calloc(icoHeader.icoPackCount, sizeof(IcoDirEntry));
    unsigned char *icoData[icoHeader.icoPackCount];
    int offset = 6 + 16*icoHeader.icoPackCount;

    for (int i = 0; i < icoHeader.icoPackCount; i++)
    {
        printf("Reading file: %s\n", icoFileNames[i]);
        FILE *pngFile = fopen(icoFileNames[i], "rb");

        fseek(pngFile, 0, SEEK_END);        // Move file pointer to end of file
        int size = (int)ftell(pngFile);     // Get file size
        fseek(pngFile, 0, SEEK_SET);        // Reset file pointer

        printf("File size: %i\n\n", size);

        icoData[i] = (unsigned char *)malloc(size);
        fread(icoData[i], size, 1, pngFile);   // Read fulll file data
        fclose(pngFile);

        icoDirEntry[i].width = (icoSizesPlatform[i] == 256) ? 0 : icoSizesPlatform[i];
        icoDirEntry[i].height = (icoSizesPlatform[i] == 256) ? 0 : icoSizesPlatform[i];
        icoDirEntry[i].bpp = 32;
        icoDirEntry[i].size = size;
        icoDirEntry[i].offset = offset;

        offset += size;
    }

    for (int i = 0; i < icoHeader.icoPackCount; i++) fwrite(&icoDirEntry[i], 1, sizeof(IcoDirEntry), icoFile);

    for (int i = 0; i < icoHeader.icoPackCount; i++)
    {
        fwrite(icoData[i], 1, icoDirEntry[i].size, icoFile);
        printf("Data written: %i\n", icoDirEntry[i].size);
    }

    fclose(icoFile);

    for (int i = 0; i < MAX_DEFAULT_ICONS; i++) free(icoFileNames[i]);

    for (int i = 0; i < icoHeader.icoPackCount; i++)
    {
        free(icoDirEntry);
        free(icoData[i]);
    }
#endif
*/