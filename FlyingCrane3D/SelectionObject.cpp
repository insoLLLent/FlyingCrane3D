#include "SelectionObject.h"


void SelectionObject::drawForSelection() {
	Application* appThis = Application::getInstancePtr();

	uint32_t colorId = 0;
	for (auto&[objKey, objValue] : appThis->getScene().getObjects()) {
		if (!objValue->getSelectionInfo().canSelect) continue;

		objValue->getSelectionInfo().colorSelect = Util::generateRGBAColorById(++colorId);
		objValue->getSelectionInfo().isSelectionProcess = true;

		for (auto&[meshKey, meshValue] : objValue->getMeshes()) {
			meshValue.setGlobalTransform(objValue->getParentTransform() + objValue->getTransform());
			meshValue.draw();
		}//rof

		objValue->getSelectionInfo().isSelectionProcess = false;
	}//rof
}

void SelectionObject::saveSelectedObject(glm::vec4 colorUnderCursor) {
	Application* appThis = Application::getInstancePtr();

	for (auto&[objKey, objValue] : appThis->getScene().getObjects()) {
		if (!objValue->getSelectionInfo().canSelect) continue;

		if (objValue->getSelectionInfo().colorSelectEquals(colorUnderCursor)) {
			objValue->getSelectionInfo().isSelected = true;

			if (this->hasSelectedObject(objKey)) {
				if (this->selectedObjects[objKey] == nullptr) {
					this->selectedObjects.erase(objKey);
					this->selectedObjects.insert({ objKey, objValue });
					break;
				}//fi
			} else {
				this->selectedObjects.insert({ objKey, objValue });
				break;
			}//fi
		}//fi
	}//rof
}

std::vector<glm::vec3> SelectionObject::getVerticesForCentroid() {
	std::vector<glm::vec3> centroidVertices {};

	for (auto&[objName, objValue] : this->selectedObjects) {
		for (auto&[meshName, meshValue] : objValue->getMeshes()) {
			for (Vertex& v : meshValue.getPolymesh().getVertices()) {
				glm::vec3 pos = v.position + objValue->getGlobalTransform().getPosition();
				centroidVertices.push_back(pos);
			}//rof
		}//rof
	}//rof

	return centroidVertices;
}

void SelectionObject::moving() {
	if (!this->hasSelectedObjects()) return;

	this->updateMousePosition();

	if (this->diffIsZero()) return;

	Application* appThis = Application::getInstancePtr();

	Axis& cameraAxis = appThis->getScene().getCamera().getAxis();
	glm::vec3 offset = cameraAxis.getRight() * diffMousePosition.x;
	offset += cameraAxis.getUp() * diffMousePosition.y;
	offset *= appThis->getDeltaTime();

	for (auto&[objKey, objValue] : this->selectedObjects) {
		glm::vec3 newPos = objValue->getTransform().getPosition() + offset;
		objValue->getTransform().setPosition(newPos);
	}//rof
}

void SelectionObject::rotation() {
	const static float STEP = 10.0f;

	if (!this->hasSelectedObjects()) return;

	this->updateMousePosition();

	if (this->diffIsZero()) return;

	Application* appThis = Application::getInstancePtr();

	Axis& cameraAxis = appThis->getScene().getCamera().getAxis();
	float deltaTime = appThis->getDeltaTime();

	for (auto&[objKey, objValue] : this->selectedObjects) {
		glm::vec3 newRot = objValue->getTransform().getRotation();
		newRot -= (cameraAxis.getRight() * diffMousePosition.y) * STEP * deltaTime;
		newRot += (cameraAxis.getUp() * diffMousePosition.x) * STEP * deltaTime;
		objValue->getTransform().setRotation(newRot);
	}//rof
}

void SelectionObject::scaling() {
	if (!this->hasSelectedObjects()) return;

	this->updateMousePosition();

	if (this->diffIsZero()) return;

	float deltaTime = Application::getInstancePtr()->getDeltaTime();

	for (auto&[objKey, objValue] : this->selectedObjects) {
		glm::vec3 newScale = objValue->getTransform().getScale();
		newScale += ((diffMousePosition.x + diffMousePosition.y) / 2.0f) * deltaTime;
		objValue->getTransform().setScale(newScale);
	}//rof
}

void SelectionObject::deleting() {
	if (!this->hasSelectedObjects()) return;

	for (auto&[objKey, objValue] : this->selectedObjects) {
		objValue->isDeleting = true;
	}//rof

	this->clearSelectedObjects();
}
