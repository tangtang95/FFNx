/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#pragma once

#include "log.h"

#include "audio/memorystream/memorystream.h"
#include "audio/vgmstream/vgmstream.h"

#include <stack>
#include <string>
#include <vector>
#include <unordered_map>
#include <soloud.h>

#define NXAUDIOENGINE_INVALID_HANDLE 0xfffff000

static void NxAudioEngineVgmstreamCallback(int level, const char* str)
{
  ffnx_info("VGMStream [L%d]: %s", level, str);
}

class NxAudioEngine
{
public:
	struct MusicOptions
	{
		MusicOptions() :
			offsetSeconds(0.0),
			fadetime(0.0),
			targetVolume(-1.0f),
			relativeSpeed(1.0f),
			format(""),
			noIntro(false),
			sync(false),
			useNameAsFullPath(false),
			suppressOpeningSilence(false)
		{}
		SoLoud::time offsetSeconds;
		SoLoud::time fadetime;
		float targetVolume;
		float relativeSpeed;
		char format[12];
		bool noIntro, sync, useNameAsFullPath, suppressOpeningSilence;
	};

	struct NxAudioEngineSFX
	{
		NxAudioEngineSFX() :
			game_id(0),
			id(0),
			stream(nullptr),
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			volume(1.0f),
			loop(false)
		{}
		int game_id;
		int id;
		SoLoud::VGMStream *stream;
		SoLoud::handle handle;
		float volume;
		bool loop;
	};

	struct NxAudioEngineMusic
	{
		NxAudioEngineMusic() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			id(-1),
			wantedMusicVolume(1.0f),
			lastMusicOffset(-1.0f),
			sync(false) {}
		void invalidate() {
			handle = NXAUDIOENGINE_INVALID_HANDLE;
			id = -1;
		}
		SoLoud::handle handle;
		int32_t id;
		float wantedMusicVolume;
		SoLoud::time lastMusicOffset;
		bool sync;
	};

	struct NxAudioEngineMusicAudioSource
	{
		NxAudioEngineMusicAudioSource(SoLoud::handle handle, SoLoud::AudioSource* audioSource) :
			handle(handle),
			audioSource(audioSource) {}
		SoLoud::handle handle;
		SoLoud::AudioSource* audioSource;
	};

	struct NxAudioEngineVoice
	{
		NxAudioEngineVoice() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			stream(nullptr),
			volume(1.0f) {}
		SoLoud::handle handle;
		SoLoud::VGMStream* stream;
		float volume;
	};

	struct NxAudioEngineAmbient
	{
		NxAudioEngineAmbient() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			stream(nullptr),
			volume(1.0f),
			fade_in(0.0f),
			fade_out(0.0f) {}
		SoLoud::handle handle;
		SoLoud::VGMStream* stream;
		float volume;
		double fade_in;
		double fade_out;
	};

	struct NxAudioEngineMovieAudio
	{
		NxAudioEngineMovieAudio() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			stream(nullptr) {}
		SoLoud::handle handle;
		SoLoud::VGMStream* stream;
	};

	struct NxAudioEngineStreamAudio
	{
		NxAudioEngineStreamAudio() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			stream(nullptr),
			volume(1.0f) {}
		SoLoud::handle handle;
		SoLoud::MemoryStream* stream;
		float volume;
	};

private:
	enum NxAudioEngineLayer
	{
		NXAUDIOENGINE_SFX,
		NXAUDIOENGINE_MUSIC,
		NXAUDIOENGINE_VOICE,
		NXAUDIOENGINE_AMBIENT,
		NXAUDIOENGINE_MOVIE_AUDIO,
	};

	bool _engineInitialized = false;
	SoLoud::Soloud _engine;

	// SFX
	short _sfxReusableChannels = 0;
	short _sfxTotalChannels = 0;
	float _sfxMasterVolume = -1.0f;
	std::map<int, NxAudioEngineSFX> _sfxChannels;
	std::map<std::string, int> _sfxSequentialIndexes;
	std::map<int, SoLoud::VGMStream*> _sfxEffectsHandler;
	std::vector<short> _sfxLazyUnloadChannels;

	SoLoud::VGMStream* loadSFX(std::string id, bool loop = false);
	void unloadSFXChannel(int channel);

	// MUSIC
	NxAudioEngineMusic _musics[2];
	std::stack<NxAudioEngineMusic> _musicStack; // For resuming
	std::list<NxAudioEngineMusicAudioSource> _audioSourcesToDeleteLater;

	float _previousMusicMasterVolume = -1.0f;
	float _musicMasterVolume = -1.0f;
	SoLoud::time _lastVolumeFadeEndTime = 0.0;

	void cleanOldAudioSources();
	SoLoud::AudioSource* loadMusic(const char* name, bool isFullPath = false, const char* format = nullptr, bool suppressOpeningSilence = false);
	void overloadPlayArgumentsFromConfig(char* name, uint32_t *id, MusicOptions *MusicOptions);
	void backupMusic(int channelSource);
	void restoreMusic(int channelDest, double stopTime = 0);
	void resetMusicVolume(double time = 0);
	void resetMusicVolume(int channel, double time = 0);

