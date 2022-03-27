#ifndef GAMMOU_ARGUMENT_PARSER_H_
#define GAMMOU_ARGUMENT_PARSER_H_

#include "application_options.h"

namespace Gammou
{
    bool parse_options(int argc, char **argv, application_options& options);
}

#endif /* GAMMOU_ARGUMENT_PARSER_H_ */