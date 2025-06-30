#include "pch.h"
#include "merlin/scene/camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Merlin {

	Camera::Camera() :
		_width(1080),
		_height(720),
		_AspectRatio(float(_width) / float(_height)),
		_projection(Projection::Perspective),
		_fov(45.0f),
		_nearPlane(0.1f),
		_farPlane(2000.f),
		_zoom(1.0f)
	{
		reset();
		resetProjection();
		_ViewMatrix = glm::mat4(1.0f);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
		recalculateViewMatrix();
	}

	Camera::Camera(float width, float height, Projection projection) :
		_width(width), 
		_height(height), 
		_AspectRatio(float(width)/float(height)), 
		_projection(projection),
		_fov(45.0f),
		_nearPlane(0.1f),
		_farPlane(2000.f), 
		_zoom(1.0f)
	{
		reset();
		resetProjection();
		_ViewMatrix = glm::mat4(1.0f);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
		recalculateViewMatrix();
	}

	void Camera::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowResizeEvent>(MERLIN_BIND_EVENT_FN(Camera::onWindowResized));
	}

	void Camera::reset(){
		_Position = { 0.0f, 0.0f, 0.0f };

		_Target = { 0.0f, 0.0f, 0.0f };
		_WorldUp = { 0.0f, 0.0f, 1.0f };

		_Right = { 1.0f, 0.0f, 0.0f };
		_Front = { 0.0f, 1.0f, 0.0f };
		_Up = { 0.0f, 0.0f, 1.0f };
	}

	void Camera::resetProjection(){
		_AspectRatio = (float)_width / (float)_height;
		if (_projection == Projection::Perspective)
			_ProjectionMatrix = glm::perspective(glm::radians(_fov), _AspectRatio, _nearPlane, _farPlane);
		else
			_ProjectionMatrix = glm::ortho(-_AspectRatio *_zoom, _AspectRatio * _zoom, -_zoom, _zoom, _nearPlane, _farPlane);
	}

	void Camera::restoreViewport() const {
		glViewport(0, 0, _width, _height);
	}

	void Camera::translate(float dx, float dy) {
		translate(glm::vec2(dx, dy));
	}

	void Camera::translate(glm::vec2 dU2D) {
		translate(glm::vec3(dU2D.y, dU2D.x, 0));
	}

	void Camera::translate(float dx, float dy, float dz) {
		translate(glm::vec3(dx, dy, dz));
	}

	void Camera::translate(glm::vec3 du) {
		// Move relative to camera orientation
		_Position += _Front * du.x;
		_Position -= _Right * du.y;
		_Position += _Up * du.z;

		recalculateViewMatrix();
	}

	void Camera::setView(CameraView view, float distance, glm::vec3 center ) {
		glm::vec3 offset;

		switch (view) {
		case CameraView::Iso:
			offset = glm::normalize(glm::vec3(1, -1, 1)) * distance;
			break;
		case CameraView::Top:
			offset = glm::vec3(0, 0, distance);
			break;
		case CameraView::Bottom:
			offset = glm::vec3(0, 0, -distance);
			break;
		case CameraView::Right:
			offset = glm::vec3(distance, 0, 0);
			break;
		case CameraView::Left:
			offset = glm::vec3(-distance, 0, 0);
			break;
		case CameraView::Front:
			offset = glm::vec3(0, -distance, 0);
			break;
		case CameraView::Rear:
			offset = glm::vec3(0, distance, 0);
			break;
		}

		_Position = center + offset;

		glm::vec3 dir = glm::normalize(center - _Position);

		_Yaw = -glm::degrees(atan2(dir.x, dir.y));      // horizontal angle: X over Y
		_Pitch = glm::degrees(asin(dir.z));              // vertical tilt from Z
		_Roll = 0.0f;

		recalculateViewMatrix();
	}






	void Camera::rotate(float deltaYaw, float deltaPitch, float deltaRoll) {
		_Yaw += deltaYaw;
		_Pitch += deltaPitch;
		_Roll += deltaRoll;
		_Pitch = glm::clamp(_Pitch, -89.9f, 89.9f);
		//_Pitch = glm::clamp(_Pitch, -89.0f, 89.0f); // Prevent gimbal lock
		recalculateViewMatrix();
	}




	bool Camera::onWindowResized(WindowResizeEvent& e) {
		if (e.getHeight() == 0 || e.getWidth() == 0) return false;
		_height = e.getHeight();
		_width = e.getWidth();
		
		resetProjection();

		return false;
	}


	void Camera::recalculateViewMatrix() {
		// Step 1: apply yaw around world Z (Z-up)
		glm::quat qYaw = glm::angleAxis(glm::radians(_Yaw), glm::vec3(0, 0, 1));

		// Step 2: pitch around camera-local right (after yaw)
		glm::vec3 right = qYaw * glm::vec3(1, 0, 0);
		glm::quat qPitch = glm::angleAxis(glm::radians(_Pitch), right);

		// Combine yaw and pitch
		glm::quat orientation = qPitch * qYaw;

		// Base direction is +Y in your system
		_Front = glm::normalize(orientation * glm::vec3(0, 1, 0));
		_Right = glm::normalize(glm::cross(_Front, _WorldUp));
		_Up = glm::normalize(glm::cross(_Right, _Front));

		_ViewMatrix = glm::lookAt(_Position, _Position + _Front, _Up);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
	}








}