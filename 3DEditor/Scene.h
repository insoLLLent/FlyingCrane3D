#pragma once

#include "ITriada.h"
#include "Vertex.h"
#include "Object.h"
#include "GridObject.h"
#include "Camera.h"
#include "Model.h"
#include "Transform.h"

class Scene final : public ITriada {
private:
	Camera camera {};
	Model model {};
	Transform transform {};

public:
	std::map<std::string, Object> standardObjects {};
	std::map<std::string, Object> objects {};

public:
	Scene() = default;
	~Scene() = default;

	Camera& getCamera();
	Model& getModel();
	Transform& getTransform();

	void addStandardObject(std::string _name, Object _object);
	void addObject(std::string _name, Object _object);

	virtual void init() override;
	virtual void draw() override;
	virtual void free() override;

};
