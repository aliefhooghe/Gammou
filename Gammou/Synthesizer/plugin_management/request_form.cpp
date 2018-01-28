#include "request_form.h"


namespace Gammou {

	namespace Sound {



		request_descriptor::request_descriptor(const std::string & name, const std::string & description)
			: m_name(name), m_description(description)
		{
		}

		const std::string & request_descriptor::get_name() const
		{
			return m_name;
		}

		const std::string & request_descriptor::get_description() const
		{
			return m_description;
		}

		//////////////




		file_request_descriptor::file_request_descriptor(const std::string & name, const std::string & description, const std::string & extension)
			: request_descriptor(name, description), m_extension(extension)
		{
		}

		const std::string & file_request_descriptor::get_extension() const
		{
			return m_extension;
		}

		integer_request_descriptor::integer_request_descriptor(const std::string & name, const std::string & description, const int min, const int max)
			: request_descriptor(name, description), m_min(min), m_max(max)
		{
		}

		int integer_request_descriptor::get_min() const
		{
			return m_min;
		}

		int integer_request_descriptor::get_max() const
		{
			return m_max;
		}


		/////////////////////////////

		request_form_descriptor::request_form_descriptor()
		{
		}

		unsigned int request_form_descriptor::get_request_count() const
		{
			return static_cast<unsigned int>(m_index.size());
		}

		void request_form_descriptor::add_file_request(const std::string & name, const std::string & description, const std::string & ext)
		{
			m_file_request.emplace_back(name, description, ext);
			m_index.push_back(std::make_pair(request_form_field_type::FILE_PATH, static_cast<unsigned int>(m_file_request.size()) - 1));
		}

		void request_form_descriptor::add_int_request(const std::string & name, const std::string & description, const int min, const int max)
		{
			m_integer_request.emplace_back(name, description, min, max);
			m_index.push_back(std::make_pair(request_form_field_type::INTEGER, static_cast<unsigned int>(m_integer_request.size()) - 1));
		}

		void request_form_descriptor::add_enum_request(const std::string& name, const std::string& description, const std::vector<std::string>& choices)
		{
			m_enum_request.emplace_back(name, description, choices);
			m_index.push_back(std::make_pair(request_form_field_type::ENUM, static_cast<unsigned int>(m_enum_request.size()) - 1));
		}

		const request_form_field_type request_form_descriptor::get_field_type(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			
			return m_index[index].first;
		}

		const file_request_descriptor & request_form_descriptor::get_file_request(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			else if (m_index[index].first != request_form_field_type::FILE_PATH)
				throw std::domain_error("request type is not FILE_PATH");
			
			const unsigned int i = m_index[index].second;
			return m_file_request[i];
		}

		const integer_request_descriptor & request_form_descriptor::get_integer_request(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			else if (m_index[index].first != request_form_field_type::INTEGER)
				throw std::domain_error("request type is not INTEGER");
			const unsigned int i = m_index[index].second;
			return m_integer_request[i];
		}

		const enum_request_descriptor & request_form_descriptor::get_enum_request(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			else if (m_index[index].first != request_form_field_type::ENUM)
				throw std::domain_error("request type is not ENUM");
			const unsigned int i = m_index[index].second;
			return m_enum_request[i];
		}


		enum_request_descriptor::enum_request_descriptor(const std::string & name, const std::string & description, const std::vector<std::string>& choices)
			: request_descriptor(name, description), m_choices(choices)
		{
		}

		const std::vector<std::string>& enum_request_descriptor::get_choices() const
		{
			return m_choices;
		}

		////////////////////


		answer_form_descriptor::answer_form_descriptor()
		{
		}

		void answer_form_descriptor::add_file_path(const std::string & file_path)
		{
			m_file_path.emplace_back(file_path);
			m_index.push_back(std::make_pair(request_form_field_type::FILE_PATH, static_cast<unsigned int>(m_file_path.size()) - 1));
		}

		void answer_form_descriptor::add_integer(const int integer)
		{
			m_integer.push_back(integer);
			m_index.push_back(std::make_pair(request_form_field_type::INTEGER, static_cast<unsigned int>(m_integer.size()) - 1));
		}

		void answer_form_descriptor::add_enum_choice(const unsigned int choice)
		{
			m_integer.push_back(choice);
			m_index.push_back(std::make_pair(request_form_field_type::ENUM, static_cast<unsigned int>(m_enum_choice.size()) - 1));
		}

		const std::string & answer_form_descriptor::get_file_path(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			else if (m_index[index].first != request_form_field_type::FILE_PATH)
				throw std::domain_error("answer type is not FILE_PATH");

			const unsigned int i = m_index[index].second;
			return m_file_path[i];
		}

		const int answer_form_descriptor::get_integer(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			else if (m_index[index].first != request_form_field_type::INTEGER)
				throw std::domain_error("answer type is not INTEGER");
			const unsigned int i = m_index[index].second;
			return m_integer[i];
		}

		const unsigned int answer_form_descriptor::get_enum_choice(const unsigned int index) const
		{
			if (index >= m_index.size())
				throw std::range_error("Invalid index");
			else if (m_index[index].first != request_form_field_type::ENUM)
				throw std::domain_error("request type is not ENUM");
			const unsigned int i = m_index[index].second;
			return m_enum_choice[i];
		}

} /* Sound */

} /* Gammou */