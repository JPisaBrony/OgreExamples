#ifdef _WIN32
#define _GLIBCXX_USE_CXX11_ABI 0
#endif
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
    // delete OGRE root object
    delete root;
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

    // set OGRE properties to render to the SDL window
    Ogre::NameValuePairList params;

    #ifdef _WIN32
        // get window information
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        SDL_GetWindowWMInfo(window, &info);

        params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)info.info.win.window);
        params["externalGLContext"] = Ogre::StringConverter::toString((unsigned long)glcontext);
        params["externalGLControl"] = "1";
    #elif __linux__
        params["currentGLContext"] = "1";
    #endif

    // initialize OGRE
    root->initialise(false);
    Ogre::RenderWindow *ogreWindow = root->createRenderWindow("view", SCREEN_WIDTH, SCREEN_HEIGHT, false, &params);

    // create config variable
    Ogre::ConfigFile conf;
    // set the filename of the resource file
    conf.load("resources.cfg");
    // iteration variables
    Ogre::String name, type;
    // get all of the sections in the resource file
    Ogre::ConfigFile::SectionIterator sec = conf.getSectionIterator();
    // iterate over each element in the resource file
    while (sec.hasMoreElements()) {
        // get the next element
        Ogre::ConfigFile::SettingsMultiMap *settings = sec.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator it;
        // iterate over each element in each section
        for (it = settings->begin(); it != settings->end(); ++it) {
            type = it->first;
            name = it->second;
            // add the resource name and type to the resource loader
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, type);
        }
    }

    // load all of the resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // create the main scene manager
    Ogre::SceneManager* scnMgr = root->createSceneManager(Ogre::ST_GENERIC);
    // create a camera node
    Ogre::SceneNode *camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    // set the position of the camera
    camNode->setPosition(0, 0, 5);
    // create the camera
    Ogre::Camera *cam = scnMgr->createCamera("cam");
    // set camera near clipping distance so we can see the mesh
    cam->setNearClipDistance(0.1);
    // set the camera to the camera node
    camNode->attachObject(cam);
    // add the viewport to the window
    Ogre::Viewport *vp = ogreWindow->addViewport(cam);
    // set the background color
    vp->setBackgroundColour(Ogre::ColourValue(1.0f, 0.0f, 1.0f));
    // create a light
    Ogre::Light *light = scnMgr->createLight("light");
    // set the position of the light
    light->setPosition(0, 0, 5);
    // create cube entity and add it to the scene
    Ogre::Entity *ent = scnMgr->createEntity("cube", "cube.mesh");
    // create a scene node
    Ogre::SceneNode *entNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    // rotate the mesh so we can see it better
    entNode->setOrientation(Ogre::Quaternion(Ogre::Degree(45), Ogre::Vector3(1, 1, 1)));
    // attach the entity to the scene object so that we can see it in our scene
    entNode->attachObject(ent);

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
