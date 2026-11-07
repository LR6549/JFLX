//
// Created by lenny on 21.01.2026.
//

#ifndef JFLX_SDL3_AUDIOHANDLER_HPP
#define JFLX_SDL3_AUDIOHANDLER_HPP

// Additional macro — can be used elsewhere to check if this header has been
// included (e.g. #ifdef JFLX_SDL3_AUDIOHANDLER).
#define JFLX_SDL3_AUDIOHANDLER

// STL includes
#include <string>         // For sound/music name keys
#include <unordered_map>  // For O(1) average-case name → audio object lookups

// SDL3_mixer extension header (MIX_Audio, MIX_Mixer, MIX_Track)
#include <SDL3_mixer/SDL_mixer.h>

namespace JFLX::SDL3 {

    /**
     * @class AudioHandler
     * @brief Loads, stores, and plays SDL3_mixer sound effects and music tracks.
     *
     * Audio assets are split into two independent categories:
     *  - **Sounds** — short sound effects played on the soundMixer (e.g. footsteps,
     *                  explosions). Multiple sounds can overlap freely.
     *  - **Music**  — longer background tracks played on the musicMixer through a
     *                  single dedicated musicTrack. Only one music track plays at a
     *                  time; switching tracks fades the current one out first.
     *
     * Assets are looked up by their filename stem (filename without extension),
     * e.g. "explosion" or "main_theme", so names must be unique within each category.
     *
     * Ownership: this class owns every MIX_Audio* it loads. Call clear() (or let
     * the destructor run) to free all audio resources and close the mixer.
     */
    class AudioHandler {
    public:
        /**
         * @brief Initialises SDL3_mixer and creates the sound and music mixers/tracks.
         *
         * Opens the audio device and sets up:
         *   - soundMixer  for concurrent sound effect playback
         *   - musicMixer  for exclusive music track playback
         *   - musicTrack  the single track used by playMusic()
         */
        AudioHandler();

        /// Destructor — calls clear() to release all audio assets and shut down the mixer.
        ~AudioHandler();

        // ------------------------------------------------------------------ //
        //  Asset loading                                                       //
        // ------------------------------------------------------------------ //

        /**
         * @brief Scans a folder for sound effect files and loads them all.
         *
         * Expects files to live under:  <path>/sounds/
         * Each file's stem becomes its lookup key in soundMap.
         * Supported formats: .mp3, .ogg, .wav, .flac
         *
         * @param path  Game root directory (the "sounds" subfolder is appended internally).
         * @return true if at least one sound was loaded successfully, false otherwise.
         */
        bool loadSounds(const std::string& path);

        /**
         * @brief Scans a folder for music files and loads them all.
         *
         * Expects files to live under:  <path>/data/music/
         * Each file's stem becomes its lookup key in musicMap.
         * Supported formats: .mp3, .ogg, .wav, .flac
         *
         * @param path  Game root directory (the "data/music" subfolders are appended
         *              internally).
         * @return true if at least one track was loaded successfully, false otherwise.
         */
        bool loadMusic(const std::string& path);

        // ------------------------------------------------------------------ //
        //  Playback                                                            //
        // ------------------------------------------------------------------ //

        /**
         * @brief Plays a sound effect once on the soundMixer.
         *
         * The sound is fired-and-forgotten — it plays to completion and overlaps
         * freely with other active sounds.
         * Logs a warning (via JFLX logging) if @p soundName is not found.
         *
         * @param soundName  Filename stem of the desired sound, e.g. "jump".
         */
        void playSound(const std::string& soundName);

        /**
         * @brief Starts playing a music track, fading in over @p fadeInMs milliseconds.
         *
         * If another track is currently playing it is faded out first (3 s), and
         * the new track only starts once that fade-out has completed — the
         * transition is handled through the track's stopped callback, so the
         * documented "fade out, then fade in" order is honoured exactly.
         * Uses the single musicTrack. Logs a warning if @p musicName is not found.
         *
         * @param musicName  Filename stem of the desired track, e.g. "main_theme".
         * @param fadeInMs   Fade-in duration in milliseconds (default: 2000 ms).
         */
        void playMusic(const std::string& musicName, int fadeInMs = 2000);

        /**
         * @brief Stops the currently playing music track.
         *
         * Also cancels a pending track switch queued by playMusic().
         *
         * @param fadeOutMs  If > 0, the track fades out over this many milliseconds
         *                   before stopping. If 0, playback stops immediately.
         *                   Default: 1000 ms.
         */
        void stopMusic(int fadeOutMs = 1000) const;

        /**
         * @brief Pauses the currently playing music track (keeps its position).
         *
         * Pausing does not fire the track-stopped callback, so a pending track
         * switch simply waits until playback is resumed and the fade completes.
         *
         * @return true on success (also true when nothing is playing), false on error.
         */
        bool pauseMusic();

        /**
         * @brief Resumes a paused music track.
         * @return true on success (also true when nothing is paused), false on error.
         */
        bool resumeMusic();

