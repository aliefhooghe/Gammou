#ifndef SOUD_MODULE_H_
#define SOUD_MODULE_H_


#include "../../process/process.h"

#include "channels_manager.h"

#include "multi_channel_data_container.h"
/*
 * 		Channels data types
 */

namespace gammou {

constexpr double DEFAULT_SAMPLE_RATE = 44100.0;
constexpr double DEFAULT_SAMPLE_DURATION = 1.0 / DEFAULT_SAMPLE_RATE;


class abstract_sound_component : public process::abstract_component<double>,
								 public process::observer<sound_component_manager> {

	friend class multi_channel_data;

public:

	abstract_sound_component(
						const std::string& name,
						const unsigned int input_count,
						const unsigned int output_count,
						const unsigned int channel_count);

	virtual ~abstract_sound_component();

	unsigned int get_channel_count() const;
	unsigned int get_current_working_channel() const;
	virtual void on_sample_rate_change() {};

protected:
	void on_notify(const unsigned notification_tag) override;

	double get_sample_duration() const;
	double get_sample_rate() const;

private:
	const unsigned int m_channels_count;
	unsigned int m_current_working_channel;

	double m_sample_rate;
	double m_sample_duration;
};



class sound_component : public abstract_sound_component {

public:

	sound_component(const std::string& name,
			const unsigned int input_count,
			const unsigned int output_count,
			const unsigned int channel_count);
	virtual ~sound_component();

	double fetch_output(const unsigned int output_id);

protected:
	multi_channel_array<double> m_output;

};






} /* gammou */




#endif /* SOUD_MODULE_H_ */