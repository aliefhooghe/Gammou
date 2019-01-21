
#include <random>
#include <cstring>

#include "plugin_helper.h"
#include "granular_base.h"
#include "wav.h"

using namespace Gammou::Sound;


class rt_granular_component : 
	public sound_component,
	private granular_base {

public:
	rt_granular_component(
		const unsigned int grain_count,
		const unsigned int channel_count);

	~rt_granular_component();

	void initialize_process() override;
	void process(const double input[]) override;
	
protected:
	double grain_value(const double phase) override;

private:
	multi_channel_queue<double> m_queue;
};

rt_granular_component::rt_granular_component(
	const unsigned int grain_count,
	const unsigned int channel_count)
	:	sound_component("RTGranular", 6, 1, channel_count),
		granular_base(this, grain_count),
		m_queue(this, 44100 * 10)
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
	granular_base::init();
	m_queue.reset();
}

void rt_granular_component::process(const double input[])
{
	const double dt = get_sample_duration();

	const double seed = -input[1];
	const double width = input[2];
	const double radius = 0.01 * std::abs(input[3]);
	const double dev = 0.01 * std::abs(input[4]);
	const double pitch = input[5];

	m_queue << input[0];
	m_output[0] =												//	-1.0 compensate delay tape movement
		granular_base::compute_step(seed, width, radius, dev, dev, pitch - 1.0, dt);
}

double rt_granular_component::grain_value(const double phase)
{
	const double sign = (phase > 0.0) ? -1.0 : 1.0;
	const double sample_count =
		get_sample_rate() * std::abs(phase);

	const unsigned int index =
		static_cast<unsigned int>(sample_count);

	const double a = m_queue[index];
	const double b = m_queue[index + 1];
	const double coef =
		sample_count - static_cast<double>(index);

	return
		sign * ((1.0 - coef) * a + coef * b);
}


class rt_granular_factory : public plugin_factory {

	public:
		rt_granular_factory()
			: plugin_factory("RTGranular", "Experimental", rt_granular_id)
		{}
		~rt_granular_factory() {}

	protected:

		abstract_sound_component *create_sound_component(
			data_input_stream& source, 
			const unsigned int channel_count) override
		{
			return new
				rt_granular_component(
					12,
					channel_count);
		}

		abstract_sound_component *create_sound_component(
			const answer_form& answer_form, 
			const unsigned int channel_count) override
		{
			return new 
				rt_granular_component(
					12, 
					channel_count);
		}
};

EXPORT_FACTORY(rt_granular_factory)


