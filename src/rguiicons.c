/*******************************************************************************************
*
*   rGuiIcons v1.0 - A simple and easy-to-use raygui icons editor
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
#define RAYGUI_SUPPORT_RICONS
#include "external/raygui.h"            // Required for: IMGUI controls

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define GUI_WINDOW_ABOUT_IMPLEMENTATION
#include "gui_window_about.h"           // GUI: About window

#define GUI_FILE_DIALOGS_IMPLEMENTATION
#include "gui_file_dialogs.h"           // GUI: File Dialog

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
#define MAX_ICONS_Y     16
#define ICON_SIZE       16
#define ICONS_PADDING    2

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// rIcon data
// NOTE: Image format should be GRAYSCALE
typedef struct IconData {
    Image image;
    Color *pixels;
    Texture2D texture;
    unsigned char nameId[64];
} IconData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char *toolName = "rGuiIcons";
const char *toolVersion = "1.0";
const char *toolDescription = "A simple and easy-to-use raygui icons editor";

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
#if defined(VERSION_ONE)
static void ShowCommandLineInfo(void);                      // Show command line usage info
static void ProcessCommandLine(int argc, char *argv[]);     // Process command line input
#endif

// Load/Save/Export data functions
static IconData *LoadIconData(const char *fileName);        // Load raylib icons data (.rgi)


// Auxiliar functions


static IconData *LoadIconsList(Image image, int iconSize, int iconPadding, unsigned int *count);

static unsigned char *ImageToBits(Image image);
static Image ImageFromBits(unsigned char *bytes, int width, int height, Color color);

static Image LoadIconsImage(unsigned int *values, int iconsCount, int iconsPerLine);
static void ExportIconsAsCode(Image image, const char *headerFileName);

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
    char inFileName[512] = { 0 };       // Input file name (required in case of drag & drop over executable)
    char outFileName[512] = { 0 };      // Output file name (required for file save/export)

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
    
#if (!defined(DEBUG) && defined(VERSION_ONE) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)))
    // WARNING (Windows): If program is compiled as Window application (instead of console),
    // no console is available to show output info... solution is compiling a console application
    // and closing console (FreeConsole()) when changing to GUI interface
    FreeConsole();
#endif

    // GUI usage mode - Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 640;
    const int screenHeight = 460;

    InitWindow(screenWidth, screenHeight, FormatText("%s v%s - %s", toolName, toolVersion, toolDescription));
    SetExitKey(0);

    // General pourpose variables
    Vector2 mousePoint = { 0.0f, 0.0f };
    int framesCounter = 0;

    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, FILTER_POINT);
    
    float iconEditScale = 1.0f;     // Icon edit mode scale
    float iconEditOffset = 0.0f;    // Icon edit offset inside box, allow icon movement

    Vector2 cell = { -1, -1 };      // Grid cell mouse position
    int scaledSize = 1.0f;
    float scaleFactor = 1.0f;
    
    // GUI: Full
    //-----------------------------------------------------------------------------------
    Vector2 anchor01 = { 0, 0 };
    
    bool btnLoadPressed = false;
    bool btnSavePressed = false;
    bool btnExportPressed = false;
    bool btnCopyPressed = false;
    bool btnCutPressed = false;
    bool btnPastePressed = false;
    bool btnAboutPressed = false;
    
    int fileTypeActive = 0;
    bool iconNameIdEditMode = false;
    char iconNameIdText[128] = "";
    
    float zoomValue = 0.0f;
    
    bool btnSaveIconPressed = false;
    bool btnClearIconPressed = false;
    //-----------------------------------------------------------------------------------

    // GUI: About Window
    //-----------------------------------------------------------------------------------
    GuiWindowAboutState windowAboutState = InitGuiWindowAbout();
    //-----------------------------------------------------------------------------------
    
    // GUI: Custom file dialogs
    //-----------------------------------------------------------------------------------
    bool showLoadFileDialog = false;
    bool showSaveFileDialog = false;
    bool showExportFileDialog = false;
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
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) showLoadFileDialog = true;      // Show dialog: load icons data (.png)
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) showSaveFileDialog = true;      // Show dialog: save icons data (.png)
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_E)) showExportFileDialog = true;    // Show dialog: export icons data (.h)

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
        
        iconEditScale += (float)GetMouseWheelMove()/10.0f;
        if (iconEditScale < 0.2f) iconEditScale = 0.2f;

        //...

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            // Draw texture to render target
            BeginTextureMode(target);

                ClearBackground(BLANK);     // Clear render target
                
                // GUI: Main toolbar
                //----------------------------------------------------------------------------------
                GuiPanel((Rectangle){ anchor01.x + 0, anchor01.y + 0, 640, 45 });
                
                if (GuiButton((Rectangle){ anchor01.x + 10, anchor01.y + 10, 25, 25 }, "#01#")) showLoadFileDialog = true;
                if (GuiButton((Rectangle){ anchor01.x + 40, anchor01.y + 10, 25, 25 }, "#02#")) showSaveFileDialog = true;
                if (GuiButton((Rectangle){ anchor01.x + 70, anchor01.y + 10, 25, 25 }, "#07#")) showExportFileDialog = true;
                
                if (GuiButton((Rectangle){ anchor01.x + 115, anchor01.y + 10, 25, 25 }, "#16#"))
                {
                    // TODO: Copy selected icon --> int iconToCopy = id;
                }
                
                if (GuiButton((Rectangle){ anchor01.x + 145, anchor01.y + 10, 25, 25 }, "#17#"))
                {
                    // TODO: Cut selected icon --> int iconToCopy = id; ClearIcon(id);
                }
                
                if (GuiButton((Rectangle){ anchor01.x + 175, anchor01.y + 10, 25, 25 }, "#18#"))
                {
                    // TODO: ClearIcon(); Paste iconToCopy
                }
                
                if (GuiButton((Rectangle){ anchor01.x + 260, anchor01.y + 10, 75, 25 }, "#191#ABOUT")) windowAboutState.windowAboutActive = true;
                //----------------------------------------------------------------------------------
                
                // GUI: Work area
                //----------------------------------------------------------------------------------
                GuiLabel((Rectangle){ anchor01.x + 15, anchor01.y + 45, 140, 25 }, "Choose rIcon for Edit:");
                
                // TODO: Draw ricons image (all of them)
                GuiDummyRec((Rectangle){ anchor01.x + 15, anchor01.y + 65, 320, 320 }, "rIcons File");
                
                fileTypeActive = GuiComboBox((Rectangle){ anchor01.x + 15, anchor01.y + 400, 160, 25 }, "rIcons FIle (.rgi);rIcons Image (.png);rIcons Code (.h)", fileTypeActive);
                btnExportPressed = GuiButton((Rectangle){ anchor01.x + 185, anchor01.y + 400, 150, 25 }, "#07#Export rIcons"); 
                
                GuiLabel((Rectangle){ anchor01.x + 365, anchor01.y + 45, 126, 25 }, "rIcon Name ID:");
                if (GuiTextBox((Rectangle){ anchor01.x + 365, anchor01.y + 65, 260, 25 }, iconNameIdText, 128, iconNameIdEditMode)) iconNameIdEditMode = !iconNameIdEditMode;
                
                zoomValue = GuiSliderBar((Rectangle){ anchor01.x + 410, anchor01.y + 110, 180, 10 }, "ZOOM:", TextFormat("x%i", (int)zoomValue*4), zoomValue, 1, 4);
                zoomValue = (float)((int)zoomValue);
                
                // TODO: Draw selected icon at selected scale (x2, x4, x6)
                for (int i = 0; i < ICON_SIZE*ICON_SIZE; i++)
                {
                    DrawRectangle(anchor01.x + 365 + (i%ICON_SIZE)*ICON_SIZE, anchor01.y + 130 + (i/ICON_SIZE)*ICON_SIZE, ICON_SIZE, ICON_SIZE, RED);
                }
               
                // Draw grid (returns selected cell)
                cell = GuiGrid((Rectangle){ anchor01.x + 365, anchor01.y + 130, 256, 256 }, 16, 1);

                // Draw selected cell lines
                if ((cell.x >= 0) && (cell.y >= 0))
                {
                    DrawRectangleLinesEx((Rectangle){ anchor01.x + 135 + 128 - scaledSize/2 + cell.x*scaleFactor, 
                                                      anchor01.y + 10 + 128 - scaledSize/2 + cell.y*scaleFactor, 
                                                      scaleFactor, scaleFactor }, 1, RED);
                }
                
                btnSaveIconPressed = GuiButton((Rectangle){ anchor01.x + 440, anchor01.y + 400, 100, 25 }, "#012#Save Image"); 
                btnClearIconPressed = GuiButton((Rectangle){ anchor01.x + 545, anchor01.y + 400, 80, 25 }, "#079#Clear"); 
                //----------------------------------------------------------------------------------

                // Draw status info
                GuiStatusBar((Rectangle){ anchor01.x + 0, anchor01.y + 435, 350, 25 }, "TOTAL FILE ICONS: 0/0");
                GuiStatusBar((Rectangle){ anchor01.x + 350, anchor01.y + 435, 290, 25 }, "SELECTED: 0/0 - NAME_ID");


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

            EndTextureMode();

            // Draw render texture to screen
            if (false) DrawTexturePro(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Rectangle){ 0, 0, target.texture.width*2, target.texture.height*2 }, (Vector2){ 0, 0 }, 0.0f, WHITE);
            else DrawTextureRec(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Vector2){ 0, 0 }, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
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

// Load raylib icons data (.rgi)
static IconData *LoadIconData(const char *fileName)
{
    IconData *icons = NULL;
    // rgi file should include following information:
    //   - file ID      4 byte
    //   - icons count  2 byte
    //   - icons size   2 byte 
    //
    //   - icons data binary array (unsigned int *) (16x16/32 = 8 int)
    //   - icons name id [64 bytes max]
    
    // Style File Structure (.rgi)
    // ------------------------------------------------------
    // Offset  | Size    | Type       | Description
    // ------------------------------------------------------
    // 0       | 4       | char       | Signature: "rGI "
    // 4       | 2       | short      | Version: 100
    // 6       | 2       | short      | reserved
    
    // 8       | 2       | short      | Num icons (N)
    // 8       | 2       | short      | Size icons (Options: 16, 32, 64)

    // Icons Data: (Icon bitdata: 16*16/8 = 32 bytes + 64 bytes (name) )*N
    // foreach (icon)
    // {
    //   8+8*i  | 2       | int       | ControlId
    //   8+8*i  | 64      | char      | Icon NameId
    // }
    
    return icons;
}

//--------------------------------------------------------------------------------------------
// Auxiliar functions
//--------------------------------------------------------------------------------------------

// Load raylib icons array from image
// NOTE: Images are loaded as GRAY+ALPHA
static IconData *LoadIconsList(Image image, int iconSize, int iconPadding, unsigned int *count)
{
    IconData *icons = NULL;
    
    return icons;
}

// Converts an image to bits array following: Alpha->0, NoAlpha->1
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

// Load GRAY+ALPHA image from and array of bits, stored as int (0-BLANK, 1-WHITE)
static Image LoadIconsImage(unsigned int *values, int iconsCount, int iconsPerLine)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    Image image = { 0 };

    image.width = ICON_SIZE*iconsPerLine;
    image.height = (ICON_SIZE*(1 + (iconsCount/iconsPerLine)));
    image.mipmaps = 1;
    image.format = UNCOMPRESSED_GRAY_ALPHA;
    image.data = (unsigned char *)calloc(image.width*image.height, 2);      // All pixels BLANK by default
    
    printf("Image size: [%i, %i]\n", image.width, image.height);

    int pixelX = 0;
    int pixelY = 0;

    for (int n = 0; n < iconsCount; n++)
    {
        for (int i = 0, y = 0; i < ICON_SIZE*ICON_SIZE/32; i++)
        {
            for (int k = 0; k < 32; k++)
            {
                pixelX = (n%iconsPerLine)*ICON_SIZE + (k%ICON_SIZE);
                pixelY = (n/iconsPerLine)*ICON_SIZE + y;

                if (BIT_CHECK(values[8*n + i], k)) ((unsigned short *)image.data)[pixelY*image.width + pixelX] = 0xffff;    // Draw pixel WHITE

                if ((k == 15) || (k == 31)) y++;
            }
        }
    }

    return image;
}

void ExportIconsAsCode(Image image, const char *headerFileName)
{
    //Image ricons = LoadImage(riconFile);    //ricons.png
    Color *pixels = GetImageData(image);
    
    Rectangle icorec = { 0, 0, ICON_SIZE, ICON_SIZE };
    Color pixel = BLACK;
    
    // Calculate number of bytes required
    int size = MAX_ICONS_X*MAX_ICONS_Y*ICON_SIZE*ICON_SIZE/32;
    unsigned int *values = (unsigned int *)calloc(size, sizeof(unsigned int));
    
    int n = 0;      // Icons counter
    int k = 0;      // Bit counter
    
    for (int y = 0; y < MAX_ICONS_Y; y++)
    {
        for (int x = 0; x < MAX_ICONS_X; x++)
        {
            // Get icon start pixel position within image (top-left corner)
            icorec.x = ICONS_PADDING + x*(ICON_SIZE + 2*ICONS_PADDING);
            icorec.y = ICONS_PADDING + y*(ICON_SIZE + 2*ICONS_PADDING);

            // Move along pixels within each icon area
            for (int p = 0; p < ICON_SIZE*ICON_SIZE; p++)
            {
                pixel = pixels[((int)icorec.y + p/ICON_SIZE)*(MAX_ICONS_X*(ICON_SIZE + 2*ICONS_PADDING)) + ((int)icorec.x + p%ICON_SIZE)];
                
                if (ColorToInt(pixel) == 0xffffffff) BIT_SET(values[n*(ICON_SIZE*ICON_SIZE/32) + p/32], k);
                
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
    
    for (int i = 0, y = 0; i < ICON_SIZE*ICON_SIZE/32; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            if (BIT_CHECK(RICONS[8*iconId + i], k)) DrawRectangle(position.x + (k%ICON_SIZE)*pixelSize, position.y + y*pixelSize, pixelSize, pixelSize, color);
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