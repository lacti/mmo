#pragma once

namespace client {
    class AudioManager {
    public:
        ~AudioManager();

        void initialize();
        void register_audio_source_from_wav(const std::string&);
        void play_source(const std::string&);
    private:
        std::map<std::string, ALuint> source_map;
    };
}
