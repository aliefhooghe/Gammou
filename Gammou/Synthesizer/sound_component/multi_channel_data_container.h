#ifndef MULTI_CHANNEL_DATA_CONTAINER_H_
#define MULTI_CHANNEL_DATA_CONTAINER_H_

#include <vector>
#include "sound_component_manager.h"
#include "polyphonic_sound_component.h"

namespace Gammou {

	namespace Sound {


		/*
		* 	Base implementation for all multi-channel container
		*/

		class multi_channel_data {

		public:
			multi_channel_data(polyphonic_sound_component *owner);
			virtual ~multi_channel_data() {};

		protected:
			inline unsigned int get_channels_count() const { return m_channels_count; }
			inline unsigned int get_current_working_channel() const
			{
				return m_owner->get_current_working_channel();
			}

		private:
			unsigned int m_channels_count;
			polyphonic_sound_component *m_owner;
		};




		/*
		* 		multi channel variable
		*/

		template<class T>
		class multi_channel_variable : private multi_channel_data{

		public:
			multi_channel_variable(polyphonic_sound_component *owner);
			virtual ~multi_channel_variable() {}

			inline T operator =(const multi_channel_variable<T>& value);
			inline T operator =(const T& value);
			inline operator T&();

		protected:
			std::vector<T> m_data;
		};

		template<class T>
		multi_channel_variable<T>::multi_channel_variable(polyphonic_sound_component *owner)
			: multi_channel_data(owner),
			m_data(get_channels_count())
		{
		}

		template<class T>
		inline T multi_channel_variable<T>::operator=(const multi_channel_variable<T>& value)
		{
			const unsigned int channel = get_current_working_channel();
			return (m_data[channel] = value.m_data[channel]);
		}

		template<class T>
		inline T multi_channel_variable<T>::operator=(const T& value)
		{
			return (m_data[get_current_working_channel()] = value);
		}

		template<class T>
		inline multi_channel_variable<T>::operator T&()
		{
			return m_data[get_current_working_channel()];
		}

		/*
		* 		multi channel array
		*/

		template<class T>
		class multi_channel_array : private multi_channel_data{

		public:
			multi_channel_array(polyphonic_sound_component *owner, const unsigned int size);
			virtual ~multi_channel_array() {}

			inline T& operator [](const unsigned int index);
			inline unsigned int size() { return m_size;  }
		private:
			const unsigned int m_size;
			std::vector<T> m_data;
		};

		template<class T>
		multi_channel_array<T>::multi_channel_array(polyphonic_sound_component *owner, const unsigned int size)
			: multi_channel_data(owner),
			m_size(size),
			m_data(get_channels_count() * size)
		{
		}

		template<class T>
		inline T& multi_channel_array<T>::operator [](const unsigned int index)
		{
			// Verif ou perf -> Perf !! 
			const unsigned int base = m_size * multi_channel_data::get_current_working_channel();
			return m_data[base + index];
		}


	} /* Sound */


} /* Gammou */





#endif /* MULTI_CHANNEL_DATA_MANAGER_H_ */
