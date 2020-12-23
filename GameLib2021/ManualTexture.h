#pragma once


#include <OgreHlmsSamplerblock.h>
#include <OgreImage2.h>

class Graphics;
namespace Ogre {
	class TextureGpu;
	class HlmsUnlitDatablock;
}

class ManualTexture {

	static std::size_t mIDCounter;
	
	Ogre::Image2 mImage;

protected:

	Ogre::TextureGpu* mTexture;
	Ogre::HlmsUnlitDatablock* mDataBlock;
	Ogre::String mName;
	int mBpp{ 0 };

	virtual void createDataBlock(Graphics& graphics);
	void createTexture(Graphics& graphics, const Ogre::uint32 width, const Ogre::uint32 height, const Ogre::PixelFormatGpu format);

public:
	ManualTexture() = default;
	virtual ~ManualTexture() = default;

	void create(Graphics& graphics, Ogre::uint32 width, Ogre::uint32 height, Ogre::PixelFormatGpu format = Ogre::PixelFormatGpu::PFG_RGBA8_UNORM_SRGB);
	void destroy(Graphics& graphics);

	void regionCopy(const Ogre::uint8* src, const Ogre::Rect& dirty);
	void copy(const Ogre::uint8* src);
	void update();

	Ogre::Image2& getImage();
	Ogre::HlmsDatablock* getDataBlock();
	Ogre::String getName();
};
