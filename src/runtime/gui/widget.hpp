#pragma once 

#include "core.hpp"

#include "math/mat3.hpp"
#include "math/color.hpp"
#include "containers/shared_ref.hpp"

#include "canvas.hpp"

namespace gui {
	enum class Visibility {
		Visible,
		Hidden,
		Collapsed,
	};

	struct Layout {
		Point2f32 local_position;
		Vec2f32 local_size;

		Mat3f32 local_to_absolute;
	};																				

	class Widget {
	public:
		NO_DISCARD inline Visibility visibility() const { return m_visibility; }
		NO_DISCARD inline Option<SharedRef<Widget> const&> parent() const { return m_parent.as_ref(); }
		NO_DISCARD inline Option<Layout> layout() const { return m_layout; }

	protected:
		virtual void on_layout(const Layout& layout) { m_layout = layout; };
		virtual void on_paint(draw2d::Canvas& canvas) const {}

		Option<Layout> m_layout = nullptr;

	private:
		friend class Window;

		Visibility m_visibility = Visibility::Visible;
		Option<SharedRef<Widget>> m_parent = nullptr;
	};

	class WidgetRegistration {
	public:
		WidgetRegistration(StringView name);

	private:
		StringView m_name;
	};

#define REGISTER_WIDGET(widget_name) \
	static gui::WidgetRegistration _registration_##widget_name = { #widget_name };

#define WIDGET(widget, parent)															\
private:																				\
	using Super = parent;																\
public:																					\
	template<typename... Args>															\
	static SharedRef<widget> make(Args&&... args) {										\
		return SharedRef<widget>::make(widget{ forward<Args>(args)... });				\
	}

#define ABSTRACT_WIDGET(widget, parent)													\
private:																				\
	using Super = parent;																\
public:	

	class Panel : public Widget {
		WIDGET(Panel, Widget)

	protected:
		void on_paint(draw2d::Canvas& canvas) const override;

	private:
		Panel(LinearColor color) : m_color(color) {}

		LinearColor m_color;
	};
	
}