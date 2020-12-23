#pragma once

#include <mutex>

#include <OgreException.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreWindow.h>

#include <Compositor/OgreCompositorWorkspace.h>

#include <SDL.h>
#include <SDL_syswm.h>


class Graphics {

    Ogre::Root* mRoot = { nullptr };
    Ogre::SceneManager* mSceneManager = { nullptr };
    Ogre::Camera* mCamera = { nullptr };
    Ogre::CompositorWorkspace* mWorkspace{ nullptr };

    Ogre::Window* mWindow = { nullptr };

    SDL_Window* mSDLWindow = { nullptr };


    Ogre::ColourValue mAmbientLightColor{ 0 };

    std::mutex mMutex;

protected:

    void setupResources();
    void registerHlms();

    void setupOgreWindow(const Ogre::String& windowTitle = "Render Window");

    void createSceneManager();
    void createDefaultCamera();

public:

    ~Graphics();

    void setup();
    void shutdown();

    bool updateFrame();

    Ogre::Window* getRenderWindow();
    SDL_Window* getSDLWindow();

    Ogre::SceneManager* getSceneManager();

    template<typename Action>
    void threadSafeAction(Action action) {

        std::scoped_lock lock(mMutex);

        action(mSceneManager);
    }

    Ogre::Camera* getCamera();
    Ogre::Root* getRoot();
    Ogre::CompositorWorkspace* getWorkspace();

    void setAmbientLightColor(const Ogre::ColourValue& colorValue);
    void setAmbientLightLevel(Ogre::Real lightLevel = 1);
    void setupCompositorDefault();
    void stopCompositor();
    void restartCompositor();
};