	// VOICE
	short _voiceMaxSlots = 0;
	float _voiceMasterVolume = -1.0f;
	std::map<int, NxAudioEngineVoice> _currentVoice;
	std::map<std::string, int> _voiceSequentialIndexes;

	// AMBIENT
	float _ambientMasterVolume = -1.0f;
	std::map<std::string, int> _ambientSequentialIndexes;
	NxAudioEngineAmbient _currentAmbient;

	// MOVIE AUDIO
	float _movieMasterVolume = -1.0f;
	short _movieAudioMaxSlots = 0;
	std::map<int, NxAudioEngineMovieAudio> _currentMovieAudio;

	// STREAM
	float _streamMasterVolume = -1.0f;
	NxAudioEngineStreamAudio _currentStream;

	// MISC
	// Returns false if the file does not exist
	bool getFilenameFullPath(char *_out, const char* _key, NxAudioEngineLayer _type);

	bool fileExists(const char* filename);

	// CFG
	std::unordered_map<NxAudioEngineLayer,toml::parse_result> nxAudioEngineConfig;

	void loadConfig();

public:

	bool init();
	void flush();
	void cleanup();

	// SFX
	int getSFXIdFromChannel(int channel);
	void unloadSFX(int id);
	bool playSFX(const char* name, int id, int channel, float panning, bool loop = false);
	void stopSFX(int channel, double time = 0);
	void pauseSFX(int channel);
	void resumeSFX(int channel);
	bool isSFXPlaying(int channel);
	float getSFXMasterVolume();
	void setSFXMasterVolume(float volume, double time = 0);
	void setSFXVolume(int channel, float volume, double time = 0);
	void setSFXSpeed(int channel, float speed, double time = 0);
	void setSFXPanning(int channel, float panning, double time = 0);
	void setSFXReusableChannels(short num);
	void setSFXTotalChannels(short num);
	void addSFXLazyUnloadChannel(int channel);

	// Music
	bool canPlayMusic(const char* name);
	bool isMusicDisabled(const char* name);
	bool playMusic(const char* name, uint32_t id, int channel, MusicOptions options = MusicOptions());
	void playSynchronizedMusics(const std::vector<std::string>& names, uint32_t id, MusicOptions options = MusicOptions());
	void swapChannels();
	void stopMusic(double time = 0);
	void stopMusic(int channel, double time = 0);
	void pauseMusic(double time = 0);
	void pauseMusic(int channel, double time = 0, bool backup = false);
	void resumeMusic(double time = 0);
	void resumeMusic(int channel, double time = 0, bool restore = false);
	bool isChannelValid(int channel);
	bool isMusicPlaying();
	bool isMusicPlaying(int channel);
	uint32_t currentMusicId(int channel);
	void setMusicMasterVolume(float volume, double time = 0);
	void restoreMusicMasterVolume(double time = 0);
	float getMusicVolume(int channel);
	bool isMusicVolumeFadeFinished();
	float getMusicMasterVolume();
	void setMusicVolume(float volume, int channel, double time = 0);
	void setMusicSpeed(float speed, int channel);
	void setMusicLooping(bool looping, int channel);

	// Voice
	bool canPlayVoice(const char* name);
	bool playVoice(const char* name, int slot = 0, float volume = 1.0f);
	void stopVoice(int slot = 0, double time = 0);
	void pauseVoice(int slot = 0, double time = 0);
	void resumeVoice(int slot = 0, double time = 0);
	bool isVoicePlaying(int slot = 0);
	void setVoiceMaxSlots(int slot);
	float getVoiceMasterVolume();
	void setVoiceMasterVolume(float volume, double time = 0);

	// Ambient
	bool canPlayAmbient(const char* name);
	bool playAmbient(const char* name, float volume = 1.0f, double time = 0);
	void stopAmbient(double time = 0);
	void pauseAmbient(double time = 0);
	void resumeAmbient(double time = 0);
	bool isAmbientPlaying();
	float getAmbientMasterVolume();
	void setAmbientMasterVolume(float volume, double time = 0);

	// Movie Audio
	bool canPlayMovieAudio(const char* filename);
	bool playMovieAudio(const char* filename, int slot = 0, float volume = 1.0f);
	void stopMovieAudio(int slot = 0);
	bool isMovieAudioPlaying(int slot = 0);
	void setMovieAudioMaxSlots(int slot);
	float getMovieMasterVolume();
	void setMovieMasterVolume(float volume, double time = 0);

	// Stream Audio
	void initStream(float duration, float sample_rate, uint32_t channels);
	void pushStreamData(uint8_t* data, uint32_t size);
	bool playStream(float volume = 1.0f);
	void stopStream(double time = 0.0f);
	void pauseStream(double time = 0.0f);
	void resumeStream(double time = 0.0f);
	bool isStreamPlaying();
	float getStreamMasterVolume();
	void setStreamMasterVolume(float volume, double time = 0);
};

extern NxAudioEngine nxAudioEngine;
