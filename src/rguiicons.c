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

// NOTE: Some raygui elements need to be defined before including raygui
#define TEXTSPLIT_MAX_TEXT_LENGTH      4096
#define TEXTSPLIT_MAX_TEXT_ELEMENTS     256
#define TOGGLEGROUP_MAX_ELEMENTS        256
#define GRID_COLOR_ALPHA                0.2f

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

#define ICON_PADDING    2       // Default icon padding

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char *toolName = "rGuiIcons";
const char *toolVersion = "1.0";
const char *toolDescription = "A simple and easy-to-use raygui icons editor";

static bool saveChangesRequired = false;    // Flag to notice save changes are required

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
#if defined(VERSION_ONE)
static void ShowCommandLineInfo(void);                      // Show command line usage info
static void ProcessCommandLine(int argc, char *argv[]);     // Process command line input
#endif

// Load/Save/Export data functions
static bool SaveIcons(const char *fileName, int format);    // Save raygui icons file (.rgi)
static void ExportIconsAsCode2(const char *fileName);       // Export gui icons as code

// Auxiliar functions
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

    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, FILTER_POINT);
    

    Vector2 cell = { -1, -1 };  // Grid cell mouse position
    int iconEditScale = 16;     // Icon edit scale
    
    // GUI: Full
    //-----------------------------------------------------------------------------------
    Vector2 anchor01 = { 0, 0 };
    
    int fileTypeActive = 0;
    bool iconNameIdEditMode = false;
    char iconNameIdText[128] = "";
    
    float zoomValue = 0.0f;
    
    int selectedIcon = 0;
    
    // ToggleGroup() text
    // NOTE: Every icon requires 6 text characters: "#001#;"
    char toggleIconsText[MAX_RICONS*6] = { 0 };
    
    for (int i = 0; i < MAX_RICONS; i++)
    {
        if ((i + 1)%16 == 0) strncpy(toggleIconsText + 6*i, TextFormat("#%03i#\n", i), 6);
        else strncpy(toggleIconsText + 6*i, TextFormat("#%03i#;", i), 6);
    }
    
    toggleIconsText[MAX_RICONS*6 - 1] = '\0';
    
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
    
    bool showSaveImageDialog = false;
    //-----------------------------------------------------------------------------------
    
    // GUI: Exit Window
    //-----------------------------------------------------------------------------------
    bool exitWindow = false;
    bool windowExitActive = false;
    //-----------------------------------------------------------------------------------

    // Check if an input file has been provided on command line
    if ((inFileName[0] != '\0') && (IsFileExtension(inFileName, ".rgi")))
    {
        GuiLoadIcons(inFileName);
        SetWindowTitle(FormatText("%s v%s - %s", toolName, toolVersion, GetFileName(inFileName)));
    }

    unsigned int iconData[8] = { 0 };
    bool iconDataToCopy = false;

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

            if (IsFileExtension(droppedFiles[0], ".rgi"))
            {
                // Load .rgi data into raygui icons set
                GuiLoadIcons(droppedFiles[0]);

                strcpy(inFileName, droppedFiles[0]);
                SetWindowTitle(FormatText("%s v%s - %s", toolName, toolVersion, GetFileName(inFileName)));
            }
#if defined(VERSION_ONE)
            else if (IsFileExtension(droppedFiles[0], ".rgs")) GuiLoadStyle(droppedFiles[0]);
