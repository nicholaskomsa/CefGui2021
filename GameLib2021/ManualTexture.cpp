#include "ManualTexture.h"

#include "Graphics.h"
#include "Exception.h"

#include <OgreTextureGpuManager.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>
#include <OgreHlmsPbsDatablock.h>
#include <OgreHlmsUnlitDatablock.h>
#include <OgreHlmsSamplerblock.h>
#include <OgreTextureBox.h>



std::size_t ManualTexture::mIDCounter = 0;


void ManualTexture::createDataBlock(Graphics& graphics) {

	Ogre::HlmsManager* hlmsManager = graphics.getRoot()->getHlmsManager();

	Ogre::HlmsUnlit* hlmsPbs = static_cast<Ogre::HlmsUnlit*>(hlmsManager->getHlms(Ogre::HLMS_UNLIT));

	mDataBlock = static_cast<Ogre::HlmsUnlitDatablock*>(
		hlmsPbs->createDatablock(mName,
			mName,
			Ogre::HlmsMacroblock(),
			Ogre::HlmsBlendblock(),
			Ogre::HlmsParamVec()
		));
}

void ManualTexture::createTexture(Graphics& graphics, const Ogre::uint32 width, const Ogre::uint32 height, const Ogre::PixelFormatGpu format) {

	Ogre::TextureGpuManager* textureMgr = graphics.getRoot()->getRenderSystem()->getTextureGpuManager();

	mTexture = textureMgr->createOrRetrieveTexture(
		mName,
		Ogre::GpuPageOutStrategy::AlwaysKeepSystemRamCopy,
		Ogre::TextureFlags::AutomaticBatching,
		Ogre::TextureTypes::Type2D, Ogre::BLANKSTRING);

	mTexture->setNumMipmaps(1);
	mTexture->setResolution(width, height);
	mTexture->setPixelFormat(format);

	//Fill the texture with a hollow rectangle, 10-pixel thick.
	size_t sizeBytes = Ogre::PixelFormatGpuUtils::calculateSizeBytes(
		width, height, 1u, 1u, format, 1u, 4u);


	mBpp = Ogre::PixelFormatGpuUtils::getBytesPerPixel(format);

	Ogre::uint8* data = reinterpret_cast<Ogre::uint8*>(
		OGRE_MALLOC_SIMD(sizeBytes, Ogre::MEMCATEGORY_GENERAL));


	mImage.loadDynamicImage(data, width, height, 1u,
		Ogre::TextureTypes::Type2D, format,
		true, 1u);

	//mSize = mImage.getBytesPerImage(0);

	bool canUseSynchronousUpload = mTexture->getNextResidencyStatus() == Ogre::GpuResidency::Resident && mTexture->isDataReady();

	if (!canUseSynchronousUpload) {
		mTexture->waitForData();
	}
	
	mTexture->scheduleTransitionTo(Ogre::GpuResidency::Resident, &mImage, false);


}


void ManualTexture::create(Graphics& graphics, Ogre::uint32 width, Ogre::uint32 height, Ogre::PixelFormatGpu format) {
	mName = "ManualTexture" + std::to_string(++mIDCounter);
	createTexture(graphics, width, height, format);
	createDataBlock(graphics);
	mDataBlock->setTexture(0, mTexture);
}

void ManualTexture::destroy(Graphics& graphics) {

	Ogre::TextureGpuManager* textureMgr = graphics.getRoot()->getRenderSystem()->getTextureGpuManager();
	textureMgr->destroyTexture(mTexture);

	mImage.freeMemory();
}



void ManualTexture::regionCopy(const Ogre::uint8* src, const Ogre::Rect& dirty) {

	//Ogre::uint32* dest = static_cast<Ogre::uint32*>(mImage.getData(0).data);
	//const Ogre::uint32* srci = reinterpret_cast<const Ogre::uint32*>(src);
	char* dest = (char*)mImage.getData(0).data;

	std::size_t w = mImage.getWidth();
	std::size_t rowLength = (dirty.right - dirty.left) * mBpp;
	
	std::size_t offset = (dirty.top * w + dirty.left) * mBpp;
	w *= mBpp;

	for (std::size_t y = dirty.top; y < dirty.bottom; ++y) {

		char* doffset = dest + offset;
		char* soffset = (char*)src + offset;
		memcpy(doffset, soffset, rowLength);

		offset += w;
	}
}


void ManualTexture::copy(const Ogre::uint8* src) {

	Ogre::uint32 size = mImage.getWidth() * mImage.getHeight() * mBpp;


	/*
	std::size_t width = mImage.getWidth(),
		height = mImage.getHeight();

	Ogre::uint32* dest = static_cast<Ogre::uint32*>( mImage.getData(0).data);

	std::size_t p = 0;
	for (std::size_t x = 0; x < width; x++) {
		for (std::size_t y = 0; y < height; y++) {
			p = (y * width) + x;

			unsigned char r = 255;
			//y % 2 == 0 || y % 3 == 0 || y % 4 == 0 ? r = 255 : r = 0;

			((char*)&dest[p])[0] = 255;
			((char*)&dest[p])[1] = 255;
			((char*)&dest[p])[2] = 255;
			((char*)&dest[p])[3] = 255;
		}
	}
	*/
	
	memcpy(mImage.getData(0).data, src, size);
}

void ManualTexture::update() {
	mImage.uploadTo(mTexture, 0, 0);
}

Ogre::Image2& ManualTexture::getImage() { return mImage; }
Ogre::HlmsDatablock* ManualTexture::getDataBlock() { return mDataBlock; }
Ogre::String  ManualTexture::getName() {
	return mName;
}