#include "FreeCamera.h"


void FreeCamera::move() {
	Application* appThis = Application::getInstancePtr();
	GLFWwindow* window = appThis->getWindow().getWindowPtr();

	float velocity = this->speedMovement * appThis->getDeltaTime();

	glm::vec3 pos = this->transform.getPosition();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pos += this->axis.getFront() * velocity;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pos += this->axis.getRight() * velocity;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		pos += this->axis.getUp() * velocity;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pos -= this->axis.getFront() * velocity;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pos -= this->axis.getRight() * velocity;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		pos -= this->axis.getUp() * velocity;

	this->transform.setPosition(pos);
}

void FreeCamera::spin() {
	this->spin(this->lastMousePosition.x, this->lastMousePosition.y);
}

void FreeCamera::spin(float xPos, float yPos) {
	glm::vec3 _rotation = this->transform.getRotation();

	_rotation.x += (this->lastMousePosition.y - yPos) * this->mouseSensitivity.x;
	_rotation.y += (xPos - this->lastMousePosition.x) * this->mouseSensitivity.y;

	_rotation.x = std::clamp<float>(_rotation.x, MIN_MOUSE_PITCH, MAX_MOUSE_PITCH);
	_rotation.y = Util::repeat(_rotation.y, MIN_MOUSE_YAW, MAX_MOUSE_YAW);

	this->transform.setRotation(_rotation);

	this->updateCameraVectors();
}

void FreeCamera::changeSpeed(float xOffset, float yOffset) {
	this->speedMovement += STEP_SPEED_MOVEMENT * yOffset;
	if (this->speedMovement < 0.0f) this->speedMovement = 0.0f;
}


void FreeCamera::keyboardInput() {
	this->move();
}

void FreeCamera::mouseInput(float xPos, float yPos) {
	if (this->lastMousePosition.x == xPos && this->lastMousePosition.y == yPos) return;

	this->spin(xPos, yPos);
	this->setLastMousePosition({ xPos, yPos });
}

void FreeCamera::mouseScrollInput(float xOffset, float yOffset) {
	this->changeSpeed(xOffset, yOffset);
}

void FreeCamera::mouseButtonInput(int button, int action, int mods) {}
