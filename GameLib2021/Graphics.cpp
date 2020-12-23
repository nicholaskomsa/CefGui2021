#include "Graphics.h"

#include <sstream>

#include "Exception.h"

#include <Compositor/Pass/PassScene/OgreCompositorPassSceneDef.h>
#include <Compositor/OgreCompositorManager2.h>


#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>

#include <OgreArchiveManager.h>
#include <OgreConfigFile.h>

void Graphics::setupResources() {

    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("resources2.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    while (seci.hasMoreElements()) {

        Ogre::String secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();

        if (secName != "Hlms") {

            Ogre::ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i) {

                Ogre::String typeName = i->first, archName = i->second;

                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
            }
        }
    }

    // Initialise, parse scripts etc
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);
}


void Graphics::registerHlms() {

    Ogre::ConfigFile cf;
    cf.load("resources2.cfg");

    Ogre::String rootHlmsFolder = cf.getSetting("DoNotUseAsResource", "Hlms", "");

    if (rootHlmsFolder.empty())
        rootHlmsFolder = "./";
    else if (rootHlmsFolder.back() != '/')
        rootHlmsFolder += "/";



    Ogre::RenderSystem* renderSystem = mRoot->getRenderSystem();

    Ogre::String shaderSyntax = "GLSL";
    if (renderSystem->getName() == "OpenGL ES 2.x Rendering Subsystem")
        shaderSyntax = "GLSLES";
    if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
        shaderSyntax = "HLSL";
    else if (renderSystem->getName() == "Metal Rendering Subsystem")
        shaderSyntax = "Metal";


    auto createHlms = [&]<typename T>(T * &hlms) {
        Ogre::String mainFolderPath;
        Ogre::StringVector libraryFolderPaths;

        //typedef std::remove_pointer< std::remove_reference< decltype(hlms) >::type >::type T;

        T::getDefaultPaths(mainFolderPath, libraryFolderPaths);

        Ogre::ArchiveVec archive;

        for (Ogre::String str : libraryFolderPaths) {

            Ogre::Archive* archiveLibrary = Ogre::ArchiveManager::getSingleton().load(rootHlmsFolder + str, "FileSystem", true);

            archive.push_back(archiveLibrary);
        }

        Ogre::Archive* archiveUnlit = Ogre::ArchiveManager::getSingleton().load(rootHlmsFolder + mainFolderPath, "FileSystem", true);
        hlms = OGRE_NEW T(archiveUnlit, &archive);
        Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlms);
    };

    Ogre::HlmsUnlit* hlmsUnlit = nullptr;
    createHlms(hlmsUnlit);

    Ogre::HlmsPbs* hlmsPbs = nullptr;
    createHlms(hlmsPbs);

   // HlmsWind* hlmsWind = nullptr;
  //  createHlms(hlmsWind);

  /*  auto createHlmsTerra = [&]() {

        Ogre::HlmsTerra* hlmsTerra = nullptr;
        createHlms(hlmsTerra);

        //Add Terra's piece files that customize the PBS implementation.
        //These pieces are coded so that they will be activated when
        //we set the HlmsPbsTerraShadows listener and there's an active Terra
        //(see Tutorial_TerrainGameState::createScene01)
        hlmsPbs = dynamic_cast<Ogre::HlmsPbs*>(Ogre::Root::getSingleton().getHlmsManager()->getHlms(Ogre::HLMS_PBS));
        Ogre::Archive* archivePbs = hlmsPbs->getDataFolder();
        Ogre::ArchiveVec libraryPbs = hlmsPbs->getPiecesLibraryAsArchiveVec();
        libraryPbs.push_back(Ogre::ArchiveManager::getSingletonPtr()->load(
            rootHlmsFolder + "Hlms/Terra/" + shaderSyntax + "/PbsTerraShadows",
            "FileSystem", true));
        hlmsPbs->reloadFrom(archivePbs, &libraryPbs);
    };
    createHlmsTerra();
    */





    if (renderSystem->getName() == "Direct3D11 Rendering Subsystem") {

        // Set lower limits 512kb instead of the default 4MB per Hlms in D3D 11.0
        // and below to avoid saturating AMD's discard limit (8MB) or
        // saturate the PCIE bus in some low end machines.
        bool supportsNoOverwriteOnTextureBuffers;
        renderSystem->getCustomAttribute("MapNoOverwriteOnDynamicBufferSRV", &supportsNoOverwriteOnTextureBuffers);

        if (!supportsNoOverwriteOnTextureBuffers) {

            hlmsPbs->setTextureBufferDefaultSize(512 * 1024);
            hlmsUnlit->setTextureBufferDefaultSize(512 * 1024);
        }
    }

}

