#include "SelectionElementFace.h"


void SelectionElementFace::drawForSelection() {
	Application* appThis = Application::getInstancePtr();

	uint32_t colorId = 0;
	for (auto&[objKey, objValue] : appThis->getScene().getObjects()) {
		if (!objValue->getSelectionInfo().canSelect) continue;

		for (auto&[meshName, meshElements] : objValue->getMeshElementManager().getFaces()) {
			for (auto& meshElement : meshElements) {
				if (!meshElement->getSelectionInfo().canSelect) continue;

				meshElement->getSelectionInfo().colorSelect = Util::generateRGBAColorById(++colorId);

				meshElement->getSelectionInfo().isSelectionProcess = true;
				meshElement->setType(GL_TRIANGLES);
				meshElement->draw();
				meshElement->setType(MeshElementFace::DEFAULT_MESH_TYPE);
				meshElement->getSelectionInfo().isSelectionProcess = false;
			}//rof
		}//rof

		this->drawObject(objValue);
	}//rof
}

void SelectionElementFace::saveSelectedObject(glm::vec4 colorUnderCursor) {
	Application* appThis = Application::getInstancePtr();

	for (auto&[objKey, objValue] : appThis->getScene().getObjects()) {
		if (!objValue->getSelectionInfo().canSelect) continue;

		for (auto&[meshName, meshElements] : objValue->getMeshElementManager().getFaces()) {
			for (auto& meshElement : meshElements) {
				if (!meshElement->getSelectionInfo().canSelect) continue;

				if (meshElement->getSelectionInfo().colorSelectEquals(colorUnderCursor)) {
					meshElement->getSelectionInfo().isSelected = true;
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

				meshElement->getSelectionInfo().colorSelect = CLEAR_COLOR;
			}//rof
		}//rof
	}//rof
}

std::vector<glm::vec3> SelectionElementFace::getVerticesForCentroid() {
	std::vector<glm::vec3> centroidVertices {};

	for (auto&[objName, objValue] : this->selectedObjects) {
		for (auto&[meshName, meshElements] : objValue->getMeshElementManager().getFaces()) {
			for (auto& me : meshElements) {
				if (!me->getSelectionInfo().isSelected) continue;

				for (Vertex& v : me->getVertices()) {
					glm::vec3 pos = v.position + objValue->getGlobalTransform().getPosition();
					centroidVertices.push_back(pos);
				}//rof
			}//rof
		}//rof
	}//rof

	return centroidVertices;
}

void SelectionElementFace::moving() {
	if (!this->hasSelectedObjects()) return;

	this->updateMousePosition();

	if (this->diffIsZero()) return;

	Application* appThis = Application::getInstancePtr();
	Axis& cameraAxis = appThis->getScene().getCamera().getAxis();

	glm::vec3 offset = cameraAxis.getRight() * diffMousePosition.x;
	offset += cameraAxis.getUp() * diffMousePosition.y;
	offset *= appThis->getDeltaTime();

	std::vector<glm::vec3> checked {};
	for (auto&[objName, objValue] : this->selectedObjects) {
		for (auto&[meshName, meshElements] : objValue->getMeshElementManager().getFaces()) {
			for (auto& me : meshElements) {
				if (!me->getSelectionInfo().isSelected) continue;

				for (Vertex& v : me->getVertices()) {
					if (std::find_if(checked.begin(), checked.end(),
									 [&v](const glm::vec3& pos) { return v.position == pos; })
						!= checked.end()) {

						continue;
					}

					glm::vec3 oldPos = v.position;
					glm::vec3 newPos = oldPos + offset;

					this->updateMovingData(objValue, oldPos, newPos);

					checked.push_back(newPos);
				}//rof
			}//rof
		}//rof
	}//rof
}

void SelectionElementFace::rotation() { /* dummy */ }

void SelectionElementFace::scaling() { /* dummy */ }

void SelectionElementFace::deleting() { /* dummy */ }
