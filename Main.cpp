#include <SDL2/SDL.h> // SDL, the main stuff
#include <SDL2/SDL_syswm.h> // SDL, the system specific stuff for fun stuff like moving the window and transparency
#include <SDL2/SDL_image.h> // Makes it easier to add an image not baked into the program
#include <Windows.h> // Allows for transparency
#include <random>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

/*
TODO:
    Make them randomly turn around
    Interactions with the mouse
    RightClickMenu to:
        Adjust Speed
        Turn/UpDown chance
    3D Model Support?
    Animated Movement
    Allow for changing the sprite size with the exe
    Allow for each sprite to have a different size
    Add more randomised movement
*/

struct RandomInt{ // This is just a random number generator struct
    std::random_device rand;
    std::mt19937 gen;
    std::uniform_int_distribution<> distr;
        
    RandomInt(int min, int max) : gen(rand()), distr(min, max) {}

    int out() {
        return distr(gen);
    }
};

struct WindowManager { // A struct which holds the window, it's coordinates and the screen dimensions
    SDL_Window* window;
    float CoordX;
    float CoordY;
    int Screenwidth;
    int Screenheight;

    WindowManager(float X = -1, float Y = -1, int SHeight = -1, int SWidth = -1) : CoordX(X), CoordY(Y) {
        if ((X != -1) && (Y != -1)) {
            SDL_DisplayMode dm;
            SDL_GetDesktopDisplayMode(0, &dm);
            Screenwidth = dm.w;
            Screenheight = dm.h;
            window = SDL_CreateWindow("TEst", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SHeight, SWidth, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
        }
        else {
            window = NULL;
            Screenwidth = NULL;
            Screenheight = NULL;
        }
    }
};

struct Textures { // Holds information of which sprite is being accessed; the dimensions of the sprite and the various Sprite textures
    int SpriteIndex; // The current index of the sprite in the vector
    int VectorIndex; // The current vector being accessed
    int SpriteHeight;
    int SpriteWidth;
    std::vector<std::vector<SDL_Texture*>*> Index;
    std::vector<SDL_Texture*> LR; // Right and Left
    std::vector<SDL_Texture*> UD; // Up and Down
    Textures(int SH, int SW) : SpriteIndex(0), VectorIndex(0), SpriteHeight(SH), SpriteWidth(SW) {
        Index.push_back(&LR);
        Index.push_back(&UD);
    }
    std::vector<SDL_Rect> Rects;
};

struct Logic {
    SDL_Event event;
    bool running = true;
    bool ShowRClickMenu = false;
};

bool MakeTransparent(SDL_Window* window); // This makes the background transparent, allowing only the sprite to be visible
void TextureInit(SDL_Renderer* renderer, Textures &Textures); // Creates the Textures Struct and puts all the information into it
void Turn(SDL_Renderer* renderer, Textures &Textures, Logic &logic); // Turn the sprite around and has them go in a different direction
void Render(SDL_Renderer* renderer, Logic &logic, Textures &Textures, WindowManager* windowow = nullptr); // The render function which slaps the sprite onto the window and renders everything
void Vertical(SDL_Renderer* renderer, Textures &Textures, Logic &logic, WindowManager &window); // Tells the sprite to go up or down

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    SDL_VideoInit(NULL);
    IMG_Init(IMG_INIT_PNG);

    bool running = true; // When running is false the program closes
    unsigned int Timer = 0;
    bool BobDirection = true; //Peak = true, trough = false | This creates the sine wave effect
    bool GoRight = true;
    int sine = 0; // The second part of the sine wave effect, defined the number of pixels to move in the Y direction
    RandomInt rng(0, 100); // Random number generator between 1 and 100
    Textures Textures(64, 64); // Initialises Textures and tells the sprite dimensions
    WindowManager window(0, 0, Textures.SpriteHeight, Textures.SpriteWidth);
    Logic logic;

