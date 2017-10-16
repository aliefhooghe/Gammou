/*
 * sound_module_manager.h
 *
 *  Created on: 2 oct. 2017
 *      Author: liefhooghe
 */

#ifndef SOUND_MODULE_MANAGER_H_
#define SOUND_MODULE_MANAGER_H_

#include "../../process/observer.h"

namespace gammou{

class abstract_sound_component;

class sound_component_manager : public process::subject<sound_component_manager> {


public:
	typedef enum {CHANNEL_CHANGE_NOTIFY, SAMPLE_RATE_NOTIFY} notification_tag;

	sound_component_manager(const unsigned int channel_count);  // securité a prevoir >0
	~sound_component_manager();

	unsigned int get_channel_count() const;
	unsigned int get_current_working_channel() const;
	double get_current_sample_rate() const;

	void set_current_working_channel(const unsigned int channel);
	void set_current_samplerate(const double sample_rate);

private:
	const unsigned int m_channel_count;
	unsigned int m_current_working_channel;
	double m_current_sample_rate;
};




}



#endif /* SOUND_MODULE_MANAGER_H_ */
