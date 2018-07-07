#ifndef PLUGIN_REQUEST_DIALOG_H_
#define PLUGIN_REQUEST_DIALOG_H_

#include <view.h>
#include <widget/dialog.h>

#include <plugin_management/request_form.h>

namespace Gammou {

    namespace Gui {

        class request_widget : public View::panel<> {

            public:
                request_widget(
                    const Sound::request& request,
                    const int x, const int y, 
                    const unsigned int width = 300,
                    const unsigned int height = 30);

                virtual ~request_widget() {}

                virtual Sound::answer get_answer() =0;
        };

        class range_request_widget : public request_widget {

            public:
                range_request_widget(
                    const Sound::request& request,  //  must be a range request
                    const int x, const int y, 
                    const unsigned int width = 300);
                
                Sound::answer get_answer() override;
        };

        class choice_request_widget : public request_widget {

            public:
                choice_request_widget(
                    const Sound::request& request,
                    const int x, const int y, 
                    const unsigned int width = 300);
                
                Sound::answer get_answer() override;
            
            private:
                View::list_box *m_listbox;
        };

        class path_request_widget : public request_widget {

            public:
                path_request_widget(
                    const Sound::request& request,
                    const int x, const int y, 
                    const unsigned int width = 300);
                
                Sound::answer get_answer() override;
            
            private:
                View::file_explorer_dialog m_dialog;
        };

        class plugin_request_dialog : public View::dialog {

            public:
                plugin_request_dialog(const Sound::request_list& list);

                std::unique_ptr<Sound::answer_form> get_answer_form();
            private:
                std::deque<request_widget*> m_requests;
        };
    }   /* Gui */

}   /* Gammou */

#endif