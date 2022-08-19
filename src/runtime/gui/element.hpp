#pragma once 

#include "math/mat3.hpp"
#include "math/color.hpp"
#include "containers/shared_ref.hpp"
#include "containers/function.hpp"

namespace draw {
	class Canvas;
	class Font;
}

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
	
	class Element {
	public:
		NO_DISCARD inline Visibility visibility() const { return m_visibility; }
		NO_DISCARD inline Option<SharedRef<Element> const&> parent() const { return m_parent.as_ref(); }
		NO_DISCARD inline Option<Layout> layout() const { return m_layout; }

		virtual void on_layout(const Layout& layout) { m_layout = layout; };
		virtual void on_paint(draw::Canvas& canvas) const {}

		Option<Layout> m_layout = nullptr;

	private:
		friend class Window;

		Visibility m_visibility = Visibility::Visible;
		Option<SharedRef<Element>> m_parent = nullptr;
	};

#define DECLARE_ELEMENT(widget, parent)													\
public:																					\
	template<typename... Args>															\
	static SharedRef<widget> make(Args&&... args) {										\
		return SharedRef<widget>::make(widget{ forward<Args>(args)... });				\
	}

#define DECLARE_ABSTRACT_ELEMENT(widget, parent) public:

	class SlotBase {
	public:
		SlotBase(SharedRef<Element>&& element) 
			: m_element(core::forward<SharedRef<Element>>(element)) {}

		SharedRef<Element>& element_mut() { return m_element; }
		SharedRef<Element> const& element_ref() const { return m_element; }
	private:
		SharedRef<Element> m_element;
	};

	class ContainerElement : public Element {
		DECLARE_ABSTRACT_ELEMENT(ContainerElement, Element);

		Option<SlotBase const&> slot_ref_at(usize index) const;
		virtual Option<SlotBase&> slot_mut_at(usize index) = 0;
		virtual usize slot_count() const = 0;
	};

	struct Margin {
		f32 left;
		f32 bottom;
		f32 right;
		f32 top;

		constexpr inline Margin() : Margin(2.f) {}
		constexpr inline Margin(f32 all)
			: left(all), bottom(all), right(all), top(all) {}
		constexpr inline Margin(f32 _left, f32 _bottom, f32 _right, f32 _top)
			: left(_left), bottom(_bottom), right(_right), top(_top) {}
	};
	using Padding = Margin;

	enum class Alignment {
		Center,
		Min,
		Max,
		Fill,

		Bottom = Min,
		Top = Max,

		Left = Min,
		Right = Max,
	};

	struct Alignment2 {
		Alignment vertical;
		Alignment horizontal;
	};

	struct Sizing {
		enum Type {
			Automatic,
			Filling
		};
		Type type = Automatic;
		f32 fill = 0.f;
	};

	class Panel : public Element {
		DECLARE_ELEMENT(Panel, Element)

	protected:
		void on_paint(draw::Canvas& canvas) const override;

	private:
		Panel(LinearColor color) : m_color(color) {}

		LinearColor m_color;
	};



	
}