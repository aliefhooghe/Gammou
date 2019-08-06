

#include "plugin_helper.h"
#include "wav.h"


using namespace Gammou::Sound;

class sampler_component : public sound_component {

public:

	sampler_component(
		wav_t *sample,
		const std::string& sample_path,
		const unsigned int channel_count);

	~sampler_component();

	void process(const double input[]) override;
	void initialize_process() override;

	unsigned int save_state(data_output_stream & data) override;
private:
	std::string m_sample_path;
	multi_channel_variable<double> m_current_pos;
	wav_t * m_sample;
};

sampler_component::sampler_component(
	wav_t *sample,
	const std::string& sample_path,
	const unsigned int channel_count)
	: sound_component("sampler", 2, wav_get_channel_count(sample), channel_count),
		m_sample_path(sample_path),
		m_current_pos(this),
		m_sample(sample)
{
	set_input_name("Speed", 0);
	set_input_name("Offset", 1);
}

sampler_component::~sampler_component()
{
	wav_free(m_sample);
}

void sampler_component::initialize_process()
{
	m_current_pos = 0.0;
}

unsigned int sampler_component::save_state(data_output_stream & data)
{
	unsigned int path_len = m_sample_path.size();
	char path[256];

	std::memcpy(path, m_sample_path.c_str(), path_len);

	data.write(&path_len, sizeof(unsigned int));
	data.write(path, path_len);

	return sizeof(unsigned int) + path_len;
}

void sampler_component::process(const double input[])
{
	const double pos = 
		m_current_pos + input[1];

	const auto channel_count = wav_get_channel_count(m_sample);
	for (auto channel = 0u; channel < channel_count; ++channel)
		m_output[channel] = wav_get_value(m_sample, pos, channel);

	m_current_pos += input[0] * get_sample_duration();
}


class sampler_factory : public plugin_factory {

	public:
		sampler_factory()
			: plugin_factory("Sampler", "Sampler", sampler_id)
		{}
		~sampler_factory() {}

	protected:
		std::unique_ptr<request_form> create_plugin_request_form() override
		{
			return create_request_form(
				request{"Sample", "", path_request{"wav"}});
		}

		abstract_sound_component *create_sound_component(
			data_input_stream& source, 
			const unsigned int channel_count) override
		{
			char path[256];
			unsigned int path_len;

			if (source.read(&path_len, sizeof(unsigned int)) != sizeof(unsigned int) ||
				path_len == 0 ||
				source.read(path, path_len) != path_len)
				throw std::runtime_error("Unable to load sample name from preset");

			path[path_len] = 0;	//	null terminated string

			std::string str_path(path);

			wav_t *sample = wav_load(path);
			if (sample == nullptr)
				throw std::runtime_error("Unable to load " + str_path);

			return new
				sampler_component(
					sample,
					str_path,
					channel_count);
		}

		abstract_sound_component *create_sound_component(
			const answer_form& answer_form, 
			const unsigned int channel_count) override
		{
			auto& answers = 
				std::get<answer_list>(answer_form);
			const std::string& path = 
				std::get<path_answer>(answers[0]).path;

			wav_t *sample = wav_load(path.c_str());

			if (sample == nullptr)
				throw std::runtime_error("Unable to load " + path);

			return new sampler_component(sample, path, channel_count);
		}
};

EXPORT_FACTORY(sampler_factory)