#endif

            ClearDroppedFiles();
        }
        //----------------------------------------------------------------------------------

        // Keyboard shortcuts
        //------------------------------------------------------------------------------------
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) showLoadFileDialog = true;      // Show dialog: load icons data (.rgi)
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) showSaveFileDialog = true;      // Show dialog: save icons data (.rgi)
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_E)) showExportFileDialog = true;    // Show dialog: export icons data (.png, .h)

        if (IsKeyPressed(KEY_F1)) windowAboutState.windowAboutActive = !windowAboutState.windowAboutActive;
        //----------------------------------------------------------------------------------

        // Basic program flow logic
        //----------------------------------------------------------------------------------
        mousePoint = GetMousePosition();    // Get mouse position each frame
        
        if (WindowShouldClose()) exitWindow = true;

        // Show closing window on ESC
        if (IsKeyPressed(KEY_ESCAPE))
        {
            // TODO: Define KEY_ESCAPE custom logic (i.e. Show save dialog)
            exitWindow = true;
        }

        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) ToggleFullscreen();
        
        
        
        
        
        
        iconEditScale += GetMouseWheelMove();
        if (iconEditScale < 2) iconEditScale = 2;
        else if (iconEditScale > 16) iconEditScale = 16;
        zoomValue = iconEditScale;
        
        // Security check to avoid cells out of limits
        if (cell.x > (RICON_SIZE - 1)) cell.x = RICON_SIZE - 1;
        if (cell.y > (RICON_SIZE - 1)) cell.y = RICON_SIZE - 1;

        // Icon painting mouse logic
        if ((cell.x >= 0) && (cell.y >= 0) && (cell.x < RICON_SIZE) && (cell.y < RICON_SIZE))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) GuiSetIconPixel(selectedIcon, (int)cell.x, (int)cell.y);
            else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) GuiClearIconPixel(selectedIcon, (int)cell.x, (int)cell.y);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            // Draw texture to render target
            BeginTextureMode(target);

                ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
                
                if (windowAboutState.windowAboutActive || windowExitActive) GuiDisable();
                else GuiEnable();
                
                // GUI: Main toolbar
                //----------------------------------------------------------------------------------
                GuiPanel((Rectangle){ anchor01.x + 0, anchor01.y + 0, 640, 45 });
                
                if (GuiButton((Rectangle){ anchor01.x + 10, anchor01.y + 10, 25, 25 }, "#01#")) showLoadFileDialog = true;
                if (GuiButton((Rectangle){ anchor01.x + 40, anchor01.y + 10, 25, 25 }, "#02#")) showSaveFileDialog = true;
                if (GuiButton((Rectangle){ anchor01.x + 70, anchor01.y + 10, 25, 25 }, "#07#")) showExportFileDialog = true;
                
                if (GuiButton((Rectangle){ anchor01.x + 115, anchor01.y + 10, 25, 25 }, "#16#"))
                {
                    memcpy(iconData, GuiGetIconData(selectedIcon), RICON_DATA_ELEMENTS*sizeof(unsigned int));
                    iconDataToCopy = true;
                }

                if (GuiButton((Rectangle){ anchor01.x + 145, anchor01.y + 10, 25, 25 }, "#17#"))
                {
                    memcpy(iconData, GuiGetIconData(selectedIcon), RICON_DATA_ELEMENTS*sizeof(unsigned int));
                    for (int i = 0; i < RICON_SIZE*RICON_SIZE; i++) GuiClearIconPixel(selectedIcon, i/RICON_SIZE, i%RICON_SIZE);
                    iconDataToCopy = true;
                }
                
                if (GuiButton((Rectangle){ anchor01.x + 175, anchor01.y + 10, 25, 25 }, "#18#"))
                {
                    if (iconDataToCopy) GuiSetIconData(selectedIcon, iconData);
                    iconDataToCopy = false;
                }
                
                if (GuiButton((Rectangle){ anchor01.x + 260, anchor01.y + 10, 75, 25 }, "#191#ABOUT")) windowAboutState.windowAboutActive = true;
                //----------------------------------------------------------------------------------
                
                // GUI: Work area
                //----------------------------------------------------------------------------------
                GuiLabel((Rectangle){ anchor01.x + 15, anchor01.y + 45, 140, 25 }, "Choose rIcon for Edit:");
                
                // Draw ricons selection panel
                selectedIcon = GuiToggleGroup((Rectangle){ anchor01.x + 15, anchor01.y + 70, 18, 18 }, toggleIconsText, selectedIcon);
                
                fileTypeActive = GuiComboBox((Rectangle){ anchor01.x + 15, anchor01.y + 400, 160, 25 }, "rIcons File (.rgi);rIcons Image (.png);rIcons Code (.h)", fileTypeActive);
                if (GuiButton((Rectangle){ anchor01.x + 185, anchor01.y + 400, 150, 25 }, "#07#Export rIcons")) showExportFileDialog = true;
                
                GuiLabel((Rectangle){ anchor01.x + 365, anchor01.y + 45, 126, 25 }, "rIcon Name ID:");
                if (GuiTextBox((Rectangle){ anchor01.x + 365, anchor01.y + 70, 260, 25 }, iconNameIdText, 128, iconNameIdEditMode)) iconNameIdEditMode = !iconNameIdEditMode;
                
                zoomValue = GuiSliderBar((Rectangle){ anchor01.x + 410, anchor01.y + 110, 180, 10 }, "ZOOM:", TextFormat("x%i", (int)zoomValue), zoomValue, 0, 16);

                // Draw selected icon at selected scale
                DrawRectangle(anchor01.x + 365, anchor01.y + 130, 256, 256, Fade(GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)), 0.3f));
                GuiDrawIcon(selectedIcon, (Vector2){ anchor01.x + 365 + 128 - RICON_SIZE*iconEditScale/2, anchor01.y + 130 + 128 - RICON_SIZE*iconEditScale/2 }, iconEditScale, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
               
                // Draw grid (returns selected cell)
                cell = GuiGrid((Rectangle){ anchor01.x + 365 + 128 - RICON_SIZE*iconEditScale/2, anchor01.y + 130 + 128 - RICON_SIZE*iconEditScale/2, RICON_SIZE*iconEditScale, RICON_SIZE*iconEditScale }, iconEditScale, 1);
                
                // TODO: Review returned cell, sometimes out-of-bounds

                // Draw selected cell lines
                if ((cell.x >= 0) && (cell.y >= 0) && (cell.x < RICON_SIZE) && (cell.y < RICON_SIZE))
                {
                    DrawRectangleLinesEx((Rectangle){ anchor01.x + 365 + iconEditScale*cell.x + 128 - RICON_SIZE*iconEditScale/2, 
                                                      anchor01.y + 130 + iconEditScale*cell.y + 128 - RICON_SIZE*iconEditScale/2, 
                                                      iconEditScale + 1, iconEditScale + 1 }, 1, RED);
                }
                
                if (GuiButton((Rectangle){ anchor01.x + 440, anchor01.y + 400, 100, 25 }, "#012#Save Image")) showSaveImageDialog = true;
                
                if (GuiButton((Rectangle){ anchor01.x + 545, anchor01.y + 400, 80, 25 }, "#079#Clear"))
                {
                    for (int i = 0; i < RICON_SIZE*RICON_SIZE; i++) GuiClearIconPixel(selectedIcon, i/RICON_SIZE, i%RICON_SIZE);
                }
                //----------------------------------------------------------------------------------

                // Draw status info
                GuiStatusBar((Rectangle){ anchor01.x + 0, anchor01.y + 435, 351, 25 }, "TOTAL FILE ICONS: 0/0");
                GuiStatusBar((Rectangle){ anchor01.x + 350, anchor01.y + 435, 290, 25 }, TextFormat("SELECTED: %i/256 - NAME_ID", selectedIcon));


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
    printf("// %s v%s ONE - %s            //\n", toolName, toolVersion, toolDescription);
    printf("// powered by raylib v2.6 (www.raylib.com) and raygui v2.6                      //\n");
    printf("// more info and bugs-report: github.com/raylibtech/rtools                      //\n");
    printf("// feedback and support:      ray[at]raylibtech.com                             //\n");
    printf("//                                                                              //\n");
    printf("// Copyright (c) 2019 raylib technologies (@raylibtech)                         //\n");
    printf("//                                                                              //\n");
    printf("//////////////////////////////////////////////////////////////////////////////////\n\n");

    printf("USAGE:\n\n");
    printf("    > rguiicons [--help] --input <filename.ext> [--output <filename.ext>]\n");

    printf("\nOPTIONS:\n\n");
    printf("    -h, --help                      : Show tool version and command line usage help\n");
    printf("    -i, --input <filename.ext>      : Define input file.\n");
    printf("                                      Supported extensions: .rgi\n");
    printf("    -o, --output <filename.ext>     : Define output file.\n");
    printf("                                      Supported extensions: .rgi, .png, .h\n");
    printf("                                      NOTE: If not specified, defaults to: output.rgi\n\n");

    printf("\nEXAMPLES:\n\n");
    printf("    > rguiicons --input icons.rgi --output icons.png\n");
    printf("        Process <icons.rgi> to generate <icons.png>\n\n");
    printf("    > rguiicons --input icons.rgi --output ricons.h\n");
    printf("        Process <icons.rgi> to generate <ricons.h>\n\n");
}

