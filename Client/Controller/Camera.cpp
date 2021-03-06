#include "Camera.hpp"
#include "Objects/Actors/Actor.hpp"
#include "Objects/Map/Map.hpp"
#include "Shared/vector.hpp"

void Camera::correct_height() {
	if (auto temp = minY(); temp < 0) {
		m_center->at(1) -= temp;
		m_camera_was_changed = true;
	}
	if (auto temp = maxY(); temp >= m_map->height()) {
		m_center->at(1) -= temp - m_map->height();
		m_camera_was_changed = true;
	}
	if (int(m_horizontal_blocks_number / m_aspect_ratio) + 1 >= int(m_map->height())) {
		m_horizontal_blocks_number = float(m_map->height()) * m_aspect_ratio - 1;
		m_center->at(1) = m_horizontal_blocks_number / m_aspect_ratio / 2.f;
		m_camera_was_changed = true;
	}
}
void Camera::correct_width() {
	if (auto temp = minX(); temp < 0) {
		m_center->at(0) -= temp;
		m_camera_was_changed = true;
	}
	if (auto temp = maxX(); temp >= m_map->width()) {
		m_center->at(0) -= temp - m_map->width();
		m_camera_was_changed = true;
	}
	if (m_horizontal_blocks_number >= m_map->width()) {
		m_horizontal_blocks_number = float(m_map->width()) - 1.f;
		m_center->at(0) = m_horizontal_blocks_number / 2.f;
		m_camera_was_changed = true;
	}
}
void Camera::correct() {
	auto width_multiplier = (maxX() - minX()) / m_map->width();
	auto height_multiplier = (maxY() - minY()) / m_map->height();

	if (size_t(width_multiplier * 10.f) == size_t(height_multiplier * 10.f)) {
		correct_height();
		correct_width();
	} else if (width_multiplier > height_multiplier)
		correct_height();
	else
		correct_width();
}

Camera::Camera(std::shared_ptr<Map> map, std::shared_ptr<Actor> center_figure,
					   float aspectRatio, float blocks)
	: m_map(map), m_aspect_ratio(aspectRatio), m_horizontal_blocks_number(blocks), 
	m_center_figure(center_figure), m_camera_was_changed(true), m_map_was_changed(true) {

	m_center = new mgl::math::vector();
	if (m_center_figure)
		*m_center = mgl::math::vector(m_center_figure->position().at(0), m_center_figure->position().at(1));
	else
		*m_center = mgl::math::vector(m_horizontal_blocks_number / 2.f, m_horizontal_blocks_number / m_aspect_ratio / 2.f);
	correct();
}

void Camera::changeAspectRatio(float aspectRatio) {
	this->m_aspect_ratio = aspectRatio;
	m_camera_was_changed = true;
	correct();
}

void Camera::changeZoom(float magnifier) {
	if (magnifier > 0.f) {
		m_horizontal_blocks_number *= magnifier;
		m_camera_was_changed = true;
		correct();
	}
}

void Camera::move_to(float x, float y) {
	move_to(mgl::math::vector(x,y));
}

void Camera::move_to(mgl::math::vector const& point) {
	*m_center = point;
	m_camera_was_changed = true;
	correct();
}

void Camera::move(float x, float y) {
	move(mgl::math::vector(x, y));
}

void Camera::move(mgl::math::vector const& point) {
	*m_center += point;
	m_camera_was_changed = true;
	correct();
}

void Camera::changeCenterFigure(std::shared_ptr<Actor> center_figure) {
	m_center_figure = center_figure;
}
void Camera::changeMap(std::shared_ptr<Map> map) {
	m_map = map;
	m_map_was_changed = true;
}

float Camera::minX() const {
	return m_center->at(0) - m_horizontal_blocks_number / 2.f;
}
float Camera::minY() const {
	return m_center->at(1) - m_horizontal_blocks_number / m_aspect_ratio / 2.f;
}
float Camera::maxX() const {
	return m_center->at(0) + m_horizontal_blocks_number / 2.f;
}
float Camera::maxY() const {
	return m_center->at(1) + m_horizontal_blocks_number / m_aspect_ratio / 2.f;
}
long long Camera::minX_i() const {
	return size_t(minX()) - 2;
}
long long Camera::minY_i() const {
	return size_t(minY()) - 2;
}
long long Camera::maxX_i() const {
	return size_t(maxX()) + 1;
}
long long Camera::maxY_i() const {
	return size_t(maxY()) + 1;
}

std::shared_ptr<Map> Camera::map() {
	return m_map;
}
Camera::~Camera() {
	if (m_center)
		delete m_center;
}
void Camera::move() {
	if (m_center_figure)
		move_to(m_center_figure->position().at(0), m_center_figure->position().at(1));
	correct();
}