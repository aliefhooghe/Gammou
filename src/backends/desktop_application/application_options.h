#ifndef GAMMOU_APPLICATION_OPTIONS_H_
#define GAMMOU_APPLICATION_OPTIONS_H_

#include "desktop_application.h"

namespace Gammou
{
    struct application_options
    {
        bool no_gui{false};
        Gammou::desktop_application::configuration configuration;
    };

} // namespace Gammou


#endif /* GAMMOU_APPLICATION_OPTIONS_H_ */