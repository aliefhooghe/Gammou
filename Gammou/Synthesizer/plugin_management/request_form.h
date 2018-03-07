#ifndef GAMMOU_REQUEST_FORM_H_
#define GAMMOU_REQUEST_FORM_H_

#include <string>
#include <vector>
#include <type_traits>
#include <memory>
#include <utility>

namespace Gammou {

	namespace Sound {

		/*
				Request Fields
		*/

		class range_request_field;
		class choice_request_field;
		class file_path_request_field;

		class abstract_request_field {

		public:
			enum class type { RANGE, CHOICE, FILE_PATH };

			abstract_request_field(const std::string& name, const std::string& description);
			virtual ~abstract_request_field() {}

			virtual const type get_type() const = 0;
			const std::string& get_name() const;
			const std::string& get_description() const;

			virtual const range_request_field& get_range_request() const;
			virtual const choice_request_field& get_choice_request_field() const;
			virtual const file_path_request_field& get_file_path_request() const;

		private:
			const std::string m_name;
			const std::string m_description;
		};


		class range_request_field : public abstract_request_field {

		public:
			range_request_field(const std::string& name, const std::string& description, const int min, const int max);
			~range_request_field() {}

			const type get_type() const override;
			const range_request_field& get_range_request() const override;

			const int get_min() const;
			const int get_max() const;
		private:
			const int m_min;
			const int m_max;
		};

		class choice_request_field : public abstract_request_field {

		public:
			choice_request_field(const std::string& name, const std::string& description);

			template<class Tname>
			void add_choice(const Tname& choice1)
			{
				m_choices.push_back(choice1);
			}

			template<class Tname1, class ...Tnextnames>
			void add_choice(const Tname1& choice1, const Tnextnames& ...nextchoices)
			{
				add_choice(choice1);
				add_choice(nextchoices...);
			}

			const type get_type() const override;
			const choice_request_field &get_choice_request_field() const override;
			const unsigned int get_choice_count() const;
			const std::string& get_choice_name(const unsigned int index) const;


		private:
			std::vector <std::string> m_choices;
		};

		class file_path_request_field : public abstract_request_field {

		public:
			file_path_request_field(const std::string& name, const std::string& description, const std::string extension);
			~file_path_request_field() {}

			const type get_type() const override;
			const file_path_request_field& get_file_path_request() const override;

			const std::string& get_file_extension() const;
		private:
			const std::string m_file_extension;
		};


		/*
				Request forms
		*/

		class field_list_request_form;
		class choice_node_request_form;
		class empty_request_form;

		class abstract_request_form {


		public:
			abstract_request_form(const std::string& name);
			virtual ~abstract_request_form() {}

			enum class type { EMPTY, FIELD_LIST, CHOICE_NODE };

			const std::string& get_name() const;
			virtual const type get_type() const = 0;

			virtual const field_list_request_form& get_field_list_request_form() const;
			virtual const choice_node_request_form& get_choice_node_request_form() const;

		private:
			const std::string m_name;
		};

		class field_list_request_form : public abstract_request_form {

		public:
			field_list_request_form(const std::string& name);

			const type get_type() const override;
			const field_list_request_form& get_field_list_request_form() const override;

			template<class Tfield1>
			void add_field(Tfield1 *new_field)
			{
				m_fields.emplace_back(new_field);
			}

			template<class Tfield1, class ...Tnextfield>
			void add_field(Tfield1 *new_field1, Tnextfield *...nextfield)
			{
				add_field(new_field1);
				add_field(nextfield...);
			}

			const unsigned int get_field_count() const;
			const abstract_request_field& get_request_field(const unsigned int index) const;

		private:
			std::vector<std::unique_ptr<abstract_request_field> > m_fields;
		};

		class choice_node_request_form : public abstract_request_form {

		public:
			choice_node_request_form(const std::string& name);

			template<class Tform1>
			void add_choice(std::unique_ptr<Tform1>&& form1)
			{
				m_choices.push_back(std::move(form1));
			}

			template<class Tform1, class ...Tnext>
			void add_choice(std::unique_ptr<Tform1>&& form1, Tnext&& ...next)
			{
				add_choice(std::move(form1));
				add_choice(std::forward<Tnext&&>(next)...);
			}

			const unsigned int get_choice_count() const;
			const abstract_request_form& get_choice(const unsigned int index) const;

			const type get_type() const override;
			const choice_node_request_form& get_choice_node_request_form() const override;

		public:
			std::vector<std::unique_ptr<abstract_request_form> > m_choices;
		};


		class empty_request_form : public abstract_request_form {
		public:
			empty_request_form();
			const type get_type() const override;
		};

		//----------

		/*
			Interface for request field creation
		*/

		abstract_request_field *create_range_request(const std::string& name, const std::string& description, const int min, const int max);
		abstract_request_field *create_file_path_request(const std::string& name, const std::string& description, const std::string& extension);

		template<class ...Tnames>
		abstract_request_field *create_choice_request(const std::string& name, const std::string& description, const Tnames& ...choices)
		{
			choice_request_field *field = new choice_request_field(name, description);
			field->add_choice(choices...);
			return field;
		}

		/*
			Interface for request form creation
		*/

		std::unique_ptr<abstract_request_form> create_empty_request_form();

		template<class ...Tfields>
		std::unique_ptr<abstract_request_form> create_field_list_request(const std::string& name, Tfields *...fields)
		{
			std::unique_ptr<field_list_request_form> form(new field_list_request_form(name));
			form->add_field(fields...);
			return form;
		}

		template<class ...Tforms>
		std::unique_ptr<abstract_request_form> create_choice_node_request_form(const std::string& name, Tforms&& ...forms)
		{
			std::unique_ptr<choice_node_request_form> form(new choice_node_request_form(name));
			form->add_choice(std::forward<Tforms&&>(forms)...);
			return form;
		}

		//-----------------
		//-----------------



		/*
			Draft : 

			-> Answer(field) = 
					match field with
						| Range -> int
						| FilePath -> String
						| Choice -> int

			-> Answer(form) =
					match form with
						| Empty -> void
						| FieldList(f1, f2, f3, ...) -> (Answer(f1), Answer(f2), Answer(f3), ...)
						| ChoiceForm(f1, f2, f3, ...) -> (int i, Answer(fi))

		*/


		//class abstract_answer_field 

		class abstract_form_answer {
		
		public:
			virtual ~abstract_form_answer() {}
			virtual abstract_request_form::type get_form_type() const = 0;
		};

		class empty_answer_form : public abstract_form_answer {

		public:
			virtual abstract_request_form::type get_form_type() const override;
		};

		class abstract_field_answer {

		public:
			virtual ~abstract_field_answer() {}
			virtual abstract_request_field::type get_field_type() const = 0;
		};

		class range_answer : public abstract_field_answer {
		
		public:
			range_answer(const int integer);

			abstract_request_field::type get_field_type() const;
			int get_integer() const;

		private:
			const int m_integer;
		};

		class choice_answer : public abstract_field_answer {

		public:
			choice_answer(const unsigned int choice_id);

			abstract_request_field::type get_field_type() const override;
			unsigned int get_choice_id() const;

		private:
			const unsigned int m_choice_id;
		};

		class file_path_answer : public abstract_field_answer {

		public:
			file_path_answer(const std::string& path);
			abstract_request_field::type get_field_type() const;

		private:
			const std::string m_path;
		};

	} /* Sound */

} /* Gammou */

#endif 