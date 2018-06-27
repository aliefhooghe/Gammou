#ifndef GAMMOU_ABSTRACT_APP_WINDOW_H_
#define GAMMOU_ABSTRACT_APP_WINDOW_H_

#include "../abstract_window.h"

namespace Gammou {

	namespace View {
        
        class abstract_app_window : public abstract_window {

            public:
                abstract_app_window(const unsigned int px_width, const unsigned int px_height)
                    : abstract_window(px_width, px_height) {}
                virtual ~abstract_app_window() {}

				virtual bool is_open() =0;
                virtual void open() =0;
        };

    }
}

#endif
