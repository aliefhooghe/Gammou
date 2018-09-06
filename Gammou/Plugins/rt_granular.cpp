
#include <cstdlib>
#include <cstring>

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

class rt_granular_component : public sound_component {

public:
	rt_granular_component(
		const unsigned int grain_count,
		const unsigned int channel_count);

	~rt_granular_component();

	void initialize_process() override;
	void process(const double input[]) override;
		
private:
	inline const double random_grain_pos(
		const double seed,
		const double width)
	{
		const double r = ((double)(rand() % 65536)) / 65535.0;
		return seed + width * (2.0 * r - 1.0);
	}

	inline double queue_get_value(const double t)
	{
		const double sample_count = 
		get_sample_rate() * std::abs(t);

		const unsigned int index = 
			static_cast<unsigned int>(sample_count);

		const double a = m_queue[index];
		const double b = m_queue[index + 1];
		const double coef = 
			sample_count - static_cast<double>(index);

		return 
			(1.0 - coef) * a + coef * b;
	}

	const unsigned int m_grain_count;
	multi_channel_array<grain> m_grain;

	multi_channel_queue<double> m_queue;

	multi_channel_variable<double> m_time;
	multi_channel_variable<double> m_first_grain_time;
};

rt_granular_component::rt_granular_component(
	const unsigned int grain_count,
	const unsigned int channel_count)
	: sound_component("RTGranular", 6, 1, channel_count),
		m_grain_count(grain_count),
		m_grain(this, grain_count),
		m_queue(this, 44100 * 10),
		m_time(this),
		m_first_grain_time(this)
{
	set_input_name("In", 0);
	set_input_name("Delay", 1);
	set_input_name("Width", 2);
	set_input_name("Radius", 3);
	set_input_name("Dev", 4);
	set_input_name("Speed", 5);
}

rt_granular_component::~rt_granular_component()
{
	DEBUG_PRINT("rt_granular DTOR\n");
}

void rt_granular_component::initialize_process()
{
	for (unsigned int i = 0; i < m_grain_count; i++) {
		auto& grain = m_grain[i];
		grain.output_time = -10.0;
		grain.signal_time = 0.0;
	}

	m_time = 0.0;
	m_first_grain_time = 0.0;
	m_queue.reset();
}

void rt_granular_component::process(const double input[])
{
	const double dt = get_sample_duration();

	const double seed = input[1];
	const double width = input[2];
	const double radius = 0.01 * input[3];
	const double dev = 0.01 * input[4];

	double out = 0.0;

	m_queue << input[0];

	for (unsigned int i = 0; i < m_grain_count; i++) {
		auto& gr = m_grain[i];

		const double cursor = m_time - gr.output_time;
		out += 
			queue_get_value(cursor + gr.signal_time) *
				grain_env(cursor, dev);

		gr.signal_time += dt;

		if (cursor > ((double)m_grain_count * radius * 0.5)) {
			//	Respawn the grain
			m_first_grain_time += radius;
			gr.output_time = m_first_grain_time;
			gr.signal_time = random_grain_pos(seed, width);
		}
	}

	m_time += dt * input[5];
	m_output[0] = out;
}


class rt_granular_factory : public plugin_factory {

	public:
		rt_granular_factory()
			: plugin_factory("RTGranular", "", 447975152)
		{}
		~rt_granular_factory() {}

	protected:

		abstract_sound_component *create_sound_component(
			data_input_stream& source, 
			const unsigned int channel_count) override
		{
			return new
				rt_granular_component(
					32,
					channel_count);
		}

		abstract_sound_component *create_sound_component(
			const answer_form& answer_form, 
			const unsigned int channel_count) override
		{
			return new 
				rt_granular_component(
					32, 
					channel_count);
		}
};

EXPORT_FACTORY(rt_granular_factory)
