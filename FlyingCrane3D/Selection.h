#pragma once

#include "Util.h"
#include "Application.h"
#include "SelectionInfo.h"
#include "Object.h"
#include "MeshElementManager.h"

class Object;

class ISelection {
protected:
	virtual void drawForSelection() = 0;
	virtual void saveSelectedObject(glm::vec4 colorUnderCursor) = 0;

	virtual std::vector<glm::vec3> getVerticesForCentroid() = 0;

public:
	virtual void moving() = 0;
	virtual void rotation() = 0;
	virtual void scaling() = 0;

	virtual void deleting() = 0;
};

class Selection : public ISelection {
protected:
	using MapObjectPtr = std::map<std::string, std::shared_ptr<Object>>;

public:
	static const glm::vec4 CLEAR_COLOR;

protected:
	glm::vec2 prevMousePosition {};
	glm::vec2 diffMousePosition {};

	MapObjectPtr selectedObjects {};

protected:
	static glm::vec4 getColorUnderCursor();
	bool diffIsZero();
	void keyboardInputDeleting();

public:
	Selection() = default;
	virtual ~Selection() = default;

	virtual void select();

public:
	MapObjectPtr& getSelectedObjects();
	bool hasSelectedObject(std::string name);
	bool hasSelectedObjects();
	void clearSelectedObjects();

	void updateMousePosition();
	void updateMousePosition(float currentMouseX, float currentMouseY);

	glm::vec3 getCentroid();

public:
	void mouseButtonInput(int button, int action, int mods);
	void keyboardInput();
};
