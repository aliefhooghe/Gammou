#ifndef ABSTRACT_VST3_DISPLAY_H_
#define ABSTRACT_VST3_DISPLAY_H_

#include "public.sdk/source/common/pluginview.h"
#include "abstract_display.h"

namespace Gammou {

	namespace View {

		class abstract_vst3_view;

		class abstract_vst3_display : virtual public abstract_display {

			friend class abstract_vst3_view;

		public:
			abstract_vst3_display(View::widget& root_widget);
			virtual ~abstract_vst3_display();

			virtual void open(void *host_parent_window) = 0;

			//	To be given to vst3 host
			virtual abstract_vst3_view *create_vst3_view_instance() = 0;

			// resize ?
		};


		class abstract_vst3_view : public Steinberg::CPluginView {

		public:
			abstract_vst3_view(abstract_vst3_display *display);
			virtual ~abstract_vst3_view();

			// CPluginView override
			Steinberg::tresult PLUGIN_API onWheel(float distance) override;
			Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyMsg, Steinberg::int16 modifiers) override;
			Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyMsg, Steinberg::int16 modifiers) override;

			void attachedToParent() override;
			void removedFromParent() override;

		protected:
			abstract_vst3_display * m_display;

			void set_size(const unsigned int width, const unsigned int height);
		};

	} /* View */

} /* Gammou */

#endif