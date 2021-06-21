#ifndef GAMMOU_WAV_LOADER_H_
#define GAMMOU_WAV_LOADER_H_

#include <filesystem>
#include <vector>
#include <stdint.h>

#include "packages/common/static_memory_chunk_types.h"

namespace Gammou {

    class wav_sample
    {
    public:
        using channel = std::vector<float>;

        wav_sample(float sample_rate, std::vector<channel>&& channels);
        wav_sample(wav_sample&&) noexcept = default;
        wav_sample(const wav_sample&) = default;
        ~wav_sample() noexcept = default;

        std::vector<uint8_t> clone_channel_data(std::size_t channel_id) const;
        const auto& get_channel(std::size_t channel_id) const noexcept { return _channels[channel_id]; }
        auto get_channel_count() const noexcept { return _channels.size(); }
        auto get_sample_rate() const noexcept { return _sample_rate; }
        auto get_sample_count() const noexcept { return _sample_count; }

    private:
        const std::vector<channel> _channels{};
        const float _sample_rate{};
        const std::size_t _sample_count{};
    };

    wav_sample load_wav_from_file(const std::filesystem::path& path);


}

#endif /* GAMMOU_WAV_LOADER_H_ */