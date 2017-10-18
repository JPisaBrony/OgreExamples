#define _GLIBCXX_USE_CXX11_ABI 0
#include <SDL2/SDL.h>
#include <OGRE/Ogre.h>
#include <SDL2/SDL_syswm.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

SDL_Event event;
SDL_Window *window = NULL;
Ogre::Root *root;

void exit_msg(char *msg) {
    printf(msg);
    exit(-1);
}

void cleanup() {
    // free window
    SDL_DestroyWindow(window);
    // quite SDL
    SDL_Quit();
}

int main(int argc, char* args[]) {
    // setup SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        exit_msg("Couldn't init SDL");

    // setup SDL window
    window = SDL_CreateWindow("SDL Examples", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if(window == NULL)
        exit_msg("Couldn't init SDL Window");

    // create OGRE root object
    root = new Ogre::Root();

    // setup render system
    Ogre::RenderSystem *render = root->getAvailableRenderers().at(0);
    root->setRenderSystem(render);

    // create opengl context
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    // get window information
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);

    // set OGRE properties to render to the SDL window
    Ogre::NameValuePairList params;
    params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)info.info.win.window);
    params["externalGLContext"] = Ogre::StringConverter::toString((unsigned long)glcontext);
    params["externalGLControl"] = "1";

    // initialize OGRE
    root->initialise(false);
    Ogre::RenderWindow *ogreWindow = root->createRenderWindow("view", SCREEN_WIDTH, SCREEN_HEIGHT, false, &params);

    // create the main scene manager
    Ogre::SceneManager* scnMgr = root->createSceneManager(Ogre::ST_GENERIC);
    // create a camera node
    Ogre::SceneNode *camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    // create the camera
    Ogre::Camera *cam = scnMgr->createCamera("cam");
    // set the camera to the camera node
    camNode->attachObject(cam);
    // add the viewport to the window
    Ogre::Viewport *vp = ogreWindow->addViewport(cam);
    // set the background color
    vp->setBackgroundColour(Ogre::ColourValue(1.0f, 0.0f, 1.0f));

    while(1) {
        // check for pending events
        while(SDL_PollEvent(&event)) {
            // quit was requested
            if(event.type == SDL_QUIT) {
                cleanup();
                return 0;
            // keyboard button was hit
            } else if (event.type == SDL_KEYDOWN) {
                // check which key was hit
                switch(event.key.keysym.sym) {
                    // quit
                    case 'q':
                        cleanup();
                        return 0;
                }
            }
        }

        // render one OGRE frame
        root->renderOneFrame();
        // swap window to display OGRE properly
        SDL_GL_SwapWindow(window);
     }

    return 0;
}
