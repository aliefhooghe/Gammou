#ifndef WIN32_APPLICATION_DISPLAY_H_
#define WIN32_APPLICATION_DISPLAY_H_

#include <thread>

#include "display/abstract_win32_display.h"
#include "display/abstract_application_display.h"

namespace Gammou {

	namespace View {

		class win32_application_display :
			public abstract_win32_display,
			public abstract_application_display {

		public:
			win32_application_display(View::widget& root_widget);
			win32_application_display(abstract_display &parent, View::widget& root_widget);
			~win32_application_display();

			void open(const std::string& title) override;
			void wait() override;
			void close() override;

		private:
			static void window_manager(win32_application_display *self);
			std::thread m_window_manager;
		};
		
	} /* View */

} /* Gammou */

#endif