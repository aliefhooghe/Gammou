#ifndef REQUEST_FORM_H_
#define REQUESGT_FORM_H_

#include <string>
#include <vector>

namespace Gammou {

	namespace Sound {

		enum class request_form_field_type {FILE_PATH, INTEGER, ENUM};

		class request_descriptor {
		public:
			request_descriptor(const std::string& name, const std::string& description);
			virtual ~request_descriptor() {}

			const std::string& get_name() const;
			const std::string& get_description() const;
		private:
			const std::string m_name;
			const std::string m_description;
		};

		class file_request_descriptor : public request_descriptor {

		public:
			file_request_descriptor(const std::string& name, const std::string& description, const std::string& extension);

			const std::string& get_extension() const;
		private:
			const std::string m_extension;
		};

		class integer_request_descriptor : public request_descriptor {

		public:
			integer_request_descriptor(const std::string& name, const std::string& description, const int min, const int max);

			int get_min() const;
			int get_max() const;
		private:
			const int m_min;
			const int m_max;
		};

		class enum_request_descriptor : public request_descriptor {

		public:
			enum_request_descriptor(const std::string& name, const std::string& description, const std::vector<std::string>& choices);

			const std::vector<std::string>& get_choices() const;
		private:
			const std::vector<std::string>& m_choices;
		};

		// todo class enum_request_descriptor

		
		/////

		class request_form {

		public:
			request_form();

			unsigned int get_request_count() const;
			void add_file_request(const std::string& name, const std::string& description, const std::string& ext);
			void add_int_request(const std::string& name, const std::string& description, const int min, const int max);
			void add_enum_request(const std::string& name, const std::string& description, const std::vector<std::string>& choices);

			const request_form_field_type get_field_type(const unsigned int index) const;
			const file_request_descriptor& get_file_request(const unsigned int index) const;
			const integer_request_descriptor& get_integer_request(const unsigned int index) const;
			const enum_request_descriptor& get_enum_request(const unsigned int index) const;
		private:
			std::vector<std::pair<request_form_field_type, unsigned int> > m_index;
			std::vector<file_request_descriptor> m_file_request;
			std::vector<integer_request_descriptor> m_integer_request;
			std::vector<enum_request_descriptor> m_enum_request;
		};




		///////


		class answer_form {
		
		public:
			answer_form();
			~answer_form() {}

			void add_file_path(const std::string& file_path);
			void add_integer(const int integer);
			void add_enum_choice(const unsigned int choice);

			const std::string& get_file_path(const unsigned int index) const;
			const int get_integer(const unsigned int index) const;
			const unsigned int get_enum_choice(const unsigned int index) const;

		private:
			std::vector<std::pair<request_form_field_type, unsigned int> > m_index;
			std::vector<std::string> m_file_path;
			std::vector<int> m_integer;
			std::vector<unsigned int> m_enum_choice;
		};

		


	} /* Sound */

} /* Gammou */

#endif 