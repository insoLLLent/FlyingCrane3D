#include "SelectionElementEdge.h"


void SelectionElementEdge::select() {
	Application* appThis = Application::getInstancePtr();

	Selection::clearColor();

	glDisable(GL_MULTISAMPLE);

	// ��������� ����� �������� � ������������
	uint32_t colorId = 0;
	for (auto&[objKey, objValue] : appThis->getScene().getObjects()) {
		if (!objValue.getSelectionInfo().canSelect) continue;

		this->drawObject(objValue);

		for (auto&[meshElemKey, meshElements] : objValue.getMeshElementManager().getEdges()) {
			for (auto& meshElement : meshElements) {
				if (!meshElement.getSelectionInfo().canSelect) continue;

				meshElement.getSelectionInfo().colorSelect = Util::generateRGBAColorById(++colorId);

				meshElement.getSelectionInfo().isSelectionProcess = true;
				glLineWidth(10.0f);
				meshElement.draw();
				glLineWidth(1.0f);
				meshElement.getSelectionInfo().isSelectionProcess = false;
			}//rof
		}//rof
	}//rof

	glEnable(GL_MULTISAMPLE);

	glm::vec4 colorUnderCursor = Selection::getColorUnderCursor();

	// ��������� ������� ��������� �������
	for (auto&[objKey, objValue] : appThis->getScene().getObjects()) {
		if (!objValue.getSelectionInfo().canSelect) continue;

		for (auto&[meshElemKey, meshElements] : objValue.getMeshElementManager().getEdges()) {
			for (auto& meshElement : meshElements) {
				if (!meshElement.getSelectionInfo().canSelect) continue;

				if (meshElement.getSelectionInfo().colorSelectEquals(colorUnderCursor)) {
					meshElement.getSelectionInfo().isSelected = true;

					if (this->hasSelectedObject(objKey)) {
						if (this->selectedObjects[objKey] == nullptr) {
							this->selectedObjects.erase(objKey);
							this->selectedObjects.insert({ objKey, &objValue });
							break;
						}//fi
					} else {
						this->selectedObjects.insert({ objKey, &objValue });
						break;
					}//fi
				}//fi
			}//rof
		}//rof
	}//rof
}

std::vector<glm::vec3> SelectionElementEdge::getVerticesForCentroid() {
	std::vector<glm::vec3> centroidVertices {};

	for (auto&[objName, objValue] : this->selectedObjects) {
		for (auto&[meshName, meshElements] : objValue->getMeshElementManager().getEdges()) {
			for (auto& me : meshElements) {
				if (!me.getSelectionInfo().isSelected) continue;

				for (Vertex& v : me.getVertices()) {
					glm::vec3 pos = v.position + objValue->getGlobalTransform().getPosition();
					centroidVertices.push_back(pos);
				}//rof
			}//rof
		}//rof
	}//rof

	return centroidVertices;
}

void SelectionElementEdge::moving() { /* dummy */ }

void SelectionElementEdge::rotation() { /* dummy */ }

void SelectionElementEdge::scaling() { /* dummy */ }
