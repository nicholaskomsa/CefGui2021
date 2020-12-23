#include "GuiRenderObject.h"

#include "Graphics.h"

#include <OgreHlmsManager.h>
#include <OgreHlmsUnlit.h>
#include <OgreHlmsUnlitDatablock.h>

#include <ogrerenderqueue.h>

#include <OgreItem.h>


void GuiRenderObject::RenderTexture::createDataBlock(Graphics& graphics){
	Ogre::HlmsManager* hlmsManager = graphics.getRoot()->getHlmsManager();

	Ogre::HlmsUnlit* hlmsPbs = static_cast<Ogre::HlmsUnlit*>(hlmsManager->getHlms(Ogre::HLMS_UNLIT));

	Ogre::HlmsMacroblock macroBlock;
	macroBlock.mDepthCheck = false;
	Ogre::HlmsBlendblock blendBlock;
	blendBlock.setBlendType(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);

	mDataBlock = static_cast<Ogre::HlmsUnlitDatablock*>(
		hlmsPbs->createDatablock(mName,
			mName,
			macroBlock,
			blendBlock,
			Ogre::HlmsParamVec()
		));
}

void GuiRenderObject::create(Graphics& graphics) {
	Ogre::uint32 width = graphics.getRenderWindow()->getWidth()
		, height = graphics.getRenderWindow()->getHeight();

	mRenderTexture.create(graphics, width, height, Ogre::PFG_BGRA8_UNORM_SRGB);

	mGui = graphics.getSceneManager()->createItem("cefgui.mesh", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::SCENE_STATIC);
	mSN = graphics.getSceneManager()->getRootSceneNode()->createChildSceneNode(Ogre::SCENE_STATIC);
	mSN->attachObject(mGui);

	mGui->getSubItem(0)->setUseIdentityView(true);
	mGui->getSubItem(0)->setUseIdentityProjection(true);

	mGui->setLocalAabb(Ogre::Aabb::BOX_INFINITE);

	graphics.getSceneManager()->getRenderQueue()->setRenderQueueMode(254, Ogre::RenderQueue::Modes::FAST);
	mGui->setRenderQueueGroup(254);

	mGui->getSubItem(0)->setDatablock(mRenderTexture.getDataBlock());
}

void GuiRenderObject::destroy(Graphics& graphics) {

	mSN->detachAllObjects();
	mSN->getParentSceneNode()->removeAndDestroyChild(mSN);
	mSN = nullptr;
	graphics.getSceneManager()->destroyItem(mGui);
	mGui = nullptr;

	mRenderTexture.destroy(graphics);
}

ManualTexture& GuiRenderObject::getRenderTexture() { return mRenderTexture; }