// Process command line input
static void ProcessCommandLine(int argc, char *argv[])
{
    // CLI required variables
    bool showUsageInfo = false;     // Toggle command line usage info

    char inFileName[512] = { 0 };   // Input file name
    char outFileName[512] = { 0 };  // Output file name
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
                if (IsFileExtension(argv[i + 1], ".rgi"))
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
                if (IsFileExtension(argv[i + 1], ".rgi") ||
                    IsFileExtension(argv[i + 1], ".png") ||
                    IsFileExtension(argv[i + 1], ".h"))
                {
                    strcpy(outFileName, argv[i + 1]);   // Read output filename
                }
                else printf("WARNING: Output file extension not recognized.\n");
                
                i++;
            }
            else printf("WARNING: No output file provided\n");
        }
    }

    // Process input file if provided
    if (inFileName[0] != '\0')
    {
        // Set a default name for output in case not provided
        if (outFileName[0] == '\0') strcpy(outFileName, "output.rgi");

        printf("\nInput file:       %s", inFileName);
        printf("\nOutput file:      %s", outFileName);

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

void ExportIconsAsCode(Image image, const char *headerFileName)
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
            icorec.x = ICON_PADDING + x*(RICON_SIZE + 2*ICON_PADDING);
            icorec.y = ICON_PADDING + y*(RICON_SIZE + 2*ICON_PADDING);

            // Move along pixels within each icon area
            for (int p = 0; p < RICON_SIZE*RICON_SIZE; p++)
            {
                pixel = pixels[((int)icorec.y + p/RICON_SIZE)*(MAX_ICONS_X*(RICON_SIZE + 2*ICON_PADDING)) + ((int)icorec.x + p%RICON_SIZE)];
                
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
