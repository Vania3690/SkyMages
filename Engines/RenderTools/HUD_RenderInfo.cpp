#include "HUD_RenderInfo.hpp"
#include "MGL\Primitive\AbstractDrawableObject.hpp"
#include "MGL\Primitive\Primitive.hpp"
#include "MGL\SharedHeaders\Color.hpp"
#include "MGL\Math\Vector.hpp"

HUD_RenderInfo::HUD_RenderInfo(EnergyStorage *energy_source) : m_energy_source(energy_source) {
	mgl::Primitive *outter = new mgl::Primitive(mgl::VertexConnectionType::TriangleStrip,
								new mgl::Color(253u, 255u, 200u));
	outter->insert(mgl::math::vector(0.975f, 0.01f));
	outter->insert(mgl::math::vector(0.995f, 0.01f));
	outter->insert(mgl::math::vector(0.975f, 0.99f));
	outter->insert(mgl::math::vector(0.995f, 0.99f));

	mgl::Primitive *inner = new mgl::Primitive(mgl::VertexConnectionType::TriangleStrip,
							   new mgl::Color(182u, 123u, 166u));
	inner->insert(mgl::math::vector(0.978f, 0.015f));
	inner->insert(mgl::math::vector(0.992f, 0.015f));
	inner->insert(mgl::math::vector(0.978f, 0.985f));
	inner->insert(mgl::math::vector(0.992f, 0.985f));

	m_energy_bar = new mgl::Primitive(mgl::VertexConnectionType::TriangleStrip,
									  new mgl::Color(104u, 7u, 77u));

	get()->addPrimitive(outter);
	get()->addPrimitive(inner);
	get()->addPrimitive(m_energy_bar);
}

HUD_RenderInfo::~HUD_RenderInfo() {}

#include "Objects\EquipableItems\EnergyStorage.hpp"
#include "MGL\Primitive\Primitive.hpp"
void HUD_RenderInfo::recalculate() {
	auto current_capacity = m_energy_source->getCapacityPercent();
	if (m_current_energy_percent != current_capacity) {
		m_current_energy_percent = current_capacity;
		current_capacity *= 0.97f;
		current_capacity += 0.015f;
		current_capacity = 1.f - current_capacity;

		m_energy_bar->deleteAll();
		m_energy_bar->insert(mgl::math::vector(0.978f, current_capacity));
		m_energy_bar->insert(mgl::math::vector(0.992f, current_capacity));
		m_energy_bar->insert(mgl::math::vector(0.978f, 0.985f));
		m_energy_bar->insert(mgl::math::vector(0.992f, 0.985f));
		m_energy_bar->send(mgl::DataUsage::StreamDraw);
	}
}
