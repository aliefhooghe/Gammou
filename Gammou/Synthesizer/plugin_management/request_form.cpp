#include "request_form.h"


namespace Gammou {

	namespace Sound {

		//	Abstract request field implementation

		abstract_request_field::abstract_request_field(const std::string & name, const std::string & description)
			: m_name(name), m_description(description)
		{
		}

		const std::string & abstract_request_field::get_name() const
		{
			return m_name;
		}

		const std::string & abstract_request_field::get_description() const
		{
			return m_description;
		}

		const range_request_field & abstract_request_field::get_range_request() const
		{
			throw std::runtime_error("Request field is not a range request");
		}

		const choice_request_field & abstract_request_field::get_choice_request_field() const
		{
			throw std::runtime_error("Request field is not a choice request");
		}

		const file_path_request_field & abstract_request_field::get_file_path_request() const
		{
			throw std::runtime_error("Request field is not a file path request");
		}

		//----------

		// Range request implementation

		range_request_field::range_request_field(const std::string & name, const std::string & description, const int min, const int max)
			: abstract_request_field(name, description),
			m_min(min), m_max(max)
		{
		}

		const abstract_request_field::type range_request_field::get_type() const
		{
			return type::RANGE;
		}

		const range_request_field & range_request_field::get_range_request() const
		{
			return (*this);
		}

		const int range_request_field::get_min() const
		{
			return m_min;
		}

		const int range_request_field::get_max() const
		{
			return m_max;
		}

		//-----------

		// File Path request implementation

		file_path_request_field::file_path_request_field(const std::string & name, const std::string & description, const std::string extension)
			: abstract_request_field(name, description),
			m_file_extension(extension)
		{
		}

		const  abstract_request_field::type file_path_request_field::get_type() const
		{
			return type::FILE_PATH;
		}

		const file_path_request_field & file_path_request_field::get_file_path_request() const
		{
			return (*this);
		}

		const std::string & file_path_request_field::get_file_extension() const
		{
			return m_file_extension;
		}

		//---------------

		// Choice request implementation

		choice_request_field::choice_request_field(const std::string & name, const std::string& description)
			: abstract_request_field(name, description)
		{
		}

		const abstract_request_field::type choice_request_field::get_type() const
		{
			return type::CHOICE;
		}

		const choice_request_field & choice_request_field::get_choice_request_field() const
		{
			return (*this);
		}

		const unsigned int choice_request_field::get_choice_count() const
		{
			return static_cast<unsigned int>(m_choices.size());
		}

		const std::string & choice_request_field::get_choice_name(const unsigned int index) const
		{
			// TODO: safer
			return m_choices[index];
		}


		////////////////////////////////////////////
		////////////////////////////////////////////

		// Abstract request form implementation

		abstract_request_form::abstract_request_form(const std::string & name)
			: m_name(name)
		{
		}

		const std::string & abstract_request_form::get_name() const
		{
			return m_name;
		}

		const field_list_request_form & abstract_request_form::get_field_list_request_form() const
		{
			throw std::runtime_error("Request form is not a field list request");
		}


		//---------

		// Choice node request form implementation

		const choice_node_request_form & abstract_request_form::get_choice_node_request_form() const
		{
			throw std::runtime_error("Request form is not a choice node request");
		}

		field_list_request_form::field_list_request_form(const std::string & name)
			: abstract_request_form(name)
		{
		}

		const abstract_request_form::type field_list_request_form::get_type() const
		{
			return type::FIELD_LIST;
		}

		const field_list_request_form & field_list_request_form::get_field_list_request_form() const
		{
			return (*this);
		}


		const unsigned int field_list_request_form::get_field_count() const
		{
			return static_cast<unsigned int>(m_fields.size());
		}

		const abstract_request_field & field_list_request_form::get_request_field(const unsigned int index) const
		{
			return *(m_fields[index]);
		}

		//-----------

		// Choice node request form implementation

		choice_node_request_form::choice_node_request_form(const std::string & name)
			: abstract_request_form(name)
		{
		}

		const unsigned int choice_node_request_form::get_choice_count() const
		{
			return static_cast<unsigned int>(m_choices.size());
		}

		const abstract_request_form & choice_node_request_form::get_choice(const unsigned int index) const
		{
			return *(m_choices[index]);
		}

		const abstract_request_form::type choice_node_request_form::get_type() const
		{
			return type::CHOICE_NODE;
		}

		const choice_node_request_form & choice_node_request_form::get_choice_node_request_form() const
		{
			return (*this);
		}

		//-----------

		// Empty request form implementation

		empty_request_form::empty_request_form()
			: abstract_request_form("")
		{
		}

		const abstract_request_form::type empty_request_form::get_type() const
		{
			return type::EMPTY;
		}

		//////////////////////////////////////
		//////////////////////////////////////


		/*
			Interface for request field creation
		*/

		abstract_request_field * create_range_request(const std::string & name, const std::string & description, const int min, const int max)
		{
			return new range_request_field(name, description, min, max);
		}

		abstract_request_field * create_file_path_request(const std::string & name, const std::string & description, const std::string & extension)
		{
			return new file_path_request_field(name, description, extension);
		}

		/*
		Interface for request form creation
		*/

		std::unique_ptr<abstract_request_form> create_empty_request_form()
		{
			return std::unique_ptr<abstract_request_form>(new empty_request_form());
		}

		/*
		
		
		*/

		// form answer

		abstract_request_form::type empty_answer_form::get_form_type() const
		{
			return abstract_request_form::type::EMPTY;
		}

		// Field Answer

		range_answer::range_answer(const int integer)
			: m_integer(integer)
		{
		}

		abstract_request_field::type range_answer::get_field_type() const
		{
			return abstract_request_field::type::RANGE;
		}

		int range_answer::get_integer() const
		{
			return m_integer;
		}


		choice_answer::choice_answer(const unsigned int choice_id)
			: m_choice_id(choice_id)
		{
		}

		abstract_request_field::type choice_answer::get_field_type() const
		{
			return abstract_request_field::type::CHOICE;
		}

		unsigned int choice_answer::get_choice_id() const
		{
			return m_choice_id;
		}

		file_path_answer::file_path_answer(const std::string & path)
			: m_path(path)
		{
		}

		abstract_request_field::type file_path_answer::get_field_type() const
		{
			return abstract_request_field::type::FILE_PATH;
		}

		const std::string& file_path_answer::get_path() const
		{
			return m_path;
		}





} /* Sound */

} /* Gammou */