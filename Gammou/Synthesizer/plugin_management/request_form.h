#ifndef GAMMOU_REQUEST_FORM_H_
#define GAMMOU_REQUEST_FORM_H_

#include <string>
#include <variant>
#include <deque>
#include <memory>

namespace Gammou {

	namespace Sound {

		//	Helper

		template<class Container, class T>
		void container_append(
			Container& container, T && x)
		{
			container.push_back(std::move(x));
		}

		template<class Container, class Tfirst, class ...Tnext>
		void container_append(
			Container& container,
			Tfirst && first,
			Tnext && ...next)
		{
			container_append(container, std::move(first));
			container_append(container, std::forward<Tnext&&>(next)...);   
		}

		//	Requests

		struct range_request {
			const int min;
			const int max;
		};

		struct path_request {
			const std::string extension;
		};

		struct choice_request {
			const std::deque<std::string> choices;
		};

		struct request {
			const std::string name;
			const std::string description;
			const std::variant <
					range_request,
					path_request,
					choice_request > req;
		};

		struct empty_request_form {};
		typedef std::deque<request> request_list;

		typedef std::variant<
			empty_request_form,
			request_list> request_form;

		template<class ...Requests>
		std::unique_ptr<request_form>
			create_request_form(Requests && ...requests)
		{
			auto ret =
				std::make_unique<request_form>(request_list{});
			auto& list = std::get<request_list>(*ret);
			container_append(list, std::forward<Requests&&>(requests)...);
            return ret;
		}

		//	Answers

		struct range_answer {
			const int value; 
		};

		struct path_answer {
			const std::string path;
		};

		struct choice_answer {
			const unsigned int choice;
		};

		typedef std::variant<
					range_answer,
					path_answer,
					choice_answer> answer;

		struct empty_answer_form {};
		typedef std::deque<answer> answer_list;

		typedef std::variant<
					empty_answer_form,
					answer_list> answer_form;

		template<class ...Answers>
		std::unique_ptr<answer_form> create_answer_form(Answers && ...answers)
		{
			auto ret = 
				std::make_unique<answer_form>(answer_list{});
			
			auto& list = std::get<answer_list>(*ret);
			container_append(list, std::forward<Answers&&>(answers)...);
            return ret;
		}

	} /* Sound */

} /* Gammou */

#endif 
