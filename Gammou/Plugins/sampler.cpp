

#include "plugin_helper.h"
#include "wav.h"


using namespace Gammou::Sound;

class sampler_component : public sound_component {

public:

	sampler_component(
		wav_t *sample,
		const unsigned int channel_count);

	~sampler_component();

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_variable<double> m_current_pos;
	wav_t * m_sample;
};

sampler_component::sampler_component(
	wav_t *sample,
	const unsigned int channel_count)
	: sound_component("sampler", 2, 1, channel_count),
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

void sampler_component::process(const double input[])
{
	const double pos = 
		m_current_pos + input[1];

	double int_index;
	double dec_index = 
		 modf(pos *(double)wav_get_samplerate(m_sample), &int_index);

	const double a = wav_get_sample(m_sample, (unsigned int)int_index, 	0);
	const double b = wav_get_sample(m_sample, (unsigned int)int_index + 1, 0);

	m_output[0] = (a * (1.0 - dec_index) + b * dec_index);
	m_current_pos += input[0] * get_sample_duration();
}


class sampler_factory : public plugin_factory {

	public:
		sampler_factory()
			: plugin_factory("Sampler", "", 485975163)
		{}
		~sampler_factory() {}

	protected:
		std::unique_ptr<request_form> create_plugin_request_form() override
		{
			return create_request_form(
				request{"Sample", "", path_request{"wav"}});
		}

		abstract_sound_component *create_sound_component(
			data_source& source, 
			const unsigned int channel_count) override
		{
			return nullptr; // todo
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

			return new sampler_component(sample, channel_count);
		}
};

EXPORT_FACTORY(sampler_factory)
