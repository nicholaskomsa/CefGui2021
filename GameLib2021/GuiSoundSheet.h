#pragma once

#include <map>

/*

#include "guisheet.h""
#include "audio.h"




class GuiSoundSheet : public GuiSheet {

	std::map<std::string, SoundSource::Ptr> mSoundSources;

	Audio& mAudio;

	void onCreateSound(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);

		std::string soundName = arge.next(),
			soundFile = arge.next();
		int maxUses = arge.next();

		mAudio.createSound(soundName, maxUses, soundFile);

	}
	void onFreeSound(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);

		std::string soundName = arge.next();

		mAudio.removeSound(soundName);
	}

	void onCreateSoundSource(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);

		std::string sourceName = arge.next();

		mSoundSources[sourceName] = mAudio.getSoundSource();
	}
	void onFreeSoundSource(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);

		std::string sourceSourceName = arge.next();

		SoundSource::Ptr source = mSoundSources[sourceSourceName];
		source->freeSound();
	}

	void onSetupSource(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);


		std::string sourceSourceName = arge.next(),
			soundName = arge.next();
		bool autoDeactivate = arge.next();
		bool looping = arge.next();
		double gain = arge.next(),
			pitch = arge.next();

		double px = arge.next(), py = arge.next(), pz = arge, ext();
		double vx = arge.next(), vy = arge.next(), vz = arge.next();

		SoundSource::Ptr source = mSoundSources[sourceSourceName];

		if (source) {
			source->setSound(mAudio.getSound(soundName));
			source->setAutoDeactivate(autoDeactivate);
			source->setLooping(looping);
			source->setGain(gain);
			source->setPitch(pitch);
			source->setPosition(px, py, pz);
			source->setVelocity(vx, vy, vz);
		}
	}
	void onSoundPlay(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);

		std::string sourceSourceName = arge.next();

		SoundSource::Ptr source = mSoundSources[sourceSourceName];
		if (source)
			source->play();
	}
	void onSoundStop(const CefRefPtr<CefListValue>& args) {
		ArgExtractor arge(args);

		std::string sourceSourceName = arge.next();

		SoundSource::Ptr source = mSoundSources[sourceSourceName];
		if (source)
			source->stop();
	}

public:

	GuiSoundSheet(Audio& audio, std::string sheetPath)
		: mAudio(audio)
		, GuiSheet(sheetPath)
	{

		addCallback("onCreateSound", [&](const CefRefPtr<CefListValue>& args) {
			onCreateSound(args);
			});
		addCallback("onFreeSound", [&](const CefRefPtr<CefListValue>& args) {
			onFreeSound(args);
			});
		addCallback("onCreateSoundSource", [&](const CefRefPtr<CefListValue>& args) {
			onCreateSoundSource(args);
			});

		addCallback("onFreeSoundSource", [&](const CefRefPtr<CefListValue>& args) {
			onFreeSoundSource(args);
			});

		addCallback("onSetupSoundSource", [&](const CefRefPtr<CefListValue>& args) {
			onSetupSource(args);
			});

		addCallback("onSoundPlay", [&](const CefRefPtr<CefListValue>& args) {
			onSoundPlay(args);
			});

		addCallback("onSoundStop", [&](const CefRefPtr<CefListValue>& args) {
			onSoundStop(args);
			});
	}
};

*/