    SDL_Renderer* renderer = SDL_CreateRenderer(window.window, -1, SDL_RENDERER_ACCELERATED);
    TextureInit(renderer, Textures); //Puts the Sprites into Textures

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    while (running) {
        Render(renderer, logic, Textures, &window); // Renders the window
        

        if (Timer % 20 == 0) {
            if (window.CoordX >= (window.Screenwidth-Textures.SpriteWidth) || window.CoordX <= -1) { // If the window touches the edge, turn
                GoRight = !GoRight;
                Turn(renderer, Textures, logic);
            }
            if (GoRight) {
                window.CoordX += 5;
            }
            else {
                window.CoordX -= 5;
            }
            window.CoordY += sine;
            
            if (abs(sine)%4 == 0 && sine != 0) { // If sine is 4 or -4, change sine direction
                BobDirection = !BobDirection;
            }
            if (BobDirection) {
                sine += 1;
            }
            else {
                sine -= 1;
            }
            
            if ((rng.out() == 5) && (Timer % 200 == 0)) { // Random change for the Sprite to go up or down
                Vertical(renderer, Textures, logic, window);
            }

        }
        Timer += 1; // Timer which means that the Sprite isn't moving every tick but every 20 or so

    }
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window.window);
    SDL_Quit(); //Destroy everything, quit and then return a sucessful completion
    return 0;
}

