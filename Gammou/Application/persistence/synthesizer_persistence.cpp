
#include <cstring>
#include <algorithm>

#include "synthesizer_persistence.h"



namespace Gammou {

	namespace Persistence {


        component_state::component_state(Sound::data_input_stream& source)
        {
            load(source);
        }

        void component_state::load(Sound::data_input_stream& source)
        {
            source.read(factory_id);
            source.read(x_pos);
            source.read(y_pos);

            uint32_t data_size;
            source.read(data_size);

            if (data_size > 0) {
                data.resize(data_size);

                if (source.read(data.data(), data_size) != data_size)
                    throw std::runtime_error("Cannot read component data");
            }
        }

        void component_state::save(Sound::data_output_stream& dest)
        {
            dest.write(factory_id);
            dest.write(x_pos);
            dest.write(y_pos);

            uint32_t data_size =
                static_cast<uint32_t>(data.size());

            dest.write(data_size);

            if (data_size > 0) {
                if (dest.write(data.data(), data_size) != data_size)
                    throw std::runtime_error("Cannot write component data");
            }
        }

        //--

        circuit_state::circuit_state(Sound::data_input_stream& source)
        {
            load(source);
        }

        void circuit_state::load(Sound::data_input_stream& source)
        {
            uint32_t component_count, link_count;

            source.read(component_count);
            source.read(link_count);

            components.resize(0);
			components.reserve(component_count);

			links.resize(link_count);

            //  Load components
            for (unsigned int i = 0; i < component_count; ++i)
                components.emplace_back(source);

            //  Load links (trivail type)
			const unsigned int links_size =
				link_count * sizeof(link_state);
			if (source.read(links.data(), links_size) != links_size)
				throw std::runtime_error("Unable to read links");
        }

        void circuit_state::save(Sound::data_output_stream& dest)
        {
            uint32_t component_count =
                static_cast<uint32_t>(components.size());
            uint32_t link_count =
                static_cast<uint32_t>(links.size());

            dest.write(component_count);
            dest.write(link_count);

            for (auto& component : components)
                component.save(dest);

			//  link_state is a trivial type
			const unsigned int links_size =
				link_count * sizeof(link_state);
			if (dest.write(links.data(), links_size) != links_size)
				throw std::runtime_error("Unable to write links\n");
        }

        //--

        gammou_state::gammou_state(Sound::data_input_stream& source)
        {
            load(source);
        }

        void gammou_state::load(Sound::data_input_stream& source)
        {
            //  Read parameters
            uint32_t parameter_count;
            source.read(parameter_count);

            const unsigned int parameters_size =
                parameter_count * sizeof(double);

            parameters.resize(parameter_count);
            if (source.read(parameters.data(), parameters_size) != parameters_size)
                throw std::runtime_error("Cannot read parameters");

            //  Read Master Volume
            source.read(&master_volume, sizeof(master_volume));

            //  Set keyboard mode
            polyphonic_keyboard = true;

			//	Skip reserved bytes
			using seek_mode = Sound::data_input_stream::seek_mode;
			source.seek(gammou_state_reserved_size, seek_mode::CURRENT);

            //  Read Circuits
            master_circuit.load(source);
            polyphonic_circuit.load(source);
        }

        void gammou_state::save(Sound::data_output_stream& dest)
        {
            // Write parameters
            uint32_t parameter_count =
                static_cast<uint32_t>(parameters.size());
			const unsigned int parameter_size =
				parameter_count * sizeof(double);
            dest.write(parameter_count);

            if (dest.write(parameters.data(), parameter_size) != parameter_size)
                throw std::runtime_error("Cannot write parameters");

            //  Write Master Volume
            dest.write(&master_volume, sizeof(double));

			//	Zero on reserved bytes
			uint8_t zero[gammou_state_reserved_size] = { 0 };

			if (dest.write(zero, gammou_state_reserved_size) !=
				gammou_state_reserved_size)
				throw std::runtime_error("Cannot write gammou state reserved area");

            // Write Circuits
            master_circuit.save(dest);
            polyphonic_circuit.save(dest);
        }

        //---

        buffer_output_stream::buffer_output_stream(
            std::vector<uint8_t>& buffer,
            const unsigned int start_cursor)
        :   m_cursor(start_cursor),
            m_buffer(buffer)
        {
            if (start_cursor > m_buffer.size())
                throw std::range_error("Start Cursor cant be greater than buffer size");
        }

        buffer_output_stream::~buffer_output_stream()
        {}

