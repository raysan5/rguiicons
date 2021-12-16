/*******************************************************************************************
*
*   rGuiIcons v1.5 - A simple and easy-to-use raygui icons editor
*
*   CONFIGURATION:
*
*   #define VERSION_ONE
*       Enable PRO features for the tool:
*       - Support command line usage
*
*   #define CUSTOM_MODAL_DIALOGS
*       Use custom raygui generated modal dialogs instead of native OS ones
*       NOTE: Avoids including tinyfiledialogs depencency library
*
*   VERSIONS HISTORY:
*       1.5  (xx-Nov-2021) Updated to raylib 4.0 and raygui 3.0
*       1.0  (30-Sep-2019) First release
*
*   DEPENDENCIES:
*       raylib 4.0              - Windowing/input management and drawing.
*       raygui 3.0              - Immediate-mode GUI controls.
*       tinyfiledialogs 3.8.8   - Open/save file dialogs, it requires linkage with comdlg32 and ole32 libs.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o rguiicons.exe rguiicons.c external/tinyfiledialogs.c -s rguiicons.rc.data -Iexternal /
*           -lraylib -lopengl32 -lgdi32 -lcomdlg32 -lole32 -std=c99 -Wall
*
*   COMPILATION (Linux - GCC):
*       gcc -o rguiicons rguiicons.c external/tinyfiledialogs.c -s -Iexternal -no-pie -D_DEFAULT_SOURCE /
*           -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
*
*   NOTE: On PLATFORM_ANDROID and PLATFORM_WEB file dialogs are not available
*
*   DEVELOPERS:
*       Ramon Santamaria (@raysan5):   Developer, supervisor, updater and maintainer.
*
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2019-2021 raylib technologies (@raylibtech). All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

#define TOOL_NAME               "rGuiIcons"
#define TOOL_SHORT_NAME         "rGI"
#define TOOL_VERSION            "1.5"
#define TOOL_DESCRIPTION        "A simple and easy-to-use raygui icons editor"
#define TOOL_RELEASE_DATE       "Dec.2021"
#define TOOL_LOGO_COLOR         0x48c9c5ff

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS        // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>  // Emscripten library - LLVM to JavaScript compiler
#endif

// NOTE: Some raygui elements need to be defined before including raygui
#define TEXTSPLIT_MAX_TEXT_LENGTH      4096
#define TEXTSPLIT_MAX_TEXT_ELEMENTS     256
#define TOGGLEGROUP_MAX_ELEMENTS        256
#define GRID_COLOR_ALPHA                  0.2f

#define RAYGUI_IMPLEMENTATION
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

#define MAX_UNDO_LEVELS         10      // Undo levels supported for the ring buffer

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Gui icon type (one icon info)
// NOTE: This structure is not used at the moment,
// All icons are joined in a single guiIcons[] array
// and icons name id is actually defined as enum values
typedef struct GuiIcon {
    unsigned int data[RICON_DATA_ELEMENTS];
    unsigned char nameId[RICON_MAX_NAME_LENGTH];
} GuiIcon;

// Full icons set
typedef struct GuiIconSet {
    unsigned int count;
    unsigned int iconSize;
    unsigned int *values;
} GuiIconSet;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char *toolName = TOOL_NAME;
const char *toolVersion = TOOL_VERSION;
const char *toolDescription = TOOL_DESCRIPTION;

static bool saveChangesRequired = false;    // Flag to notice save changes are required

// Icons name id text
static char guiIconsName[RICON_MAX_ICONS][32] = {
    "NONE",
    "FOLDER_FILE_OPEN",
    "FILE_SAVE_CLASSIC",
    "FOLDER_OPEN",
    "FOLDER_SAVE",
    "FILE_OPEN",
    "FILE_SAVE",
    "FILE_EXPORT",
    "FILE_NEW",
    "FILE_DELETE",
    "FILETYPE_TEXT",
    "FILETYPE_AUDIO",
    "FILETYPE_IMAGE",
    "FILETYPE_PLAY",
    "FILETYPE_VIDEO",
    "FILETYPE_INFO",
    "FILE_COPY",
    "FILE_CUT",
    "FILE_PASTE",
    "CURSOR_HAND",
    "CURSOR_POINTER",
    "CURSOR_CLASSIC",
    "PENCIL",
    "PENCIL_BIG",
    "BRUSH_CLASSIC",
    "BRUSH_PAINTER",
    "WATER_DROP",
    "COLOR_PICKER",
    "RUBBER",
    "COLOR_BUCKET",
    "TEXT_T",
    "TEXT_A",
    "SCALE",
    "RESIZE",
    "FILTER_POINT",
    "FILTER_BILINEAR",
    "CROP",
    "CROP_ALPHA",
    "SQUARE_TOGGLE",
    "SIMMETRY",
    "SIMMETRY_HORIZONTAL",
    "SIMMETRY_VERTICAL",
    "LENS",
    "LENS_BIG",
    "EYE_ON",
    "EYE_OFF",
    "FILTER_TOP",
    "FILTER",
    "TARGET_POINT",
    "TARGET_SMALL",
    "TARGET_BIG",
    "TARGET_MOVE",
    "CURSOR_MOVE",
    "CURSOR_SCALE",
    "CURSOR_SCALE_RIGHT",
    "CURSOR_SCALE_LEFT",
    "UNDO",
    "REDO",
    "REREDO",
    "MUTATE",
    "ROTATE",
    "REPEAT",
    "SHUFFLE",
    "EMPTYBOX",
    "TARGET",
    "TARGET_SMALL_FILL",
    "TARGET_BIG_FILL",
    "TARGET_MOVE_FILL",
    "CURSOR_MOVE_FILL",
    "CURSOR_SCALE_FILL",
    "CURSOR_SCALE_RIGHT_FILL",
    "CURSOR_SCALE_LEFT_FILL",
    "UNDO_FILL",
    "REDO_FILL",
    "REREDO_FILL",
    "MUTATE_FILL",
    "ROTATE_FILL",
    "REPEAT_FILL",
    "SHUFFLE_FILL",
    "EMPTYBOX_SMALL",
    "BOX",
    "BOX_TOP",
    "BOX_TOP_RIGHT",
    "BOX_RIGHT",
    "BOX_BOTTOM_RIGHT",
    "BOX_BOTTOM",
    "BOX_BOTTOM_LEFT",
    "BOX_LEFT",
    "BOX_TOP_LEFT",
    "BOX_CENTER",
    "BOX_CIRCLE_MASK",
    "POT",
    "ALPHA_MULTIPLY",
    "ALPHA_CLEAR",
    "DITHERING",
    "MIPMAPS",
    "BOX_GRID",
    "GRID",
    "BOX_CORNERS_SMALL",
    "BOX_CORNERS_BIG",
    "FOUR_BOXES",
    "GRID_FILL",
    "BOX_MULTISIZE",
    "ZOOM_SMALL",
    "ZOOM_MEDIUM",
    "ZOOM_BIG",
    "ZOOM_ALL",
    "ZOOM_CENTER",
    "BOX_DOTS_SMALL",
    "BOX_DOTS_BIG",
    "BOX_CONCENTRIC",
    "BOX_GRID_BIG",
    "OK_TICK",
    "CROSS",
    "ARROW_LEFT",
    "ARROW_RIGHT",
    "ARROW_BOTTOM",
    "ARROW_TOP",
    "ARROW_LEFT_FILL",
    "ARROW_RIGHT_FILL",
    "ARROW_BOTTOM_FILL",
    "ARROW_TOP_FILL",
    "AUDIO",
    "FX",
    "WAVE",
    "WAVE_SINUS",
    "WAVE_SQUARE",
    "WAVE_TRIANGULAR",
    "CROSS_SMALL",
    "PLAYER_PREVIOUS",
    "PLAYER_PLAY_BACK",
    "PLAYER_PLAY",
    "PLAYER_PAUSE",
    "PLAYER_STOP",
    "PLAYER_NEXT",
    "PLAYER_RECORD",
    "MAGNET",
    "LOCK_CLOSE",
    "LOCK_OPEN",
    "CLOCK",
    "TOOLS",
    "GEAR",
    "GEAR_BIG",
    "BIN",
    "HAND_POINTER",
    "LASER",
    "COIN",
    "EXPLOSION",
    "1UP",
    "PLAYER",
    "PLAYER_JUMP",
    "KEY",
    "DEMON",
    "TEXT_POPUP",
    "GEAR_EX",
    "CRACK",
    "CRACK_POINTS",
    "STAR",
    "DOOR",
    "EXIT",
    "MODE_2D",
    "MODE_3D",
    "CUBE",
    "CUBE_FACE_TOP",
    "CUBE_FACE_LEFT",
    "CUBE_FACE_FRONT",
    "CUBE_FACE_BOTTOM",
    "CUBE_FACE_RIGHT",
    "CUBE_FACE_BACK",
    "CAMERA",
    "SPECIAL",
    "LINK_NET",
    "LINK_BOXES",
    "LINK_MULTI",
    "LINK",
    "LINK_BROKE",
    "TEXT_NOTES",
    "NOTEBOOK",
    "SUITCASE",
    "SUITCASE_ZIP",
    "MAILBOX",
    "MONITOR",
    "PRINTER",
    "PHOTO_CAMERA",
    "PHOTO_CAMERA_FLASH",
    "HOUSE",
    "HEART",
    "CORNER",
    "VERTICAL_BARS",
    "VERTICAL_BARS_FILL",
    "LIFE_BARS",
    "INFO",
    "CROSSLINE",
    "HELP",
    "FILETYPE_ALPHA",
    "FILETYPE_HOME",
    "LAYERS_VISIBLE",
    "LAYERS",
    "WINDOW"
};

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
#if defined(VERSION_ONE)
// Command line functionality
static void ShowCommandLineInfo(void);                      // Show command line usage info
static void ProcessCommandLine(int argc, char *argv[]);     // Process command line input
#endif

// Load/Save/Export data functions
static void LoadIconsFromImage(Image image, int iconCount, int iconSize, int iconsPerLine, int padding); // Load icons from image file
static bool SaveIcons(const char *fileName);                // Save raygui icons file (.rgi)
static void ExportIconsAsCode(const char *fileName);        // Export gui icons as code (.h)

// Auxiliar functions
static void DrawIconData(unsigned int *data, int x, int y, int pixelSize, Color color);                 // Draw icon data (one icon)

static Image ImageFromIconData(unsigned int *values, int iconCount, int iconsPerLine, int padding);    // Gen image drom icon data array

static unsigned char *ImageToBits(Image image);
static Image ImageFromBits(unsigned char *bytes, int width, int height, Color color);

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

    InitWindow(screenWidth, screenHeight, TextFormat("%s v%s | %s", toolName, toolVersion, toolDescription));
    SetExitKey(0);

    // General pourpose variables
    Vector2 mousePoint = { 0.0f, 0.0f };

    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    int screenScale = 1;

    Vector2 cell = { -1, -1 };  // Grid cell mouse position
    int iconEditScale = 16;     // Icon edit scale

    // GUI: Full
    //-----------------------------------------------------------------------------------
    Vector2 anchor01 = { 0, 0 };

    int fileTypeActive = 0;
    bool iconNameIdEditMode = false;

    int selectedIcon = 0;

    // ToggleGroup() text
    // NOTE: Every icon requires 6 text characters: "#001#;"
    char toggleIconsText[RICON_MAX_ICONS*6] = { 0 };

    for (int i = 0; i < RICON_MAX_ICONS; i++)
    {
        if ((i + 1)%16 == 0) strncpy(toggleIconsText + 6*i, TextFormat("#%03i#\n", i), 6);
        else strncpy(toggleIconsText + 6*i, TextFormat("#%03i#;", i), 6);
    }

    toggleIconsText[RICON_MAX_ICONS*6 - 1] = '\0';

    bool hiDpiActive = false;
    bool prevHiDpiActive = hiDpiActive;

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

    bool showExportIconImageDialog = false;
    //-----------------------------------------------------------------------------------

    // GUI: Exit Window
    //-----------------------------------------------------------------------------------
    bool exitWindow = false;
    bool windowExitActive = false;
    //-----------------------------------------------------------------------------------

    // Check if an input file has been provided on command line
    if ((inFileName[0] != '\0') && (IsFileExtension(inFileName, ".rgi")))
    {
        char **tempIconsName = GuiLoadIcons(inFileName, true);
        for (int i = 0; i < RICON_MAX_ICONS; i++) { strcpy(guiIconsName[i], tempIconsName[i]); free(tempIconsName[i]); }
        free(tempIconsName);

        SetWindowTitle(TextFormat("%s v%s - %s", toolName, toolVersion, GetFileName(inFileName)));
    }

    unsigned int iconData[8] = { 0 };
    char iconName[32] = { 0 };
    bool iconDataToCopy = false;

    // Undo system variables
    int currentUndoIndex = 0;
    int firstUndoIndex = 0;
    int lastUndoIndex = 0;
    int undoFrameCounter = 0;
    GuiIconSet *undoIconSet = (GuiIconSet *)calloc(MAX_UNDO_LEVELS, sizeof(GuiIconSet));

    // Init undo system with current icons set
    for (int i = 0; i < MAX_UNDO_LEVELS; i++)
    {
        undoIconSet[i].count = RICON_MAX_ICONS;
        undoIconSet[i].iconSize = RICON_SIZE;
        undoIconSet[i].values = (unsigned int *)calloc(RICON_MAX_ICONS*RICON_DATA_ELEMENTS, sizeof(unsigned int));

        memcpy(undoIconSet[i].values, GuiGetIcons(), RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int));
    }

    SetTargetFPS(60);
    //------------------------------------------------------------

    // Main game loop
    while (!exitWindow)             // Detect window close button
    {
        // Undo icons change logic
        //----------------------------------------------------------------------------------
        // Make sure no windows are open to store changes
        if (!windowAboutState.windowActive && !windowExitActive && !showLoadFileDialog &&
            !showSaveFileDialog && !showExportFileDialog && !showExportIconImageDialog)
        {
            undoFrameCounter++;

            // Every 120 frames we check if current layout has changed and record a new undo state
            if (undoFrameCounter >= 120)
            {
                if (memcmp(undoIconSet[currentUndoIndex].values, GuiGetIcons(), RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int)) != 0)
                {
                    // Move cursor to next available position to record undo
                    currentUndoIndex++;
                    if (currentUndoIndex >= MAX_UNDO_LEVELS) currentUndoIndex = 0;
                    if (currentUndoIndex == firstUndoIndex) firstUndoIndex++;
                    if (firstUndoIndex >= MAX_UNDO_LEVELS) firstUndoIndex = 0;

                    memcpy(undoIconSet[currentUndoIndex].values, GuiGetIcons(), RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int));

                    lastUndoIndex = currentUndoIndex;

                    // Set a '*' mark on loaded file name to notice save requirement
                    if ((inFileName[0] != '\0') && !saveChangesRequired)
                    {
                        SetWindowTitle(TextFormat("%s v%s - %s*", toolName, toolVersion, GetFileName(inFileName)));
                        saveChangesRequired = true;
                    }
                }

                undoFrameCounter = 0;
            }
        }
        else undoFrameCounter = 120;

        // Recover previous layout state from buffer
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z))
        {
            if (currentUndoIndex != firstUndoIndex)
            {
                currentUndoIndex--;
                if (currentUndoIndex < 0) currentUndoIndex = MAX_UNDO_LEVELS - 1;

                if (memcmp(undoIconSet[currentUndoIndex].values, GuiGetIcons(), RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int)) != 0)
                {
                    // Restore previous icons state
                    // NOTE: GuiGetIcons() returns a pointer to internal data
                    memcpy(GuiGetIcons(), undoIconSet[currentUndoIndex].values,  RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int));
                }
            }
        }

        // Recover next layout state from buffer
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y))
        {
            if (currentUndoIndex != lastUndoIndex)
            {
                int nextUndoIndex = currentUndoIndex + 1;
                if (nextUndoIndex >= MAX_UNDO_LEVELS) nextUndoIndex = 0;

                if (nextUndoIndex != firstUndoIndex)
                {
                    currentUndoIndex = nextUndoIndex;

                    if (memcmp(undoIconSet[currentUndoIndex].values, GuiGetIcons(), RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int)) != 0)
                    {
                        memcpy(GuiGetIcons(), undoIconSet[currentUndoIndex].values,  RICON_MAX_ICONS*RICON_DATA_ELEMENTS*sizeof(unsigned int));
                    }
                }
            }
        }
        //----------------------------------------------------------------------------------

        // Dropped files logic
        //----------------------------------------------------------------------------------
        if (IsFileDropped())
        {
            int dropFileCount = 0;
            char **droppedFiles = GetDroppedFiles(&dropFileCount);

            if (IsFileExtension(droppedFiles[0], ".rgi"))
            {
                // Load .rgi data into raygui icons set (and gui icon names for the tool)
                char **tempIconsName = GuiLoadIcons(droppedFiles[0], true);
                for (int i = 0; i < RICON_MAX_ICONS; i++) { strcpy(guiIconsName[i], tempIconsName[i]); free(tempIconsName[i]); }
                free(tempIconsName);

                strcpy(inFileName, droppedFiles[0]);
                SetWindowTitle(TextFormat("%s v%s - %s", toolName, toolVersion, GetFileName(inFileName)));
            }
            else if (IsFileExtension(droppedFiles[0], ".png"))
            {
                // TODO: Support icons loading from image
                //Image image = LoadImage(droppedFiles[0]);
                //LoadIconsFromImage(image, RICON_MAX_ICONS, 16, 16, 1);    // Loading window required to config parameters (similar to raw)
                //UnloadImage(image);

                // TODO: Load icons name id from PNG zTXt chunk if available
            }
            else if (IsFileExtension(droppedFiles[0], ".rgs")) GuiLoadStyle(droppedFiles[0]);

            ClearDroppedFiles();
        }
        //----------------------------------------------------------------------------------

        // Keyboard shortcuts
        //------------------------------------------------------------------------------------
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) showLoadFileDialog = true;      // Show dialog: load icons data (.rgi)
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            if (inFileName[0] == '\0') showSaveFileDialog = true;       // Show dialog: save icons data (.rgi)
            else if (saveChangesRequired)
            {
                SaveIcons(inFileName);
                SetWindowTitle(TextFormat("%s v%s - %s", toolName, toolVersion, GetFileName(inFileName)));
                saveChangesRequired = false;
            }
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_E)) showExportFileDialog = true;    // Show dialog: export icons data (.png, .h)

        if (IsKeyPressed(KEY_F1)) windowAboutState.windowActive = !windowAboutState.windowActive;
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

        if (IsKeyPressed(KEY_DELETE))
        {
            for (int i = 0; i < RICON_SIZE*RICON_SIZE; i++) GuiClearIconPixel(selectedIcon, i/RICON_SIZE, i%RICON_SIZE);

            memset(guiIconsName[selectedIcon], 0, 32);
        }

        iconEditScale += GetMouseWheelMove();
        if (iconEditScale < 2) iconEditScale = 2;
        else if (iconEditScale > 16) iconEditScale = 16;

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

        // Screen scale logic (x2)
        //----------------------------------------------------------------------------------
        if (hiDpiActive != prevHiDpiActive)
        {
            if (hiDpiActive)
            {
                screenScale = 2;
                SetWindowSize(screenWidth*2, screenHeight*2);
                SetMouseScale(0.5f, 0.5f);
            }
            else
            {
                screenScale = 1;
                SetWindowSize(screenWidth, screenHeight);
                SetMouseScale(1.0f, 1.0f);
            }

            prevHiDpiActive = hiDpiActive;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            // Draw texture to render target
            BeginTextureMode(target);

                ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

                if (windowAboutState.windowActive || windowExitActive) GuiDisable();
                else GuiEnable();

                // GUI: Main toolbar
                //----------------------------------------------------------------------------------
                GuiPanel((Rectangle){ anchor01.x + 0, anchor01.y + 0, 640, 45 });

                if (GuiButton((Rectangle){ anchor01.x + 10, anchor01.y + 10, 25, 25 }, "#01#")) showLoadFileDialog = true;
                if (GuiButton((Rectangle){ anchor01.x + 40, anchor01.y + 10, 25, 25 }, "#02#")) showSaveFileDialog = true;
                if (GuiButton((Rectangle){ anchor01.x + 70, anchor01.y + 10, 25, 25 }, "#07#")) showExportFileDialog = true;

                // Copy button/shortcut logic
                if ((GuiButton((Rectangle){ anchor01.x + 115, anchor01.y + 10, 25, 25 }, "#16#")) ||
                    (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)))
                {
                    memcpy(iconData, GuiGetIconData(selectedIcon), RICON_DATA_ELEMENTS*sizeof(unsigned int));
                    strcpy(iconName, guiIconsName[selectedIcon]);
                    iconDataToCopy = true;
                }

                // Cut button/shortcut logic
                if ((GuiButton((Rectangle){ anchor01.x + 145, anchor01.y + 10, 25, 25 }, "#17#")) ||
                    (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_X)))
                {
                    memcpy(iconData, GuiGetIconData(selectedIcon), RICON_DATA_ELEMENTS*sizeof(unsigned int));
                    for (int i = 0; i < RICON_SIZE*RICON_SIZE; i++) GuiClearIconPixel(selectedIcon, i/RICON_SIZE, i%RICON_SIZE);

                    strcpy(iconName, guiIconsName[selectedIcon]);
                    memset(guiIconsName[selectedIcon], 0, 32);

                    iconDataToCopy = true;
                }

                // Paste button/shortcut logic
                if ((GuiButton((Rectangle){ anchor01.x + 175, anchor01.y + 10, 25, 25 }, "#18#")) ||
                    (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)))
                {
                    if (iconDataToCopy)
                    {
                        GuiSetIconData(selectedIcon, iconData);
                        strcpy(guiIconsName[selectedIcon], iconName);
                    }
                }

                GuiGroupBox((Rectangle){ anchor01.x + 210, anchor01.y + 10, 25, 25 }, NULL);
                if (iconDataToCopy) DrawIconData(iconData, anchor01.x + 210 + 4, anchor01.y + 10 + 4, 1, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
#if !defined(PLATFORM_WEB)
                hiDpiActive = GuiToggle((Rectangle){ anchor01.x + 520, anchor01.y + 10, 25, 25 }, "#199#", hiDpiActive);
#else
                if (GuiButton((Rectangle){ anchor01.x + 520, anchor01.y + 10, 25, 25 }, "#53#")) ToggleFullscreen();
#endif
                if (GuiButton((Rectangle){ anchor01.x + 550, anchor01.y + 10, 75, 25 }, "#191#ABOUT")) windowAboutState.windowActive = true;
                //----------------------------------------------------------------------------------

                // GUI: Work area
                //----------------------------------------------------------------------------------
                GuiLabel((Rectangle){ anchor01.x + 15, anchor01.y + 45, 140, 25 }, "Choose rIcon for Edit:");

                // Draw icons selection panel
                selectedIcon = GuiToggleGroup((Rectangle){ anchor01.x + 15, anchor01.y + 70, 18, 18 }, toggleIconsText, selectedIcon);

                fileTypeActive = GuiComboBox((Rectangle){ anchor01.x + 15, anchor01.y + 400, 160, 25 }, "rIcons File (.rgi);rIcons Image (.png);rIcons Code (.h)", fileTypeActive);

                if (GuiButton((Rectangle){ anchor01.x + 185, anchor01.y + 400, 150, 25 }, "#07#Export rIcons")) showExportFileDialog = true;

                GuiLabel((Rectangle){ anchor01.x + 365, anchor01.y + 45, 126, 25 }, "rIcon Name ID:");

                // NOTE: We show icon name id for selected icon
                if (GuiTextBox((Rectangle){ anchor01.x + 365, anchor01.y + 70, 260, 25 }, guiIconsName[selectedIcon], 32, iconNameIdEditMode)) iconNameIdEditMode = !iconNameIdEditMode;

                iconEditScale = (int)GuiSliderBar((Rectangle){ anchor01.x + 410, anchor01.y + 110, 180, 10 }, "ZOOM:", TextFormat("x%i", iconEditScale), (float)iconEditScale, 0, 16);
                if (iconEditScale < 2) iconEditScale = 2;
                else if (iconEditScale > 16) iconEditScale = 16;

                // Draw selected icon at selected scale
                DrawRectangle(anchor01.x + 365, anchor01.y + 130, 256, 256, Fade(GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)), 0.3f));
                GuiDrawIcon(selectedIcon, (int)anchor01.x + 365 + 128 - RICON_SIZE*iconEditScale/2, (int)anchor01.y + 130 + 128 - RICON_SIZE*iconEditScale/2, iconEditScale, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

                // Draw grid (returns selected cell)
                cell = GuiGrid((Rectangle){ anchor01.x + 365 + 128 - RICON_SIZE*iconEditScale/2, anchor01.y + 130 + 128 - RICON_SIZE*iconEditScale/2, RICON_SIZE*iconEditScale, RICON_SIZE*iconEditScale }, iconEditScale, 1);

                // Draw selected cell lines
                if ((cell.x >= 0) && (cell.y >= 0) && (cell.x < RICON_SIZE) && (cell.y < RICON_SIZE))
                {
                    DrawRectangleLinesEx((Rectangle){ anchor01.x + 365 + iconEditScale*cell.x + 128 - RICON_SIZE*iconEditScale/2,
                                                      anchor01.y + 130 + iconEditScale*cell.y + 128 - RICON_SIZE*iconEditScale/2,
                                                      iconEditScale + 1, iconEditScale + 1 }, 1, RED);
                }

                if (GuiButton((Rectangle){ anchor01.x + 440, anchor01.y + 400, 100, 25 }, "#012#Save Image")) showExportIconImageDialog = true;

                if (GuiButton((Rectangle){ anchor01.x + 545, anchor01.y + 400, 80, 25 }, "#079#Clear"))
                {
                    for (int i = 0; i < RICON_SIZE*RICON_SIZE; i++) GuiClearIconPixel(selectedIcon, i/RICON_SIZE, i%RICON_SIZE);
                }
                //----------------------------------------------------------------------------------

                // Draw status info
                int textPadding = GuiGetStyle(STATUSBAR, TEXT_PADDING);
                GuiSetStyle(STATUSBAR, TEXT_PADDING, 15);
                GuiStatusBar((Rectangle){ anchor01.x + 0, anchor01.y + 435, 351, 25 }, TextFormat("TOTAL ICONS: %i", RICON_MAX_ICONS));
                GuiStatusBar((Rectangle){ anchor01.x + 350, anchor01.y + 435, 290, 25 }, TextFormat("SELECTED: %i - %s", selectedIcon, guiIconsName[selectedIcon]));
                GuiSetStyle(STATUSBAR, TEXT_PADDING, textPadding);

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
                                                TextFormat("#159#Closing %s", toolName), "Do you really want to exit?", "Yes;No");

                    if ((message == 0) || (message == 2)) windowExitActive = false;
                    else if (message == 1) exitWindow = true;
                }
                //--------------------------------------------------------------------------------

                // GUI: Load File Dialog (and loading logic)
                //----------------------------------------------------------------------------------------
                if (showLoadFileDialog)
                {
    #if defined(CUSTOM_MODAL_DIALOGS)
                    int result = GuiFileDialog(DIALOG_MESSAGE, "Load raygui icons file ...", inFileName, "Ok", "Just drag and drop your .rgi style file!");
    #else
                    int result = GuiFileDialog(DIALOG_OPEN, "Load raygui icons file", inFileName, "*.rgi", "raygui Icons Files (*.rgi)");
    #endif
                    if (result == 1)
                    {
                        // Load gui icons data (and gui icon names for the tool)
                        char **tempIconsName = GuiLoadIcons(inFileName, true);
                        for (int i = 0; i < RICON_MAX_ICONS; i++) { strcpy(guiIconsName[i], tempIconsName[i]); free(tempIconsName[i]); }
                        free(tempIconsName);

                        SetWindowTitle(TextFormat("%s v%s - %s", toolName, toolVersion, GetFileName(inFileName)));
                        saveChangesRequired = false;
                    }

                    if (result >= 0) showLoadFileDialog = false;
                }
                //----------------------------------------------------------------------------------------

                // GUI: Save File Dialog (and saving logic)
                //----------------------------------------------------------------------------------------
                if (showSaveFileDialog)
                {
                    if (inFileName[0] != '\0') strcpy(outFileName, inFileName);
                    else strcpy(outFileName, "gui_icons.rgi");

    #if defined(CUSTOM_MODAL_DIALOGS)
                    int result = GuiFileDialog(DIALOG_TEXTINPUT, "Save raygui icons file...", outFileName, "Ok;Cancel", NULL);
    #else
                    int result = GuiFileDialog(DIALOG_SAVE, "Save raygui icons file...", outFileName, "*.rgi", "raygui Icons Files (*.rgi)");
    #endif
                    if (result == 1)
                    {
                        // Save icons file
                        // Check for valid extension and make sure it is
                        if ((GetFileExtension(outFileName) == NULL) || !IsFileExtension(outFileName, ".rgi")) strcat(outFileName, ".rgi\0");

                        // Save icons file
                        SaveIcons(outFileName);

                    #if defined(PLATFORM_WEB)
                        // Download file from MEMFS (emscripten memory filesystem)
                        // NOTE: Second argument must be a simple filename (we can't use directories)
                        emscripten_run_script(TextFormat("saveFileFromMEMFSToDisk('%s','%s')", outFileName, GetFileName(outFileName)));
                    #endif
                    }

                    if (result >= 0) showSaveFileDialog = false;
                }
                //----------------------------------------------------------------------------------------

                // GUI: Export File Dialog (and saving logic)
                //----------------------------------------------------------------------------------------
                if (showExportFileDialog)
                {
                    if (inFileName[0] != '\0') strcpy(outFileName, GetFileNameWithoutExt(inFileName));
                    else strcpy(outFileName, "gui_icons");

                    char filters[64] = { 0 };   // Consider different supported file types

                    switch (fileTypeActive)
                    {
                        case 0: strcpy(filters, "*.rgi"); strcat(outFileName, ".rgi"); break;   // Icons file (.rgi)
                        case 1: strcpy(filters, "*.png"); strcat(outFileName, ".png"); break;   // Icons image (.png)
                        case 2: strcpy(filters, "*.h"); strcat(outFileName, ".h"); break;       // Icons code (.h)
                        default: break;
                    }

    #if defined(CUSTOM_MODAL_DIALOGS)
                    int result = GuiFileDialog(DIALOG_TEXTINPUT, "Export raygui icons file...", outFileName, "Ok;Cancel", NULL);
    #else
                    int result = GuiFileDialog(DIALOG_SAVE, "Export raygui icons file...", outFileName, filters, TextFormat("File type (%s)", filters));
    #endif
                    if (result == 1)
                    {
                        // Export file: outFileName
                        switch (fileTypeActive)
                        {
                            case 0: 
                            {
                                // Check for valid extension and make sure it is
                                if ((GetFileExtension(outFileName) == NULL) || !IsFileExtension(outFileName, ".rgi")) strcat(outFileName, ".rgi\0");
                                SaveIcons(outFileName);
                            } break;
                            case 1:
                            {
                                // Check for valid extension and make sure it is
                                if ((GetFileExtension(outFileName) == NULL) || !IsFileExtension(outFileName, ".png")) strcat(outFileName, ".png\0");
                                Image image = ImageFromIconData(GuiGetIcons(), RICON_MAX_ICONS, 16, 1);
                                ExportImage(image, outFileName);
                                UnloadImage(image);

                                // TODO: Save icons name id into PNG zTXt chunk:
                                //SaveTextChunkPNG(TextDataPNG data, const char *fileName)

                            } break;
                            case 2: 
                            {
                                // Check for valid extension and make sure it is
                                if ((GetFileExtension(outFileName) == NULL) || !IsFileExtension(outFileName, ".h")) strcat(outFileName, ".h\0");
                                ExportIconsAsCode(outFileName);
                            } break;
                            default: break;
                        }

                    #if defined(PLATFORM_WEB)
                        // Download file from MEMFS (emscripten memory filesystem)
                        // NOTE: Second argument must be a simple filename (we can't use directories)
                        emscripten_run_script(TextFormat("saveFileFromMEMFSToDisk('%s','%s')", outFileName, GetFileName(outFileName)));
                    #endif
                    }

                    if (result >= 0) showExportFileDialog = false;
                }
                //----------------------------------------------------------------------------------------

                // GUI: Export Icon Image Dialog (and saving logic)
                //----------------------------------------------------------------------------------------
                if (showExportIconImageDialog)
                {
                    strcpy(outFileName, TextFormat("%s_%ix%i.png", TextToLower(guiIconsName[selectedIcon]), RICON_SIZE, RICON_SIZE));

                #if defined(CUSTOM_MODAL_DIALOGS)
                    int result = GuiFileDialog(DIALOG_TEXTINPUT, "Export raygui icon as image file...", outFileName, "Ok;Cancel", NULL);
                #else
                    int result = GuiFileDialog(DIALOG_SAVE, "Export raygui icon as image file...", outFileName, "*.png", "Image File (*.png)");
                #endif
                    if (result == 1)
                    {
                        // Export file: outFileName
                        // Check for valid extension and make sure it is
                        if ((GetFileExtension(outFileName) == NULL) || !IsFileExtension(outFileName, ".png")) strcat(outFileName, ".png\0");
                        Image icon = ImageFromIconData(GuiGetIconData(selectedIcon), 1, 1, 0);
                        ExportImage(icon, outFileName);
                        UnloadImage(icon);

                    #if defined(PLATFORM_WEB)
                        // Download file from MEMFS (emscripten memory filesystem)
                        // NOTE: Second argument must be a simple filename (we can't use directories)
                        emscripten_run_script(TextFormat("saveFileFromMEMFSToDisk('%s','%s')", outFileName, GetFileName(outFileName)));
                    #endif
                    }

                    if (result >= 0) showExportIconImageDialog = false;
                }
                //----------------------------------------------------------------------------------------

            EndTextureMode();

            // Draw render texture to screen
            DrawTexturePro(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Rectangle){ 0, 0, target.texture.width*screenScale, target.texture.height*screenScale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

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

#if defined(VERSION_ONE)            // Command line
// Show command line usage info
static void ShowCommandLineInfo(void)
{
    printf("\n//////////////////////////////////////////////////////////////////////////////////\n");
    printf("//                                                                              //\n");
    printf("// %s v%s ONE - %s            //\n", toolName, toolVersion, toolDescription);
    printf("// powered by raylib v4.0 (www.raylib.com) and raygui v3.0                      //\n");
    printf("// more info and bugs-report: github.com/raylibtech/rtools                      //\n");
    printf("// feedback and support:      ray[at]raylibtech.com                             //\n");
    printf("//                                                                              //\n");
    printf("// Copyright (c) 2019-2022 raylib technologies (@raylibtech)                    //\n");
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
    bool showUsageInfo = false;         // Toggle command line usage info

    char inFileName[512] = { 0 };       // Input file name
    char outFileName[512] = { 0 };      // Output file name
    int outputFormat = 0;               // Supported output formats

#if defined(COMMAND_LINE_ONLY)
    if (argc == 1) showUsageInfo = true;
#endif

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

        // Load input file: icons data and name ids
        char **tempIconsName = GuiLoadIcons(inFileName, true);
        for (int i = 0; i < RICON_MAX_ICONS; i++) { strcpy(guiIconsName[i], tempIconsName[i]); free(tempIconsName[i]); }
        free(tempIconsName);

        // Process input --> output
        if (IsFileExtension(outFileName, ".rgi")) SaveIcons(outFileName);
        else if (IsFileExtension(outFileName, ".png"))
        {
            Image image = ImageFromIconData(GuiGetIcons(), RICON_MAX_ICONS, 16, 1);
            ExportImage(image, outFileName);
            UnloadImage(image);

            // TODO: Save icons name id into PNG zTXt chunk:
            //SaveTextChunkPNG(TextDataPNG data, const char *fileName)
        }
        else if (IsFileExtension(outFileName, ".h")) ExportIconsAsCode(outFileName);
    }

    if (showUsageInfo) ShowCommandLineInfo();
}
#endif      // VERSION_ONE: Command line

//--------------------------------------------------------------------------------------------
// Load/Save/Export functions
//--------------------------------------------------------------------------------------------

// Load icons from image file
// NOTE: Several parameters are required for proper loading
void LoadIconsFromImage(Image image, int iconCount, int iconSize, int iconsPerLine, int padding)
{
    int lines = iconCount/iconsPerLine;
    if (iconCount%iconsPerLine > 0) lines++;

    Color *pixels = LoadImageColors(image);

    Rectangle icorec = { 0, 0, iconSize, iconSize };
    Color pixel = BLACK;

    // Calculate number of bytes required
    int size = iconsPerLine*lines*iconSize*iconSize/32;
    unsigned int *values = (unsigned int *)calloc(size, sizeof(unsigned int));

    int n = 0;      // Icons counter
    int k = 0;      // Bit counter

    for (int y = 0; y < lines; y++)
    {
        for (int x = 0; x < iconsPerLine; x++)
        {
            // Get icon start pixel position within image (top-left corner)
            icorec.x = padding + x*(iconSize + 2*padding);
            icorec.y = padding + y*(iconSize + 2*padding);

            // Move along pixels within each icon area
            for (int p = 0; p < iconSize*iconSize; p++)
            {
                pixel = pixels[((int)icorec.y + p/iconSize)*(iconsPerLine*(iconSize + 2*padding)) + ((int)icorec.x + p%iconSize)];

                if (ColorToInt(pixel) == 0xffffffff) BIT_SET(values[n*(iconSize*iconSize/32) + p/32], k);

                k++;
                if (k == 32) k = 0;
            }

            n++;
        }
    }

    UnloadImageColors(pixels);
    UnloadImage(image);

    // TODO: return values;
}

// Save raygui icons file (.rgi)
static bool SaveIcons(const char *fileName)
{
    // Style File Structure (.rgi)
    // ------------------------------------------------------
    // Offset  | Size    | Type       | Description
    // ------------------------------------------------------
    // 0       | 4       | char       | Signature: "rGI "
    // 4       | 2       | short      | Version: 100
    // 6       | 2       | short      | reserved

    // 8       | 2       | short      | Num icons (N)
    // 10      | 2       | short      | Icons size (Options: 16, 32, 64) (S)

    // Icons name id (32 bytes per name id)
    // foreach (icon)
    // {
    //   12+32*i  | 32   | char       | Icon NameId
    // }

    // Icons data: One bit per pixel, stored as unsigned int array (depends on icon size)
    // S*S pixels/32bit per unsigned int = K unsigned int per icon
    // foreach (icon)
    // {
    //   ...   | K       | unsigned int | Icon Data
    // }

    bool result = false;
    FILE *rgiFile = fopen(fileName, "wb");

    if (rgiFile != NULL)
    {
        char signature[5] = "rGI ";
        short version = 100;
        short reserved = 0;
        short iconCount = RICON_MAX_ICONS;
        short iconSize = RICON_SIZE;

        fwrite(signature, 1, 4, rgiFile);
        fwrite(&version, 1, sizeof(short), rgiFile);
        fwrite(&reserved, 1, sizeof(short), rgiFile);
        fwrite(&iconCount, 1, sizeof(short), rgiFile);
        fwrite(&iconSize, 1, sizeof(short), rgiFile);

        for (int i = 0; i < iconCount; i++)
        {
            // Write icons name id
            fwrite(guiIconsName[i], 32, 1, rgiFile);
        }

        for (int i = 0; i < iconCount; i++)
        {
            // Write icons data
            fwrite(GuiGetIconData(i), (iconSize*iconSize/32), sizeof(unsigned int), rgiFile);
        }

        fclose(rgiFile);
        result = true;
    }

    return result;
}

// Export gui icons as code (.h)
static void ExportIconsAsCode(const char *fileName)
{
    FILE *codeFile = fopen(fileName, "wt");

    if (codeFile != NULL)
    {
        fprintf(codeFile, "//////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(codeFile, "//                                                                              //\n");
        fprintf(codeFile, "// raygui Icons exporter v1.1 - Icons data exported as a values array           //\n");
        fprintf(codeFile, "//                                                                              //\n");
        fprintf(codeFile, "// more info and bugs-report:  github.com/raysan5/raygui                        //\n");
        fprintf(codeFile, "// feedback and support:       ray[at]raylibtech.com                            //\n");
        fprintf(codeFile, "//                                                                              //\n");
        fprintf(codeFile, "// Copyright (c) 2019-2021 raylib technologies (@raylibtech)                    //\n");
        fprintf(codeFile, "//                                                                              //\n");
        fprintf(codeFile, "//////////////////////////////////////////////////////////////////////////////////\n\n");

        fprintf(codeFile, "//----------------------------------------------------------------------------------\n");
        fprintf(codeFile, "// Defines and Macros\n");
        fprintf(codeFile, "//----------------------------------------------------------------------------------\n");
        fprintf(codeFile, "#define RICON_SIZE             %i   // Size of icons (squared)\n", RICON_SIZE);
        fprintf(codeFile, "#define RICON_MAX_ICONS       %i   // Maximum number of icons\n", RICON_MAX_ICONS);
        fprintf(codeFile, "#define RICON_MAX_NAME_LENGTH   %i       // Maximum length of icon name id\n\n", RICON_MAX_NAME_LENGTH);

        fprintf(codeFile, "// Icons data is defined by bit array (every bit represents one pixel)\n");
        fprintf(codeFile, "// Those arrays are stored as unsigned int data arrays, so every array\n");
        fprintf(codeFile, "// element defines 32 pixels (bits) of information\n");
        fprintf(codeFile, "// Number of elemens depend on RICON_SIZE (by default 16x16 pixels)\n");
        fprintf(codeFile, "#define RICON_DATA_ELEMENTS   (RICON_SIZE*RICON_SIZE/32)\n\n");

        fprintf(codeFile, "//----------------------------------------------------------------------------------\n");
        fprintf(codeFile, "// Icons enumeration\n");
        fprintf(codeFile, "//----------------------------------------------------------------------------------\n");

        fprintf(codeFile, "typedef enum {\n");
        for (int i = 0; i < RICON_MAX_ICONS; i++) fprintf(codeFile, "    RICON_%-24s = %i,\n", (guiIconsName[i][0] != '\0')? guiIconsName[i] : TextFormat("%03i", i), i);
        fprintf(codeFile, "} guiIconName;\n\n");

        fprintf(codeFile, "//----------------------------------------------------------------------------------\n");
        fprintf(codeFile, "// Icons data\n");
        fprintf(codeFile, "//----------------------------------------------------------------------------------\n");

        fprintf(codeFile, "static unsigned int guiIcons[RICON_MAX_ICONS*RICON_DATA_ELEMENTS] = {\n");
        for (int i = 0; i < RICON_MAX_ICONS; i++)
        {
            unsigned int *icon = GuiGetIconData(i);

            fprintf(codeFile, "    ");
            for (int j = 0; j < RICON_DATA_ELEMENTS; j++) fprintf(codeFile, "0x%08x, ", icon[j]);

            fprintf(codeFile, "     // RICON_%s\n", (guiIconsName[i][0] != '\0')? guiIconsName[i] : TextFormat("%03i", i));
        }
        fprintf(codeFile, "};\n");

        fclose(codeFile);
    }
}

//--------------------------------------------------------------------------------------------
// Auxiliar functions
//--------------------------------------------------------------------------------------------

// Draw icon data
static void DrawIconData(unsigned int *data, int x, int y, int pixelSize, Color color)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    for (int i = 0, j = 0; i < RICON_SIZE*RICON_SIZE/32; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            if (BIT_CHECK(data[i], k))
            {
                DrawRectangle(x + (k%RICON_SIZE)*pixelSize, y + j*pixelSize, pixelSize, pixelSize, color);
            }

            if ((k == 15) || (k == 31)) j++;
        }
    }
}

// Gen GRAYSCALE image from and array of bits stored as int (0-BLANK, 1-WHITE)
static Image ImageFromIconData(unsigned int *icons, int iconCount, int iconsPerLine, int padding)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    Image image = { 0 };

    int lines = iconCount/iconsPerLine;
    if (iconCount%iconsPerLine > 0) lines++;

    image.width = (RICON_SIZE + 2*padding)*iconsPerLine;
    image.height = (RICON_SIZE + 2*padding)*lines;
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    image.data = (unsigned char *)calloc(image.width*image.height, 1);  // All pixels BLACK by default

    int pixelX = 0;
    int pixelY = 0;

    for (int n = 0; n < iconCount; n++)
    {
        for (int i = 0, y = 0; i < RICON_SIZE*RICON_SIZE/32; i++)
        {
            for (int k = 0; k < 32; k++)
            {
                pixelX = padding + (n%iconsPerLine)*(RICON_SIZE + 2*padding) + (k%RICON_SIZE);
                pixelY = padding + (n/iconsPerLine)*(RICON_SIZE + 2*padding) + y;

                if (BIT_CHECK(icons[n*RICON_DATA_ELEMENTS + i], k)) ((unsigned char *)image.data)[pixelY*image.width + pixelX] = 0xff;    // Draw pixel WHITE

                if ((k == (RICON_SIZE - 1)) || (k == 31)) y++;  // Move to next pixels line
            }
        }
    }

    return image;
}

// Converts an image to bits array following: Alpha->0, NoAlpha->1
// Very useful to store 1bit color images in an efficient (and quite secure) way
// NOTE: Image size MUST be multiple of 8 for correct fit
static unsigned char *ImageToBits(Image image)
{
    Color *pixels = LoadImageColors(image);

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

    UnloadImageColors(pixels);

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
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
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
