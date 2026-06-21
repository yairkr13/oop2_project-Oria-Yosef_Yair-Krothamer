#include "Game.h"
#include "TextureManager.h"

int main()
{
    TextureManager::getInstance().loadTexture("Muffintop", "resources/Gemini_Generated_Image_ucq2onucq2onucq2.png");
    //TextureManager::getInstance().loadTexture("Blue", "resources/7131a315-9db1-44f6-ab89-e5cff8e0e38c.jpg");
    Game game;
    game.run();
}
