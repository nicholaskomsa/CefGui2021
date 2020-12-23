#pragma once

#include "ManualTexture.h"



namespace Ogre {
	class SceneNode;
	class ManualObject;
}

class GuiRenderObject {

	class RenderTexture : public ManualTexture {
		virtual void createDataBlock(Graphics& graphics) override;
	public:
	};

	std::string mName{ "GuiRenderObject" };

	RenderTexture mRenderTexture;
	Ogre::SceneNode* mSN{ nullptr };
	Ogre::Item* mGui{ nullptr };

public:

	void create(Graphics& graphics);
	void destroy(Graphics& graphics);

	ManualTexture& getRenderTexture();
};