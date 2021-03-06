#pragma once

#include "ITriada.h"
#include "Transform.h"
#include "SelectionInfo.h"
#include "Mesh.h"
#include "Shader.h"
#include "MeshElementManager.h"


class Object : public ITriada {
protected:
	template<class T>
	using Map = std::map<std::string, T>;
	using MapMesh = Map<Mesh>;
	using MapObject = Map<Object>;

protected:
	Transform transform {};
	Transform parentTransform {};
	Transform globalTransform {};

	MeshElementManager mem { this };
	MapMesh meshes {};

	SelectionInfo selectionInfo {};
	TriadaMode mode = TriadaMode::NONE;

public:
	Object() = default;
	virtual ~Object();

	bool isDeleting = false;

	Transform& getTransform();
	Transform& getParentTransform();
	Transform& getGlobalTransform();
	SelectionInfo& getSelectionInfo();
	TriadaMode getMode();

	void setTransform(Transform _transform);
	void setParentTransform(Transform _pTransform);
	void setGlobalTransform(Transform _gTransform);
	void setSelectionInfo(SelectionInfo _selectionInfo);

	MeshElementManager& getMeshElementManager();

	MapMesh& getMeshes();
	void addMesh(const std::string& _name, Mesh& _mesh);

	void drawMeshes();

	virtual void init() override;
	virtual void draw() override;
	virtual void free() override;
};
