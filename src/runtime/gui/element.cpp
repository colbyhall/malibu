#include "element.hpp"

#include "basic_shapes.hpp"
#include "math/aabb2.hpp"

namespace gui {
	Option<SlotBase const&> CompoundElement::slot_ref_at(usize index) const {
		auto* mut_this = const_cast<CompoundElement*>(this);
		auto result = mut_this->slot_mut_at(index);
		if (result) return result.unwrap();
		return NONE;
	}

	void Panel::on_paint(draw2d::Canvas& canvas) const {
		auto& layout = m_layout.as_ref().unwrap();

		const auto local_position = Vec3f32{ layout.local_position, 1.f };
		const auto local_size = Vec3f32{ layout.local_size, 0.f };

		const auto absolute_position = (layout.local_to_absolute * local_position).xy;
		const auto absolute_size = (layout.local_to_absolute * local_size).xy;

		const auto min = absolute_position;
		const auto max = min + absolute_size;

		canvas.set_color(m_color);
		canvas.paint(draw2d::Rect(AABB2f32::from_min_max(min, max)));
	}
}