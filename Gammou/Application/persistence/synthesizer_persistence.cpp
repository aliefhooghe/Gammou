
#include <algorithm>

#include "synthesizer_persistence.h"

namespace Gammou {

	namespace Persistence {

		buffer_data_sink::buffer_data_sink()
			: m_cursor(0u), m_buffer(0u)
		{
		}

		buffer_data_sink::~buffer_data_sink()
		{
		}

		bool buffer_data_sink::seek(const int offset, Sound::data_stream::seek_mode mode)
		{
			int new_cursor;

			switch (mode)
			{

				case Gammou::Sound::data_stream::seek_mode::CURRENT:
					new_cursor =
						static_cast<int>(m_cursor) + offset;
					break;

				case Gammou::Sound::data_stream::seek_mode::SET:
					new_cursor = static_cast<unsigned int>(offset);
					break;

				case Gammou::Sound::data_stream::seek_mode::END:
					return false; // Doesn't make sense here ( TODO Sure ??? )
					break;
			}

			if (new_cursor < 0)
				return false;
			else
				m_cursor = static_cast<unsigned int>(new_cursor);

			const unsigned int minimal_size = m_cursor + 1;

			if (m_buffer.size() < minimal_size)
				m_buffer.resize(minimal_size, 0u);

			return true;
		}

		unsigned int buffer_data_sink::tell()
		{
			return m_cursor;
		}

		unsigned int buffer_data_sink::write(void * data, const unsigned int size)
		{
			const unsigned new_cursor = m_cursor + size;
			const unsigned new_minimal_size = new_cursor + 1;

			if (m_buffer.size() < new_minimal_size)
				m_buffer.resize(new_minimal_size, 0u);

			std::memcpy(m_buffer.data() + m_cursor, data, size);
			m_cursor = new_cursor;

			return size;
		}

		void buffer_data_sink::flush_data(Sound::data_sink & target)
		{
			// TODO more verif
			if (m_buffer.size() > 1) {
				const unsigned int size = static_cast<unsigned int>(m_buffer.size() - 1);
				target.write(m_buffer.data(), size);
			}

			m_buffer.resize(0);
			m_cursor = 0;
		}

		//----------------

		constrained_data_source::constrained_data_source(Sound::data_source & data, 
			const unsigned int max_forward_offset)
			: m_start_offset(data.tell()),
				m_max_forward_offset(max_forward_offset),
				m_data(data)
		{

		}

		bool constrained_data_source::seek(const int offset, Sound::data_stream::seek_mode mode)
		{
			int new_offset; 

			switch (mode)
			{
			case Sound::data_stream::seek_mode::CURRENT:
				new_offset = static_cast<int>(tell()) + offset;
				break;

			case Sound::data_stream::seek_mode::SET:
				new_offset = offset;
				break;

			case Sound::data_stream::seek_mode::END:
				new_offset = static_cast<int>(m_max_forward_offset) + offset;
				break;
			}

			if (new_offset >= 0) {
				const unsigned int unsigned_new_offset = static_cast<unsigned int>(new_offset);
				if(unsigned_new_offset <= m_max_forward_offset)
					return m_data.seek(m_start_offset + new_offset, Sound::data_stream::seek_mode::SET);
			}

			return false;
		}

		unsigned int constrained_data_source::tell()
		{
			return m_data.tell() - m_start_offset;
		}

		unsigned int constrained_data_source::read(void * data, const unsigned int size)
		{
			const unsigned int max_read_size = m_max_forward_offset - tell();
			const unsigned int read_size = std::min(max_read_size, size);
			return m_data.read(data, read_size);
		}


		//----------------

		bool record_id_is_internal_component(const uint32_t component_record_id)
		{
			return INTERNAL_COMPONENT_ID_MASK ==
				(INTERNAL_COMPONENT_ID_MASK & component_record_id);
		}

		uint8_t internal_component_id_by_record_id(const uint32_t component_record_id)
		{
			return (0xFF & component_record_id);
		}

		uint32_t record_id_by_internal_component_id(const uint8_t internal_component_id)
		{
			return (INTERNAL_COMPONENT_ID_MASK | internal_component_id);
		}

	} /* Persistence */

} /* Persistence */

