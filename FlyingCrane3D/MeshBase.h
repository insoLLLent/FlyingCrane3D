#pragma once

#include "ITriada.h"
#include "Shader.h"
#include "Transform.h"

class IMeshBase : public ITriada {
	virtual void updateBufferedVertices() = 0;
};

class MeshBase : public IMeshBase {
protected:
	static const GLsizei BUFFER_SIZE;

	enum AttribIndex : GLuint {
		POSITION, NORMAL
	};

protected:
	GLuint vao, vbo, ebo;

	Transform transform {};
	Transform parentTransform {};
	Transform globalTransform {};

	Shader shader {};

	GLenum type;

	MeshBase();
	MeshBase(GLenum _type, Shader _shader);

public:
	virtual ~MeshBase() = default;

	GLuint& getVAO();
	GLuint& getVBO();
	GLuint& getEBO();

	Transform& getTransform();
	Transform& getParentTransform();
	Transform& getGlobalTransform();
	Shader& getShader();
	GLenum getType();

	void setTransform(Transform _transform);
	void setParentTransform(Transform _pTransform);
	void setGlobalTransform(Transform _gTransform);
	void setShader(Shader& _shader);
	void setType(GLenum _type);

	virtual void free() override;
};
