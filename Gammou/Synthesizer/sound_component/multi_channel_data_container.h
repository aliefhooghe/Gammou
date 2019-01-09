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
			multi_channel_array(
				polyphonic_sound_component *owner, 
				const unsigned int size);
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

		/*
		*		Multi Channel Queue
		*/

		template<class T>
		class multi_channel_queue : private multi_channel_data {

		public:
			multi_channel_queue(
				polyphonic_sound_component *owner, 
				const unsigned int capacity);

			inline void operator<<(const T& rvalue);	//	enqueue
			inline void operator>>(T& lvalue);			//	dequeue
			inline T operator[](const unsigned int offset);	//	value from back

			void reset();
		private:
			const unsigned int m_capacity; // todo : allow resize

			std::vector<std::pair<unsigned int, unsigned int> > m_index; // r, w
			std::vector<T> m_data;
		};

		template<class T>
		multi_channel_queue<T>::multi_channel_queue(polyphonic_sound_component * owner, const unsigned int capacity)
			: multi_channel_data(owner),
				m_capacity(capacity),
				m_index(owner->get_channel_count(), std::make_pair(0u, 1u)),
				m_data(owner->get_channel_count() * m_capacity)
				
		{
		}

		template<class T>
		inline void multi_channel_queue<T>::operator<<(const T & rvalue)
		{
			const unsigned int channel = 
				multi_channel_data::get_current_working_channel();
			const unsigned int base = m_capacity * channel;
			auto index = m_index[channel];

			m_data[base + index.second] = rvalue;
			
			const unsigned int new_write_pos = (index.second + 1) % m_capacity;

			if (index.first == index.second) {
				m_index[channel] = std::make_pair(new_write_pos, new_write_pos);
			}
			else {
				m_index[channel].second = new_write_pos;
			}
		}

		template<class T>
		inline void multi_channel_queue<T>::operator>>(T & lvalue)
		{
			const unsigned int channel = 
				multi_channel_data::get_current_working_channel();
			const unsigned int base = m_capacity * channel;
			auto index = m_index[channel];

			const unsigned int new_read_pos = (index.first + 1) % m_capacity;

			if (new_read_pos == index.second) {
				lvalue = T();
			}
			else {
				lvalue = m_data[base + new_read_pos];
				m_index[channel].first = new_read_pos;
			}
		}
		
		template<class T>
		inline T multi_channel_queue<T>::operator[](const unsigned int offset)
		{
			const unsigned int channel = 
				multi_channel_data::get_current_working_channel();
			const unsigned int base = m_capacity * channel;

			const auto& index = m_index[channel];
			const unsigned int size =
				(index.first < index.second) ?
				(index.second - index.first - 1)
				: ((m_capacity + index.second) - index.first - 1);
									
			if (offset >= size) {
				return T();
			}
			else {
				const int pos = 
					static_cast<int>(index.second) - 
						(static_cast<int>(offset) + 1);
				
				return m_data[base + (pos + m_capacity) % m_capacity];
			}
		}

		template<class T>
		inline void multi_channel_queue<T>::reset()
		{
			const unsigned int channel = 
				multi_channel_data::get_current_working_channel();

			m_index[channel] = std::make_pair(0u, 1u);
		}

} /* Sound */


} /* Gammou */





#endif /* MULTI_CHANNEL_DATA_MANAGER_H_ */