        // ------------------------------------------------------------------ //
        //  Volume control                                                      //
        // ------------------------------------------------------------------ //

        /**
         * @brief Sets the master gain of the sound-effect mixer (all SFX at once).
         * @param volume Linear gain; 0 = silent, 1 = default. Negative values
         *               are clamped to 0 (SDL_mixer forbids negative gain);
         *               values > 1 amplify.
         * @return true on success, false if no mixer was created.
         */
        bool setSoundVolume(float volume);

        /**
         * @brief Returns the sound-effect mixer's current gain (1.0 if unavailable).
         */
        [[nodiscard]] float getSoundVolume() const;

        /**
         * @brief Sets the master gain of the music mixer.
         * @param volume Linear gain; 0 = silent, 1 = default. Negative values
         *               are clamped to 0; values > 1 amplify.
         * @return true on success, false if no mixer was created.
         */
        bool setMusicVolume(float volume);

        /**
         * @brief Returns the music mixer's current gain (1.0 if unavailable).
         */
        [[nodiscard]] float getMusicVolume() const;

        // ------------------------------------------------------------------ //
        //  Unloading individual assets                                        //
        // ------------------------------------------------------------------ //

        /**
         * @brief Destroys a single sound effect and removes it from soundMap.
         *
         * If the sound is still playing, playback continues to completion —
         * SDL_mixer keeps its own reference until the sound finishes.
         *
         * @param soundName  Filename stem of the sound to remove.
         * @return true if the sound existed and was removed, false otherwise.
         */
        bool unloadSound(const std::string& soundName);

        /**
         * @brief Destroys a single music track and removes it from musicMap.
         *
         * If this track is currently playing it is stopped immediately, and a
         * pending switch queued for it is cancelled before the audio is freed.
         *
         * @param musicName  Filename stem of the track to remove.
         * @return true if the track existed and was removed, false otherwise.
         */
        bool unloadMusic(const std::string& musicName);

        // ------------------------------------------------------------------ //
        //  Existence checks                                                    //
        // ------------------------------------------------------------------ //

        /**
         * @brief Returns true if a sound with the given name has been loaded.
         * @param soundName  Filename stem to look up in soundMap.
         */
        bool hasSound(const std::string& soundName) const;

        /**
         * @brief Returns true if a music track with the given name has been loaded.
         * @param musicName  Filename stem to look up in musicMap.
         */
        bool hasMusic(const std::string& musicName) const;

    private:
        /// All loaded sound effects, keyed by filename stem.
        /// Values are owned by this class and freed in clear().
        std::unordered_map<std::string, MIX_Audio*> soundMap;

        /// All loaded music tracks, keyed by filename stem.
        /// Values are owned by this class and freed in clear().
        std::unordered_map<std::string, MIX_Audio*> musicMap;

        /// SDL3_mixer mixer instance dedicated to background music playback.
        /// Drives musicTrack exclusively — only one music track at a time.
        MIX_Mixer* musicMixer = nullptr;

        /// SDL3_mixer mixer instance for sound effects.
        /// Supports concurrent playback of multiple overlapping sounds.
        MIX_Mixer* soundMixer = nullptr;

        /// The single playback track used for music on musicMixer.
        /// Reused for every playMusic() call; previous audio is faded out first.
        MIX_Track* musicTrack = nullptr;

        /// A track switch queued by playMusic() while the current track is
        /// fading out. Guarded by MIX_LockMixer(musicMixer) — the stopped
        /// callback runs on the audio thread.
        struct PendingMusic {
            MIX_Audio* audio = nullptr;
            int fadeInMs = 0;
            std::string name;
        };
        mutable PendingMusic pendingMusic {};

        /**
         * @brief Track-stopped callback (audio thread): starts queued music.
         *
         * Registered on musicTrack in the constructor. If a PendingMusic entry
         * exists, it applies that audio input and restarts the track with its
         * fade-in — SDL_mixer explicitly allows this from the callback.
         */
        static void SDLCALL onMusicTrackStopped(void* userdata, MIX_Track* track);

        /**
         * @brief Assigns @p audio to musicTrack and starts looping playback
         *        with a fade-in (@p name is used for logging only).
         *        No-op when no track was created.
         */
        void startMusicTrack(MIX_Audio* audio, int fadeInMs, const std::string& name);

        /**
         * @brief Fades the current music track out over @p fadeOutMs and stops it.
         *        Does not touch pendingMusic (the stopped callback consumes it).
         */
        void fadeOutMusicTrack(int fadeOutMs) const;

        /**
         * @brief Releases all loaded audio assets and resets mixer state.
         *
         * Destroys every MIX_Audio* in both maps and clears the maps.
         * Safe to call multiple times. Called automatically by the destructor.
         */
        void clear();
    };

} // namespace JFLX::SDL3

#endif // JFLX_SDL3_AUDIOHANDLER_HPP