void Graphics::setupOgreWindow(const Ogre::String& windowTitle) {

    if (mRoot->restoreConfig() || mRoot->showConfigDialog()) {

        mRoot->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
        mRoot->initialise(false);

        Ogre::ConfigOptionMap ropts = mRoot->getRenderSystem()->getConfigOptions();

        Ogre::NameValuePairList miscParams;
        miscParams["FSAA"] = ropts["FSAA"].currentValue;
        miscParams["vsync"] = ropts["VSync"].currentValue;
        miscParams["Full Screen"] = ropts["Full Screen"].currentValue;
        miscParams["gamma"] = ropts["sRGB Gamma Conversion"].currentValue;

        bool fullScreen = miscParams["Full Screen"] == "Yes";

        int w = 0, h = 0;

        //set the window size to the selected video mode, unsure if generally sensical
        std::istringstream mode(ropts["Video Mode"].currentValue);
        Ogre::String token;

        mode >> w; // width
        mode >> token; // 'x' as seperator between width and height
        mode >> h; // height

        auto setupSDLVideo = [&]() {

            if (!SDL_WasInit(SDL_INIT_VIDEO)) {
                SDL_InitSubSystem(SDL_INIT_VIDEO);
            }
            SDL_WindowFlags flags = fullScreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;

            mSDLWindow = SDL_CreateWindow(windowTitle.c_str(),
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags);
        };

        setupSDLVideo();

        auto setupOgreWindow = [&]() {

            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            SDL_GetWindowWMInfo(mSDLWindow, &wmInfo);

            miscParams["externalWindowHandle"] = Ogre::StringConverter::toString(std::size_t(wmInfo.info.win.window));

            mWindow = mRoot->createRenderWindow(windowTitle, w, h, fullScreen, &miscParams);
        };

        setupOgreWindow();



    }
    else {

    }
}

void Graphics::createSceneManager() {

    // Create SceneManager
    const size_t numThreads = 1u;
    mSceneManager = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, numThreads, "ExampleSMInstance");

    mAmbientLightColor = Ogre::ColourValue(1, 1, 1);
}

void Graphics::setAmbientLightColor(const Ogre::ColourValue& colorValue) {
    mAmbientLightColor = colorValue;
}
void Graphics::setAmbientLightLevel(Ogre::Real lightLevel) {

    Ogre::ColourValue ambientLight = mAmbientLightColor * lightLevel;
    mSceneManager->setAmbientLight(ambientLight, ambientLight, Ogre::Vector3::UNIT_X);


}
void Graphics::createDefaultCamera() {

    // Create & setup camera
    mCamera = mSceneManager->createCamera("Main Camera");
    
    mCamera->setNearClipDistance(1.0f);
    mCamera->setFarClipDistance(150.0f);
    // mCamera->setFarClipDistance(1500.0f);
    mCamera->setAutoAspectRatio(true);
}

Graphics::~Graphics(){}

void Graphics::setup() {

    const Ogre::String pluginsFolder = "./";
    const Ogre::String writeAccessFolder = "./";
    const char* pluginsFile = "plugins.cfg";

    mRoot = OGRE_NEW Ogre::Root(pluginsFolder + pluginsFile,     //
        writeAccessFolder + "ogre.cfg",  //
        writeAccessFolder + "Ogre.log");

    setupOgreWindow();

    registerHlms();

    createSceneManager();
    createDefaultCamera();

    setupResources();


   // HlmsWind* hlmsWind = dynamic_cast<HlmsWind*>(Ogre::Root::getSingleton().getHlmsManager()->getHlms(Ogre::HLMS_USER0));
    //hlmsWind->setup(mSceneManager);


    //setupCompositorDefault();

    getSceneManager()->setForwardClustered(true, 4, 4, 24, 20, 5, 20, 0, 1200);
}

void Graphics::shutdown() {

    try {

        auto shutdownSDLVideo = [&]() {
            if (mSDLWindow) {
                SDL_DestroyWindow(mSDLWindow);
                mSDLWindow = nullptr;
            }

            SDL_QuitSubSystem(SDL_INIT_VIDEO);

            mWindow = nullptr;
        };

        mCamera = nullptr;
        mSceneManager = nullptr;

        if (mRoot)
            OGRE_DELETE mRoot;
        mRoot = nullptr;
    }
    catch (Exception& e) {
        e.showMessage();
    }
    catch (Ogre::Exception& e) {
        Exception::showMessage(e.getFullDescription());
    }
    catch (...) {
        Exception::showMessage("Graphics::shutdown threw an uncaught exception");
    }
}

void Graphics::setupCompositorDefault() {

    Ogre::CompositorManager2* compositorManager = mRoot->getCompositorManager2();
    mWorkspace = compositorManager->addWorkspace(mSceneManager, mWindow->getTexture(), mCamera,
        "DefaultWorkspace", true);
}

bool Graphics::updateFrame() {

    if (mWindow->isVisible())
        if (!mRoot->renderOneFrame())
            return false;

    return true;
}
Ogre::Window* Graphics::getRenderWindow() { return mWindow; }
SDL_Window* Graphics::getSDLWindow() { return mSDLWindow; }

Ogre::SceneManager* Graphics::getSceneManager() { return mSceneManager; }
Ogre::Camera* Graphics::getCamera() { return mCamera; }
Ogre::Root* Graphics::getRoot() { return mRoot; }
Ogre::CompositorWorkspace* Graphics::getWorkspace() { return mWorkspace; }

void Graphics::stopCompositor() {

    if (mWorkspace) {

        Ogre::CompositorManager2* compositorManager = mRoot->getCompositorManager2();
        compositorManager->removeWorkspace(mWorkspace);
        mWorkspace = nullptr;
    }
}
//-----------------------------------------------------------------------------------
void Graphics::restartCompositor() {

    stopCompositor();
    setupCompositorDefault();
}