        bool buffer_output_stream::seek(
                const int offset,
                Sound::abstract_data_stream::seek_mode mode)
        {
            int new_cursor;

            switch (mode)
            {

                case Gammou::Sound::abstract_data_stream::seek_mode::CURRENT:
                    new_cursor =
                        static_cast<int>(m_cursor) + offset;
                    break;

                case Gammou::Sound::abstract_data_stream::seek_mode::SET:
                    new_cursor = static_cast<int>(offset);
                    break;

                case Gammou::Sound::abstract_data_stream::seek_mode::END:
                    return false; // Doesn't make sense here ( TODO Sure ??? )
                    break;
            }

            if (new_cursor < 0)
                return false;
            else
                m_cursor = static_cast<unsigned int>(new_cursor);

            if (m_buffer.size() < m_cursor)
                m_buffer.resize(m_cursor, 0u);

            return true;
        }

        unsigned int buffer_output_stream::tell()
        {
            return m_cursor;
        }

        unsigned int buffer_output_stream::write(void *data, const unsigned int size)
        {
            const unsigned new_cursor = m_cursor + size;

            if (m_buffer.size() < new_cursor)
                m_buffer.resize(new_cursor, 0u);

            std::memcpy(m_buffer.data() + m_cursor, data, size);
            m_cursor = new_cursor;

            return size;
        }

        //----------------

        buffer_input_stream::buffer_input_stream(
            const std::vector<uint8_t>& buffer,
            const unsigned int start_cursor)
        :   m_cursor(start_cursor),
            m_buffer(buffer)
        {
            if (start_cursor > m_buffer.size())
                throw std::range_error("Start Cursor cant be greater than buffer size");
        }

        buffer_input_stream::~buffer_input_stream()
        {}

        bool buffer_input_stream::seek(
                const int offset, Sound::abstract_data_stream::seek_mode mode)
        {
            int new_cursor;
            const int buffer_size =
                static_cast<int>(m_buffer.size());

            switch (mode)
            {

                case Gammou::Sound::abstract_data_stream::seek_mode::CURRENT:
                    new_cursor =
                        static_cast<int>(m_cursor) + offset;
                    break;

                case Gammou::Sound::abstract_data_stream::seek_mode::SET:
                    new_cursor = static_cast<int>(offset);
                    break;

                case Gammou::Sound::abstract_data_stream::seek_mode::END:
                    new_cursor = buffer_size;
                    break;
            }

            if (new_cursor < 0 ||
                    new_cursor > buffer_size) {
                return false;
            }
            else {
                m_cursor = static_cast<unsigned int>(new_cursor);
                return true;
            }
        }

        unsigned int buffer_input_stream::tell()
        {
            return m_cursor;
        }

        unsigned int buffer_input_stream::read(void *data, const unsigned size)
        {
            const unsigned int buffer_size =
                static_cast<unsigned int>(m_buffer.size());

			const unsigned int new_cursor =
				m_cursor + size;

            if (new_cursor > buffer_size)
                return 0;

            std::memcpy(data, m_buffer.data() + m_cursor, size);
			m_cursor = new_cursor;

			return size;
        }

		//----------------

        constrained_input_stream::constrained_input_stream(Sound::data_input_stream & data,
			const unsigned int max_forward_offset)
			: m_start_offset(data.tell()),
				m_max_forward_offset(max_forward_offset),
				m_data(data)
		{

		}

        bool constrained_input_stream::seek(const int offset, Sound::abstract_data_stream::seek_mode mode)
		{
			int new_offset; 

			switch (mode)
			{
			case Sound::abstract_data_stream::seek_mode::CURRENT:
				new_offset = static_cast<int>(tell()) + offset;
				break;

			case Sound::abstract_data_stream::seek_mode::SET:
				new_offset = offset;
				break;

			case Sound::abstract_data_stream::seek_mode::END:
				new_offset = static_cast<int>(m_max_forward_offset) + offset;
				break;
			}

			if (new_offset >= 0) {
				const unsigned int unsigned_new_offset = static_cast<unsigned int>(new_offset);
				if(unsigned_new_offset <= m_max_forward_offset)
					return m_data.seek(m_start_offset + new_offset, Sound::abstract_data_stream::seek_mode::SET);
			}

			return false;
		}

        unsigned int constrained_input_stream::tell()
		{
			return m_data.tell() - m_start_offset;
		}

        unsigned int constrained_input_stream::read(void * data, const unsigned int size)
		{
			const unsigned int max_read_size = m_max_forward_offset - tell();
			const unsigned int read_size = std::min(max_read_size, size);
			return m_data.read(data, read_size);
		}


		//----------------

	} /* Persistence */

} /* Persistence */

