#ifndef ABSTRACT_APPLICATION_DISPLAY_H_
#define ABSTRACT_APPLICATION_DISPLAY_H_

#include <string>
#include "abstract_display.h"

namespace Gammou {

	namespace View {


		class abstract_application_display : 
			virtual public abstract_display {

		public:
			abstract_application_display(View::widget& root_widget);
			virtual ~abstract_application_display();

			virtual void open(const std::string& title) =0;
			//virtual bool is_open();
		};


	} /* View */

} /* Gammou */


#endif