
#include "plugin_request_dialog.h"

namespace Gammou {

    namespace Gui {

        request_widget::request_widget(
            const Sound::request& request,
            const int x, const int y, 
            const unsigned int width,
            const unsigned int height)
        :   View::panel<>(x, y, width, height, View::cl_lightgrey)
        {
            add_widget(
                std::make_unique<View::label>
                    (request.name, 
                        0, 0, 
                        width / 2,
                        height));
        }

        range_request_widget::range_request_widget(
            const Sound::request& request,
            const int x, const int y, 
            const unsigned int width)
        :   request_widget(request, x, y, width, 30)
        {
            //const Sound::range_request& req = 
            //    std::get<range_request>(request.req);
        }

        Sound::answer range_request_widget::get_answer()
        {
            throw std::runtime_error("Not implemented : TODO");
        }

        choice_request_widget::choice_request_widget(
            const Sound::request& request,
            const int x, const int y, 
            const unsigned int width)
        :   request_widget(request, x, y, width, 30)
        {
            const Sound::choice_request& req = 
                std::get<Sound::choice_request>(request.req);
            const unsigned int choice_count = 
                req.choices.size();

            auto listbox = 
                std::make_unique<View::list_box>(
                    width / 2, 0,
                    width / 2, choice_count * 20,
                    choice_count);

            for (auto& choice : req.choices)
                listbox->add_item(choice);

            resize(get_width(), choice_count * 20);

            m_listbox = listbox.get();
            listbox->select_item(0);    //  ensure that something is selected
            add_widget(std::move(listbox));
        }

        Sound::answer choice_request_widget::get_answer()
        {
            const int ret = 
                m_listbox->get_selected_item();

            if (ret < 0)
                throw std::runtime_error("Nothing was selected\n");
            else
                return Sound::choice_answer{static_cast<unsigned int>(ret)};
        }

        path_request_widget::path_request_widget(
            const Sound::request& request,
            const int x, const int y, 
            const unsigned int width)
        :   request_widget(request, x, y, width, 30),
                m_dialog("/home/liefhooghe/Musique")
        {
            //const Sound::path_request& req = 
            //    std::get<path_request>(request.req);

            add_widget(
                std::make_unique<View::push_button>(
                    [this](View::push_button *button)
                    {
                        m_dialog.show("Browse");
                    },
                    "Browse",
                    width / 2, 1,
                    width / 2, 28
                ));
        }

        Sound::answer path_request_widget::get_answer()
        {
            std::string filename;
            if (m_dialog.get_filename(filename))
                return Sound::path_answer{filename};
            else
                return Sound::path_answer{"none"};
        }

        //-----

        plugin_request_dialog::plugin_request_dialog(
            const Sound::request_list& list)
        :   View::dialog(300, 512)
        {
            unsigned int y = 0u;

            //  Add request widgets

            for (auto& request : list) {

                std::unique_ptr<request_widget> widget;

                if (std::holds_alternative<Sound::range_request>(request.req)) {
                    widget = 
                        std::make_unique<range_request_widget>(
                            request,
                                0, y,
                                get_width());
                }
                else if (std::holds_alternative<Sound::choice_request>(request.req)) {
                    widget = 
                        std::make_unique<choice_request_widget>(
                            request,
                            0, y,
                            get_width());
                }
                else { // path 
                    widget = 
                        std::make_unique<path_request_widget>(
                            request,
                            0, y, 
                            get_width());
                }

                y += widget->get_height();
                m_requests.push_back(widget.get());
                add_widget(std::move(widget));
            } 

            //  Add Validation button

            add_widget(
                std::make_unique<View::push_button>(
                    [this](View::push_button *self)
                    {
                        get_display()->non_blocking_close();
                    },
                    "OK",
                    0, y + 2,
                    get_width(),
                    30));
            
            resize(get_width(), y + 30 + 3);
        }

        std::unique_ptr<Sound::answer_form> plugin_request_dialog::get_answer_form()
        {
            auto form = 
                std::make_unique<Sound::answer_form>
                    (Sound::answer_list{});
            auto& l = std::get<Sound::answer_list>(*form);

            for(auto request : m_requests)
                l.push_back(request->get_answer());
        
            return std::move(form);
        }

    }   /* Gui */

}   /* Gammou */