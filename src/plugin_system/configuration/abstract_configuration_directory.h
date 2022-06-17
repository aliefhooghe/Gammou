#ifndef GAMMOU_CONFIGURATION_DIRECTORY_H_
#define GAMMOU_CONFIGURATION_DIRECTORY_H_

#include <memory>
#include <view.h>
#include "abstract_configuration_page.h"

namespace Gammou
{
    class abstract_configuration_directory : public abstract_configuration_page
    {
    public:
        ~abstract_configuration_directory() override = default;
        virtual std::unique_ptr<abstract_configuration_directory> create_directory(std::string &desired_name, std::shared_ptr<View::widget>&& widget) =0;
        virtual std::unique_ptr<abstract_configuration_page> create_page(std::string &desired_name, std::shared_ptr<View::widget>&& widget) =0;
    };

}

#endif /* GAMMOU_CONFIGURATION_DIRECTORY_H_ */