
#include <fstream>
#include <cstring>

#include "wav_loader.h"

// Trick from gist.github.com/PhilCK/1534763
#ifdef __GNUC__
#define PACKED(struct_decl) struct_decl __attribute__((__packed__))
#else
#define PACKED(struct_decl) __pragma(pack(push, 1)) struct_decl __pragma(pack(pop))
#endif

namespace Gammou {

    /**
     * Wav sample implementation
     */

    wav_sample::wav_sample(float sample_rate, std::vector<channel>&& channels)
    :   _channels{std::move(channels)},
        _sample_rate{sample_rate},
        _sample_count{_channels[0].size()}
    {
        const auto channel_count = get_channel_count();
        for (auto chan = 0u; chan != channel_count; ++chan) {
            if (_channels[chan].size() != _sample_count)
                throw std::invalid_argument("wav_sample : inconsistent channel sizes");
        }
    }

    std::vector<uint8_t> wav_sample::clone_channel_data(std::size_t channel_id) const
    {
        const auto data_size = _sample_count * sizeof(float);
        const auto header_size = sizeof(wav_channel::sample_count) + sizeof(wav_channel::sample_rate);
        std::vector<uint8_t> chunk(header_size + data_size);

        const auto& chan = get_channel(channel_id);
        auto *raw_channel = reinterpret_cast<wav_channel*>(chunk.data());

        raw_channel->sample_count = _sample_count;
        raw_channel->sample_rate = _sample_rate;
        std::memcpy(raw_channel->samples, chan.data(), data_size);

        return chunk;
    }

    /**
     * wav loader implementation
     */
    static constexpr uint16_t WAVE_FMT_PCM = 0x0001u;
    static constexpr uint16_t WAVE_FMT_FLOAT = 0x0003u;

    // Packed struct that match the wav binary format
    PACKED(
    struct WAVE_file_header
    {
        char RIFF[4];
        uint32_t file_size_m8;
        char WAVE[4];
    });

    PACKED(
    struct WAVE_generic_block_header
    {
        char block_id[4];
        uint32_t block_size;
    });

    PACKED(
    struct WAVE_format_descriptor
    {
        uint16_t format_id;
        uint16_t channel_count;
        uint32_t sample_rate;
        uint32_t byte_per_sec;
        uint16_t byte_per_block;
        uint16_t bit_per_sample;
    }
    );

    PACKED(
    struct WAVE_format_block
    {
        WAVE_generic_block_header header;
        WAVE_format_descriptor format;
    });

    template <typename Tfloat>
    static void _load_float_samples(std::vector<wav_sample::channel>& chans, const uint8_t *raw_data)
    {
        const auto channel_count = chans.size();
        const auto sample_count = chans[0].size();
        const auto *data = reinterpret_cast<const Tfloat*>(raw_data);

        for (auto sample = 0u; sample < sample_count; ++sample)
            for (auto chan = 0u; chan < channel_count; ++chan)
                chans[chan][sample] = static_cast<float>(data[chan + sample * channel_count]);
    }

    static int32_t _load_pcm_sample(const uint8_t *raw_data, uint16_t sample_size)
    {
        int32_t block = 0;

        for (auto i = 0u; i < sample_size; ++i)
            block |= (raw_data[i] << (i * 8u));

        //  non positive block, sign bit must be propagated
        if ((raw_data[sample_size - 1u] & 0x80u) != 0u) {
            for (auto i = sample_size; i < sizeof(int32_t); ++i)
                block |= (0xFFu << (i * 8u));
        }

        return block;
    }

    static void _load_pcm_samples(std::vector<wav_sample::channel>& chans, const uint8_t *raw_data, uint16_t bit_depth)
    {
        const auto channel_count = chans.size();
        const auto sample_count = chans[0].size();
        const auto sample_size = bit_depth / 8u;
        const auto max_value = static_cast<float>((1u << (bit_depth - 1 )) - 1);

        for (auto sample = 0u; sample < sample_count; ++sample) {
            for (auto chan = 0u; chan < channel_count; ++chan) {
                chans[chan][sample] = static_cast<float>(_load_pcm_sample(raw_data, sample_size)) / max_value;
                raw_data += sample_size;
            }
        }
    }

    static std::vector<wav_sample::channel> _load_channels_from_data(
        uint32_t sample_count, uint16_t channel_count, uint16_t bit_depth,
        uint16_t wav_fmt, const uint8_t *data)
    {
        if (wav_fmt != WAVE_FMT_PCM && wav_fmt != WAVE_FMT_FLOAT)
            throw std::invalid_argument("wav loader : unhandled wav format");

        //  Allocate channels
        std::vector<wav_sample::channel> channels;
        for (uint16_t chan = 0u; chan < channel_count; ++chan)
            channels.emplace_back(wav_sample::channel(sample_count));

        if (wav_fmt == WAVE_FMT_FLOAT) {
            if (bit_depth == 32)
                _load_float_samples<float>(channels, data);
            else if (bit_depth == 64)
                _load_float_samples<double>(channels, data);
            else
                throw std::invalid_argument("wav loader : invalid wav float bitdepth");
        }
        else { // PCM
            _load_pcm_samples(channels, data, bit_depth);
        }

        return channels;
    }

    static std::vector<uint8_t> _load_wav_file(const std::filesystem::path& path)
    {
        std::ifstream stream{path, std::ios::binary};
        if (!stream.is_open())
            throw std::invalid_argument("wav loader : unable to open the specified path");

        stream.seekg(0, std::ios::end);
        const auto file_size = stream.tellg();

        if (file_size <= sizeof(WAVE_file_header)) {
            stream.close();
            throw std::invalid_argument("wav loader : file is too small");
        }
        else {
            std::vector<uint8_t> raw_data(file_size);
            stream.seekg(std::ios::beg);
            stream.read(reinterpret_cast<char*>(raw_data.data()), file_size);
            stream.close();

            return raw_data;
        }
    }

    wav_sample load_wav_from_file(const std::filesystem::path& path)
    {
        const auto raw_data = _load_wav_file(path);

        //   Check file header
        const auto file_header = reinterpret_cast<const WAVE_file_header*>(raw_data.data());

        if (std::strncmp(file_header->RIFF, "RIFF", 4) != 0 || std::strncmp(file_header->WAVE, "WAVE", 4) != 0)
            throw std::invalid_argument("wav loader : invalid file header");

        const auto *cursor = raw_data.data() + sizeof(WAVE_file_header);
        const auto *end = raw_data.data() + raw_data.size();

        bool found_format_block = false;
        WAVE_format_descriptor format;

        // Read block
        while (cursor < (end - sizeof(WAVE_generic_block_header))) {
            const auto block_header = reinterpret_cast<const WAVE_generic_block_header*>(cursor);

            // format block
            if (std::strncmp(block_header->block_id, "fmt ", 4) == 0) {
                const auto fmt_block = reinterpret_cast<const WAVE_format_block*>(block_header);
                found_format_block = true;
                format = fmt_block->format;
            }
            // data block
            else if (std::strncmp(block_header->block_id, "data", 4) == 0) {
                if (found_format_block)
                    return wav_sample(
                        static_cast<float>(format.sample_rate),
                        _load_channels_from_data(
                            block_header->block_size / format.byte_per_block,
                            format.channel_count, format.bit_per_sample, format.format_id,
                            cursor + sizeof(WAVE_generic_block_header)));
            }
            // else unhandled block

            cursor += (sizeof(WAVE_generic_block_header) + block_header->block_size);
        }

        throw std::invalid_argument("wav loader : no audio data found in wave file");
    }
}