

#include "plugin_helper.h"
#include "wav.h"


using namespace Gammou::Sound;

struct grain {
	double signal_time;
	double output_time;
};

static double grain_env(
	const double t,
	const double dev)
{
	return exp(-t*t / (2.0 * dev * dev));
}

class granular_component : public sound_component {

public:
	granular_component(
		wav_t *sample,
		const unsigned int grain_count,
		const unsigned int channel_count);

	~granular_component();

	void process(const double input[]) override;
	void initialize_process() override;

private:
	wav_t * m_sample;
	const unsigned int m_grain_count;
	multi_channel_array<grain> m_grain;

	multi_channel_variable<double> m_time;
	multi_channel_variable<double> m_first_grain_time;
};

granular_component::granular_component(
	wav_t *sample,
	const unsigned int grain_count,
	const unsigned int channel_count)
	: sound_component("Granular", 4, 1, channel_count),
		m_sample(sample),
		m_grain_count(grain_count),
		m_grain(this, grain_count),
		m_time(this),
		m_first_grain_time(this)
{
	set_input_name("Seed", 0);
	set_input_name("Radius", 1);
	set_input_name("Dev", 2);
	set_input_name("Speed", 3);
}

granular_component::~granular_component()
{
	wav_free(m_sample);
}

void granular_component::initialize_process()
{
	for (unsigned int i = 0; i < m_grain_count; i++) {
		auto& grain = m_grain[i];
		grain.output_time = -1.0;
		grain.signal_time = 0.0;
	}

	m_time = 0.0;
	m_first_grain_time = 0.0;
}

void granular_component::process(const double input[])
{
	const double dt = get_sample_duration();

	const double seed = input[0];
	const double radius = 0.01 * input[1];
	const double dev = 0.01 * input[2];


	double out = 0.0;

	for (unsigned int i = 0; i < m_grain_count; i++) {
		auto& gr = m_grain[i];

		const double cursor = m_time - gr.output_time;
		out += 
			wav_get_value(m_sample, cursor + gr.signal_time, 0) *
				grain_env(cursor, dev);

		if (cursor > ((double)m_grain_count * radius * 0.5)) {
			//	Respawn the grain
			m_first_grain_time += radius;
			gr.output_time = m_first_grain_time;
			gr.signal_time = seed;
		}
	}

	m_time += dt * input[3];
	m_output[0] = out;
}


class granular_factory : public plugin_factory {

	public:
		granular_factory()
			: plugin_factory("granular", "", 447975152)
		{}
		~granular_factory() {}

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

			return new 
				granular_component(
					sample,
					32, 
					channel_count);
		}
};

EXPORT_FACTORY(granular_factory)
