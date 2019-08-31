
#include <random>
#include <cstring>

#include "plugin_helper.h"
#include "wav.h"

using namespace Gammou::Sound;

#define GRAIN_COUNT 64

/*
namespace Gammou {

	namespace Sound {

		template <typename Impl>
		class granular_crtp {


			private:
				double grain_value(const double t)
				{
					return static_cast<Impl*>(this)->grain_value(t);
				}
		};
	}
}*/

struct grain
{
	double t_sample;
	double t_center;
	double speed_offset;
	double dev;
};

class granular2_component : public sound_component {

public:
	granular2_component(
		wav_t *sample,
		const std::string& sample_path,	//	for state serialization
		const unsigned int grain_count,
		const unsigned int channel_count);

	~granular2_component();

	void initialize_process() override;
	void process(const double input[]) override;

	unsigned int save_state(data_output_stream& data) override;

private:
	
	static double gr_rand(const double center, const double width)
	{
		static std::default_random_engine m_engine{};
		static std::uniform_real_distribution<double> m_distribution{-1, 1};
		return center + width * m_distribution(m_engine);
	}

	static double env(const double phase, const grain& g)
	{
		if (phase >= -g.dev && phase <= g.dev) {
			const double value = M_PI * phase / g.dev;
			return 0.42 - 0.5 * cos(2.0 * value) + 0.08 * cos(4.0 * value);
		}
		else {
			return 0.0;
		}

		//1.0 / (1.0 + (phase * phase) / (g.dev * g.dev));
		//exp(-phase * phase / (2.0 * g.dev * g.dev));
	}

	multi_channel_array<grain> m_grains;
	multi_channel_variable<double> m_time;
	multi_channel_variable<double> m_first_grain_center;

	wav_t *m_sample;
	std::string m_sample_path;
};

granular2_component::granular2_component(
	wav_t *sample,
	const std::string &sample_path,
	const unsigned int grain_count,
	const unsigned int channel_count)
	: sound_component("Granular2", 8, 1, channel_count),
	  m_grains(this, GRAIN_COUNT),
	  m_time(this),
	  m_first_grain_center(this),
	  m_sample(sample),
	  m_sample_path(sample_path)
{
	set_input_name("Pos", 0);
	set_input_name("PosW", 1);

	set_input_name("Radius", 2);
	set_input_name("RadiusW", 3);

	set_input_name("Dev", 4);
	set_input_name("DevW", 5);

	set_input_name("Speed", 6);
	set_input_name("SpeedW", 7);

	set_output_name("", 0);
}

granular2_component::~granular2_component()
{
	DEBUG_PRINT("Granular2 DTOR\n");
	wav_free(m_sample);
}

void granular2_component::initialize_process()
{
	m_time = 0.0;
	m_first_grain_center = 0.0;

	for (auto & grain : m_grains) {
		grain.t_sample = -10000.0;
		grain.t_center = -10000.0;
	}
}

void granular2_component::process(const double input[])
{
	const double dt = get_sample_duration();

	//	GEt inputs
	const auto pos = input[0];
	const auto pos_w = input[1];

	const auto radius = 0.01 * std::abs(input[2]);
	const auto radius_w  = 0.01 * std::abs(input[3]);
	
	const auto dev = 0.01 * std::abs(input[4]);
	const auto dev_w = 0.01 * std::abs(input[5]);
	
	const auto speed = input[6];
	const auto speed_w = input[7];

	double out = 0.0;

	const double grain_phase_treshold = 
		0.5 * static_cast<double>(GRAIN_COUNT) * radius;

	for (auto &grain : m_grains) {

		const double grain_phase = m_time - grain.t_center;
		
		//	Apply grain value to output signal
		out += 
			wav_get_value(m_sample, grain.t_sample, 0) * 
			env(grain_phase, grain);

		// Update grain
		grain.t_sample += dt * (speed + grain.speed_offset);
	
		// If the grain is far in the past
		if (grain_phase > grain_phase_treshold) {
			//	Respawn the grain
			grain.t_sample = gr_rand(pos, pos_w);
			grain.t_center = m_first_grain_center + gr_rand(radius, radius_w);

			grain.speed_offset = gr_rand(0.0, speed_w);
			grain.dev = gr_rand(dev, dev_w);

			m_first_grain_center = grain.t_center;
			//DEBUG_PRINT("Reset Grain\n");
		}
	}

	m_time += dt;
	m_output[0] = out;
}

unsigned int granular2_component::save_state(data_output_stream & data)
{
	unsigned int path_len = m_sample_path.size();
	char path[256];

	std::memcpy(path, m_sample_path.c_str(), path_len);

	data.write(&path_len, sizeof(unsigned int));
	data.write(path, path_len);
	
	return sizeof(unsigned int) + path_len;
}


class granular2_factory : public plugin_factory {

	public:
		granular2_factory()
			: plugin_factory("granular2", "Experimental", 424242)
		{}
		~granular2_factory() {}

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
				granular2_component(
					sample,
					str_path,
					GRAIN_COUNT,
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
				granular2_component(
					sample,
					path,
					GRAIN_COUNT, 
					channel_count);
		}
};

EXPORT_FACTORY(granular2_factory)
