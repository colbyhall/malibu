#pragma once

#include "core.hpp"
#include "containers/shared_ref.hpp"

namespace gui {
	enum class Visibility {
		Visible,
		Hidden,
		Collapsed,
	};

	struct Layout {

	};

	class WidgetRegistration {
	public:
		WidgetRegistration(StringView name);

	private:
		StringView m_name;
	};

#define REGISTER_WIDGET(widget_name) \
	static gui::WidgetRegistration _registration_##widget_name = { #widget_name };

	class Widget {
	public:
		
	private:
		Visibility m_visibility;
		Option<SharedRef<Widget>> m_parent = nullptr;
		Option<Layout> m_layout = nullptr;
	};
}