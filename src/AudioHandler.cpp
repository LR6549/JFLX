//
// Created by lenny on 21.01.2026.
//

#include <SDL3/AudioHandler.hpp>

#include <utilities/logging.hpp>

#include <SDL3/SDL.h>

#include <filesystem>  // Directory iteration when loading audio folders
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace {

    /// Returns true for the container formats SDL3_mixer can decode here.
    bool isAudioExtension(const fs::path& extension) {
        return extension == ".mp3" || extension == ".ogg" ||
               extension == ".wav" || extension == ".flac";
    }

    /**
     * Loads every supported audio file below @p folder into @p out.
     * Duplicate stems keep the previous "last one wins" behavior, but the
     * overwritten asset is released so it cannot leak.
     *
     * @return Number of files loaded successfully.
     */
    size_t loadAudioFolder(const fs::path& folder, MIX_Mixer* mixer,
                           std::unordered_map<std::string, MIX_Audio*>& out,
                           const char* kind) {
        size_t loaded = 0;

        for (const auto& entry : fs::recursive_directory_iterator(folder)) {
            if (!entry.is_regular_file() || !isAudioExtension(entry.path().extension())) continue;

            const std::string name = entry.path().stem().string();
            const std::string file = entry.path().string();

            MIX_Audio* audio = MIX_LoadAudio(mixer, file.c_str(), false);
            if (!audio) {
                JFLX::utilities::log(std::string("Failed to load ") + kind, file, JFLX::utilities::LOGTYPE::JFLX_ERROR);
                JFLX::utilities::log("  SDL error: ", SDL_GetError(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
                continue;
            }

            auto [it, inserted] = out.try_emplace(name, audio);
            if (!inserted) {
                // Same stem in two subfolders — replace the old asset (the mixer
                // tracks keep their own reference, so destroying it is safe).
                MIX_DestroyAudio(it->second);
                it->second = audio;
            }

            JFLX::utilities::log(std::string("Loaded ") + kind, name, JFLX::utilities::LOGTYPE::JFLX_SUCCESS);
            ++loaded;
        }

        return loaded;
    }

} // namespace

JFLX::AudioHandler::AudioHandler() {
    // Initialize SDL_mixer if needed
    if (!MIX_Init()) {
        JFLX::utilities::log("Failed to initialize SDL_mixer: ", SDL_GetError(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
    }

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 44100;
    audioSpec.format = SDL_AUDIO_S16LE;
    audioSpec.channels = 2;

    musicMixer = MIX_CreateMixer(&audioSpec);
    soundMixer = MIX_CreateMixer(&audioSpec);

    if (!musicMixer || !soundMixer) {
        JFLX::utilities::log("Failed to create audio mixers: ", SDL_GetError(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
    }

    musicTrack = MIX_CreateTrack(musicMixer);
    if (musicTrack) {
        // Fires on the audio thread when a fade-out completes (or the track
        // ends/stops) — starts any track switch queued by playMusic().
        MIX_SetTrackStoppedCallback(musicTrack, &AudioHandler::onMusicTrackStopped, this);
    }
}

JFLX::AudioHandler::~AudioHandler() {
    // Drop any queued switch first so the stopped callback can never observe
    // audio that clear() is about to destroy.
    MIX_LockMixer(musicMixer);
    pendingMusic = {};
    MIX_UnlockMixer(musicMixer);

    clear();

    if (musicTrack) {
        MIX_SetTrackStoppedCallback(musicTrack, nullptr, nullptr);
        MIX_DestroyTrack(musicTrack);
    }
    if (musicMixer) MIX_DestroyMixer(musicMixer);
    if (soundMixer) MIX_DestroyMixer(soundMixer);

    MIX_Quit();
}

void JFLX::AudioHandler::clear() {
    for (const auto& entry : soundMap) {
        if (entry.second) MIX_DestroyAudio(entry.second);
    }
    soundMap.clear();

    for (const auto& entry : musicMap) {
        if (entry.second) MIX_DestroyAudio(entry.second);
    }
    musicMap.clear();
}

bool JFLX::AudioHandler::loadSounds(const std::string& path) {
    const fs::path soundFolder = fs::path(path) / "sounds";
    if (!fs::exists(soundFolder)) {
        JFLX::utilities::log("Sound folder does not exist: ", soundFolder.string(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
        return false;
    }

    const size_t loaded = loadAudioFolder(soundFolder, soundMixer, soundMap, "sound");
    if (loaded == 0) {
        JFLX::utilities::log("No sound files loaded from: ", soundFolder.string(), JFLX::utilities::LOGTYPE::JFLX_WARNING);
        return false;
    }
    return true;
}

bool JFLX::AudioHandler::loadMusic(const std::string& path) {
    const fs::path musicFolder = fs::path(path) / "data" / "music";
    if (!fs::exists(musicFolder)) {
        JFLX::utilities::log("Music folder does not exist: ", musicFolder.string(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
        return false;
    }

    const size_t loaded = loadAudioFolder(musicFolder, musicMixer, musicMap, "music");
    if (loaded == 0) {
        JFLX::utilities::log("No music files loaded from: ", musicFolder.string(), JFLX::utilities::LOGTYPE::JFLX_WARNING);
        return false;
    }
    return true;
}

void JFLX::AudioHandler::playSound(const std::string& soundName) {
    const auto it = soundMap.find(soundName);
    if (it == soundMap.cend()) {
        JFLX::utilities::log("Sound not found: ", soundName, JFLX::utilities::LOGTYPE::JFLX_ERROR);
        return;
    }

    if (MIX_Audio* audio = it->second; MIX_PlayAudio(soundMixer, audio)) {
        JFLX::utilities::log("Played sound: ", soundName, JFLX::utilities::LOGTYPE::JFLX_SUCCESS);
    } else {
        JFLX::utilities::log("MIX_PlayAudio failed: ", SDL_GetError(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
    }
}

void JFLX::AudioHandler::playMusic(const std::string& name, int fadeInMs) {
    const auto it = musicMap.find(name);
    if (it == musicMap.end()) {
        JFLX::utilities::log("Music not found: ", name, JFLX::utilities::LOGTYPE::JFLX_ERROR);
        return;
    }

    MIX_Audio* audio = it->second;

    MIX_LockMixer(musicMixer);

    if (pendingMusic.audio == audio) {
        MIX_UnlockMixer(musicMixer);
        JFLX::utilities::log("Music already queued: ", name, JFLX::utilities::LOGTYPE::JFLX_INFO);
        return;
    }

    if (musicTrack && MIX_TrackPlaying(musicTrack) &&
        MIX_GetTrackAudio(musicTrack) == audio && pendingMusic.audio == nullptr) {
        MIX_UnlockMixer(musicMixer);
        JFLX::utilities::log("Music already playing: ", name, JFLX::utilities::LOGTYPE::JFLX_INFO);
        return;
    }

    if (musicTrack && MIX_TrackPlaying(musicTrack)) {
        // Queue the switch; onMusicTrackStopped() starts it once the current
        // track has finished fading out (documented fade-out-then-fade-in).
        pendingMusic = { audio, fadeInMs, name };
        MIX_UnlockMixer(musicMixer);
        fadeOutMusicTrack(3000);
    } else {
        pendingMusic = {};
        MIX_UnlockMixer(musicMixer);
        startMusicTrack(audio, fadeInMs, name);
    }
}

void JFLX::AudioHandler::startMusicTrack(MIX_Audio* audio, int fadeInMs, const std::string& name) {
    if (!musicTrack) {
        JFLX::utilities::log("No music track available: ", name, JFLX::utilities::LOGTYPE::JFLX_ERROR);
        return;
    }

    MIX_SetTrackAudio(musicTrack, audio);

    SDL_PropertiesID options = SDL_CreateProperties();
    SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    SDL_SetNumberProperty(options, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInMs);

    if (MIX_PlayTrack(musicTrack, options)) {
        JFLX::utilities::log("Playing music: ", name, JFLX::utilities::LOGTYPE::JFLX_SUCCESS);
    } else {
        JFLX::utilities::log("MIX_PlayTrack failed: ", SDL_GetError(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
    }

    SDL_DestroyProperties(options);
}

void JFLX::AudioHandler::fadeOutMusicTrack(const int fadeOutMs) const {
    if (musicTrack && MIX_TrackPlaying(musicTrack)) {
        // MIX_StopTrack() takes sample frames; convert the caller's milliseconds.
        const Sint64 fadeOutFrames = (fadeOutMs > 0)
                ? MIX_TrackMSToFrames(musicTrack, fadeOutMs)
                : 0;

        if (!MIX_StopTrack(musicTrack, fadeOutFrames)) {
            JFLX::utilities::log("Failed to stop music track: ", SDL_GetError(), JFLX::utilities::LOGTYPE::JFLX_ERROR);
        }
    }
}

void SDLCALL JFLX::AudioHandler::onMusicTrackStopped(void* userdata, MIX_Track* track) {
    auto* self = static_cast<AudioHandler*>(userdata);
    if (!self || track != self->musicTrack) return;

    MIX_LockMixer(self->musicMixer);
    const PendingMusic pending = self->pendingMusic;
    self->pendingMusic = {};
    MIX_UnlockMixer(self->musicMixer);

    if (pending.audio) {
        self->startMusicTrack(pending.audio, pending.fadeInMs, pending.name);
    }
}

void JFLX::AudioHandler::stopMusic(const int fadeOutMs) const {
    // A user-requested stop cancels any queued track switch.
    MIX_LockMixer(musicMixer);
    pendingMusic = {};
    MIX_UnlockMixer(musicMixer);

    fadeOutMusicTrack(fadeOutMs);
}

bool JFLX::AudioHandler::pauseMusic() {
    if (!musicTrack) return false;
    return MIX_PauseTrack(musicTrack);
}

bool JFLX::AudioHandler::resumeMusic() {
    if (!musicTrack) return false;
    return MIX_ResumeTrack(musicTrack);
}

bool JFLX::AudioHandler::setSoundVolume(const float volume) {
    if (!soundMixer) return false;
    return MIX_SetMixerGain(soundMixer, volume < 0.f ? 0.f : volume);
}

float JFLX::AudioHandler::getSoundVolume() const {
    return soundMixer ? MIX_GetMixerGain(soundMixer) : 1.0f;
}

bool JFLX::AudioHandler::setMusicVolume(const float volume) {
    if (!musicMixer) return false;
    return MIX_SetMixerGain(musicMixer, volume < 0.f ? 0.f : volume);
}

float JFLX::AudioHandler::getMusicVolume() const {
    return musicMixer ? MIX_GetMixerGain(musicMixer) : 1.0f;
}

bool JFLX::AudioHandler::unloadSound(const std::string& soundName) {
    const auto it = soundMap.find(soundName);
    if (it == soundMap.end()) {
        JFLX::utilities::log("Sound not found: ", soundName, JFLX::utilities::LOGTYPE::JFLX_WARNING);
        return false;
    }

    // Safe even while the sound is playing: SDL_mixer tracks hold their own
    // reference and release it when playback finishes.
    MIX_DestroyAudio(it->second);
    soundMap.erase(it);
    return true;
}

bool JFLX::AudioHandler::unloadMusic(const std::string& musicName) {
    const auto it = musicMap.find(musicName);
    if (it == musicMap.end()) {
        JFLX::utilities::log("Music not found: ", musicName, JFLX::utilities::LOGTYPE::JFLX_WARNING);
        return false;
    }

    MIX_Audio* audio = it->second;

    // Cancel a queued switch that targets this audio before it can start.
    MIX_LockMixer(musicMixer);
    if (pendingMusic.audio == audio) pendingMusic = {};
    MIX_UnlockMixer(musicMixer);

    if (musicTrack && MIX_GetTrackAudio(musicTrack) == audio) {
        stopMusic(0); // stopMusic clears pendingMusic as well
    }

    MIX_DestroyAudio(audio);
    musicMap.erase(it);
    return true;
}

bool JFLX::AudioHandler::hasSound(const std::string& soundName) const {
    return soundMap.contains(soundName);
}

bool JFLX::AudioHandler::hasMusic(const std::string& musicName) const {
    return musicMap.contains(musicName);
}
