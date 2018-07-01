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
			~win32_application_display();

			void open(const std::string& title) override;
			void close() override;

		private:
			static void win_event_loop(win32_application_display *self);
			bool m_running;
			std::thread m_event_loop;
		};
		
	} /* View */

} /* Gammou */

#endif