bool MakeTransparent(SDL_Window* window){
    SDL_SysWMinfo Info;
    SDL_VERSION(&Info.version);
    SDL_GetWindowWMInfo(window, &Info);
    HWND WindowHandle = Info.info.win.window;
    SetWindowLong(WindowHandle, GWL_EXSTYLE, GetWindowLong(WindowHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
    return SetLayeredWindowAttributes(WindowHandle, RGB(1, 1, 1), 0, LWA_COLORKEY); // Turn a weird greying colour transparent, this means that that colour CANNOT be used in the sprite
}

void Turn(SDL_Renderer* renderer, Textures &Textures, Logic &logic) {
    bool Complete = false;
    unsigned int Timer = 0;
    bool Up = true; //If Up is true, the Sprite index is increasing, else, decreasing
    if (Textures.SpriteIndex == ((*Textures.Index[Textures.VectorIndex]).size()-1)) { // If the sprite index is at the top, then the only way is down
        Up = false;
    }
    while (!Complete) {
        if (Timer % 300 == 0) {
            if (Up == true) {
                Textures.SpriteIndex++;
            }
            else {
                Textures.SpriteIndex--;
            }
        }
        if (((Textures.SpriteIndex == ((*Textures.Index[Textures.VectorIndex]).size()-1)) && (Up == true) || ((Textures.SpriteIndex == 0) && (Up == false)))) {
            Complete = true;
        }
        Render(renderer, logic, Textures);

        Timer += 1;
    }
}

void TextureInit(SDL_Renderer* renderer, Textures &Textures) {
    std::fstream PathFile("Paths.txt");
    std::string str;
    std::vector<std::string> Paths;
    while (std::getline(PathFile, str)) { // Get the paths to the text files which hold the paths to their respective sprites
        if (str != "") {
            Paths.push_back(str);
        }
    }

    std::vector<std::vector<SDL_Texture*>*> Iterator = { &Textures.LR, &Textures.UD }; // Get some pointers to the Vectors

    int counter = 0;
    for (std::string i : Paths) { // Look through the paths for each text file and push them to another vector
        std::fstream PathFile(i);
        std::string str;
        std::vector<std::string> SpritePaths;
        while (std::getline(PathFile, str)) {
            if (str != "") {
                SpritePaths.push_back(str);
            }
        }

        std::vector<SDL_Texture*> SpriteTextures;

        for (std::string j : SpritePaths) { // Load the sprites and put them into their respective vectors
            if (j != "") {
                SDL_Surface* SurfaceSprite = IMG_Load(j.c_str());
                SDL_Texture* Sprite = SDL_CreateTextureFromSurface(renderer, SurfaceSprite);
                SDL_FreeSurface(SurfaceSprite);
                SpriteTextures.push_back(Sprite);
            }
        }
        *Iterator[counter] = SpriteTextures;
        counter++;
    }
}

void Render(SDL_Renderer* renderer, Logic &logic, Textures &Textures, WindowManager* windowow) {
    WindowManager &window = *windowow;
    while (SDL_PollEvent(&logic.event) != 0) {
        if (logic.event.type == SDL_QUIT) {
            logic.running = false;
        }
        if (logic.event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mouse = { logic.event.button.x, logic.event.button.y };
            if (logic.event.button.button == SDL_BUTTON_RIGHT) {
                SDL_Rect SetButton = { mouse.x, mouse.y, 140, 30 };
                SDL_Rect QuitButton = { mouse.x, mouse.y + 30, 140, 30 };
                SDL_Rect SetShadow = { SetButton.x + 5, SetButton.y + 5, SetButton.w, SetButton.h };
                SDL_Rect QuitShadow = { QuitButton.x + 5, QuitButton.y + 5, QuitButton.w, QuitButton.h };
                Textures.Rects.push_back(SetButton);
                Textures.Rects.push_back(SetShadow);
                Textures.Rects.push_back(QuitButton);
                Textures.Rects.push_back(QuitShadow);
                logic.ShowRClickMenu = true;
                SDL_SetWindowSize(window.window, 400, 400);
            } else if (logic.event.button.button == SDL_BUTTON_LEFT && logic.ShowRClickMenu) {
                if (SDL_PointInRect(&mouse, &Textures.Rects[0])) {
                    SDL_Log("OH MY GOD");
                    logic.ShowRClickMenu = false;
                }
                else if (SDL_PointInRect(&mouse, &Textures.Rects[2])) {
                    SDL_Log("ARGH");
                    logic.running = false;
                }
                else {
                    Textures.Rects = {};
                    logic.ShowRClickMenu = false;
                }
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 1, 1, 1, 0); // Make the background that horrible transparent shade of grey
    SDL_RenderClear(renderer);
    if (logic.ShowRClickMenu) {
        for (int i = 0; i < 3; i+=2) {
            SDL_SetRenderDrawColor(renderer, 5, 5, 5, 175);
            SDL_RenderFillRect(renderer, &Textures.Rects[i+1]);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &Textures.Rects[i]);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &Textures.Rects[i]);
        }
    }
    else {
        if (windowow != nullptr) { // If window is a null pointer then don't change the window, just update its contents
            SDL_SetWindowPosition(window.window, window.CoordX, window.CoordY);
            MakeTransparent(window.window);
        }



    
        SDL_Rect destRect = { 0, 0, Textures.SpriteHeight, Textures.SpriteWidth}; // Create a rectangle which has the same dimensions as the textures
        SDL_RenderCopy(renderer, (*Textures.Index[Textures.VectorIndex])[Textures.SpriteIndex], NULL, &destRect); // Slap on the texture to the rectangle
    }
    SDL_RenderPresent(renderer); // Render it
}

void Vertical(SDL_Renderer* renderer, Textures &Textures, Logic &logic, WindowManager &window) {
    int Complete = 0;
    unsigned int Timer = 0;
    RandomInt rng(0, 1);
    bool Up = NULL;
    if (window.CoordY - 200 < 0) {
        Up = false; //If Up is true, the Sprite is going up, if not, it's going downwards.
    }
    else if (window.CoordY + 200 + Textures.SpriteHeight > window.Screenheight) {
        Up = true;
    }
    else {
        if (rng.out() == 0) {
            Up = true;
        }
        else {
            Up = false;
        }
    }
    int direction = Textures.SpriteIndex; // Direction is the current index which tells us which way the sprite was facing and thus moving
    Textures.VectorIndex = 1; // Change to the Up/Down index
    int middle = std::floor((*Textures.Index[Textures.VectorIndex]).size()/2); // Find the middle
    Textures.SpriteIndex = middle;
    int count = 0;
    while (Complete <= 2) {
        if ((Timer % 20 == 0) && (Timer != 0)) {
            if (count == 0) { // If the counting hasn't started yet, change the textures until they are in the correct position and start the count
                if (Timer % 300 == 0) {
                    if (Up) {
                        if (Textures.SpriteIndex != 0) {
                            Textures.SpriteIndex--;
                        }
                        else {
                            count++;
                        }
                    }
                    else {
                        if (Textures.SpriteIndex != (*Textures.Index[Textures.VectorIndex]).size() - 1) {
                            Textures.SpriteIndex++;
                        }
                        else {
                            count++;
                        }
                       
                    }
                }
            }
            else if (count > 200) { // If the count is over, then return back to the neutral textures
                if (Timer % 300 == 0) {
                    if ((Textures.SpriteIndex != middle))
                        if (Up) {
                            Textures.SpriteIndex++;
                        }
                        else {
                            Textures.SpriteIndex--;
                        }
                    Complete++;
                }
            }
            else { // If the count is happening then go Up or Down
                if (Up) {
                    window.CoordY -= 1;
                    count++;
                }
                else {
                    window.CoordY += 1;
                    count++;
                }
            }
        }

        Render(renderer, logic, Textures, &window); // Render the changes
        Timer += 1;
    }
    Textures.SpriteIndex = direction; //Put the Sprite index Back to position
    Textures.VectorIndex = 0; // Go back to the Left/Right Vector

}