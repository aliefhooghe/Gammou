#ifndef GAMMOU_INTERNAL_NODE_WIDGET_H_
#define GAMMOU_INTERNAL_NODE_WIDGET_H_

#include "circuit_editor.h"

namespace Gammou {

    /**
     *  \brief this node widget is used to handle internal nodes,
     *      i.e. the nodes that are not created using a plugin
     */
    class internal_node_widget : public node_widget {
    public:
        internal_node_widget(
            const std::string& name,
            const std::string& identifier,
            DSPJIT::compile_node_class&);

        /**
         *  \brief the internal node widgets are serialized using
         *      string identifiers
         */
        nlohmann::json serialize() override;

    private:
        const std::string _identifier;
    };

}

#endif