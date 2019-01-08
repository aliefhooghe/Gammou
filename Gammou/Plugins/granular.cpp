
#include <random>
#include <cstring>

#include "plugin_helper.h"
#include "granular_base.h"
#include "wav.h"

using namespace Gammou::Sound;

class granular_component : 
	public sound_component,
	public granular_base {

public:
	granular_component(
		wav_t *sample,
		const std::string& sample_path,	//	for state serialization
		const unsigned int grain_count,
		const unsigned int channel_count);

	~granular_component();

	void initialize_process() override;
	void process(const double input[]) override;

	unsigned int save_state(data_output_stream& data) override;

protected:
	double grain_value(const double phase) override;

private:
	wav_t * m_sample;
	std::string m_sample_path;
};


granular_component::granular_component(
	wav_t *sample,
	const std::string& sample_path,
	const unsigned int grain_count,
	const unsigned int channel_count)
:	sound_component("Granular", 5, 1, channel_count),
	granular_base(this, grain_count),
	m_sample(sample),
	m_sample_path(sample_path)
{
	set_input_name("Seed", 0);
	set_input_name("Width", 1);
	set_input_name("Radius", 2);
	set_input_name("Dev", 3);
	set_input_name("Speed", 4);
}

granular_component::~granular_component()
{
	DEBUG_PRINT("Granular DTOR\n");
	wav_free(m_sample);
}

void granular_component::initialize_process()
{
	granular_base::init();
}

void granular_component::process(const double input[])
{
	const double dt = get_sample_duration();

	const double seed = input[0];
	const double width = input[1];
	const double radius = 0.01 * input[2];
	const double dev = 0.01 * input[3];
	const double pitch = input[4];

	m_output[0] = 
		granular_base::compute_step(seed, width, radius, dev, dev, pitch, dt);
}

double granular_component::grain_value(const double phase)
{
	return	wav_get_value(m_sample, phase, 0);
}

unsigned int granular_component::save_state(data_output_stream & data)
{
	unsigned int path_len = m_sample_path.size();
	char path[256];

	std::memcpy(path, m_sample_path.c_str(), path_len);

	data.write(&path_len, sizeof(unsigned int));
	data.write(path, path_len);
	
	return sizeof(unsigned int) + path_len;
}


class granular_factory : public plugin_factory {

	public:
		granular_factory()
			: plugin_factory("granular", "Experimental", granular_id)
		{}
		~granular_factory() {}

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

			//	Todo check !!!
			source.read(&path_len, sizeof(unsigned int));
			source.read(path, path_len);
			path[path_len] = 0;	//	null terminated string

			std::string str_path(path);

			wav_t *sample = wav_load(path);
			if (sample == nullptr)
				throw std::runtime_error("Unable to load " + str_path);

			return new
				granular_component(
					sample,
					str_path,
					32,
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

			return new 
				granular_component(
					sample,
					path,
					32, 
					channel_count);
		}
};

EXPORT_FACTORY(granular_factory)
