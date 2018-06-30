
#include "abstract_application_display.h"

namespace Gammou {

	namespace View {


		abstract_application_display::abstract_application_display(
			View::widget& root_widget)
		:	abstract_display(root_widget)
		{
		}

		abstract_application_display::~abstract_application_display()
		{
		}
	} /* View */
} /* Gammou */