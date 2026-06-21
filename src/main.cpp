#include "Game.h"
#include "TextureManager.h"

int main()
{
    TextureManager::getInstance().loadTexture("muffintop", "Gemini_Generated_Image_ucq2onucq2onucq2-removebg-preview.png");
    Game game;
    game.run();
}
