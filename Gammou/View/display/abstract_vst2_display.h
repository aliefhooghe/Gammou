#ifndef ABSTRACT_VST2_DISPLAY_H_
#define ABSTRACT_VST2_DISPLAY_H_

#include "abstract_display.h"

namespace Gammou {

    namespace View {

        class abstract_vst2_display : virtual public abstract_display {

            public:
                abstract_vst2_display(View::widget& root_widget);
                virtual ~abstract_vst2_display();

                virtual void open(void *host_parent_window) = 0;

				int on_effEditKeyUp(const int32_t index, const int64_t value);
				int on_effEditKeyDown(const int32_t index, const int64_t value);
			
			private:
				static keycode convert_key(const int32_t index, const int64_t value);
        };

    }   /*  View */

} /* Gammou */

#endif