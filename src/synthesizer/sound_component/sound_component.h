#ifndef SOUD_MODULE_H_
#define SOUD_MODULE_H_


#include "../../process/process.h"

#include "channels_manager.h"

#include "multi_channel_data_container.h"
/*
 * 		Channels data types
 */

namespace gammou {


class abstract_sound_component : public process::abstract_component<double> {

	friend class channels_manager;
	friend class multi_channel_data;

public:

	abstract_sound_component(
						const std::string& name,
						const unsigned int input_count,
						const unsigned int output_count,
						const unsigned int channel_count,
						const bool is_multi_channel);

	virtual ~abstract_sound_component();

	unsigned int channel_count() const;
	bool is_multi_channel() const;

	unsigned int get_current_working_channel() const;
private:
	const bool m_is_multi_channel; // a laisser en premier (ordre d'initialisation)
	const unsigned int m_channels_count;
	channels_manager_link m_channels_manager_link;




};



class sound_component : public abstract_sound_component {

public:

	sound_component(const std::string& name,
			const unsigned int input_count,
			const unsigned int output_count,
			const unsigned int channel_count,
			const bool is_multi_channel);
	virtual ~sound_component();

	double fetch_output(const unsigned int output_id);

protected:
	multi_channel_array<double> m_output;

};






} /* gammou */




#endif /* SOUD_MODULE_H_ */
