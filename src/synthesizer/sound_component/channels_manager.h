/*
 * sound_module_manager.h
 *
 *  Created on: 2 oct. 2017
 *      Author: liefhooghe
 */

#ifndef SOUND_MODULE_MANAGER_H_
#define SOUND_MODULE_MANAGER_H_

#include "../../process/link.h"

namespace gammou{

class abstract_sound_component;

class channels_manager : private process::link_monitor<channels_manager> {

public:
	channels_manager(const unsigned int channel_count);  // securité a prevoir >0
	~channels_manager();

	void register_sound_component(abstract_sound_component *component);

	unsigned int get_channel_count() const;
	unsigned int get_current_working_channel() const;

	void set_current_working_channel(const unsigned int channel);

private:
	const unsigned int m_channel_count;
	unsigned int m_current_working_channel;
};



class channels_manager_link : public process::link<channels_manager> {

public:
	channels_manager_link();
	~channels_manager_link();

	unsigned int get_current_working_channel() const;

protected:
	void on_tick();

private:
	unsigned int m_current_working_channel;

};

}



#endif /* SOUND_MODULE_MANAGER_H_ */
