#pragma once

#include "Util.h"
#include "ITriada.h"
#include "Mesh.h"
#include "Application.h"


class Mesh;

class Shader : public ITriada {
private:
	using FsPath = std::filesystem::path;
	using LambdaDraw = std::function<void(Shader* _this)>;

private:
	GLuint id;

	FsPath vertexPath {};
	FsPath fragmentPath {};

	std::string vertexSource;
	std::string fragmentSource;

	GLuint vertexCompile;
	GLuint fragmentCompile;

	// uniform ������ ��� ������������� ��������
	LambdaDraw lambdaDraw;
	bool useMVP = true;

	Transform parentTransform {};

private:
	static constexpr unsigned int INFOLOG_SIZE = 512;

	GLint successChecker;
	GLchar infoLog[Shader::INFOLOG_SIZE];

	void vertexInit();
	void fragmentInit();
	void programInit();

public:
	static const std::string SHADER_DIRECTORY;
	static const FsPath DEFAULT_VERTEX_PATH;
	static const FsPath DEFAULT_FRAGMENT_PATH;

public:
	Shader();
	Shader(FsPath _vertexPath, FsPath _fragmentPath);
	~Shader() = default;

	void setLambdaDraw(LambdaDraw _lambdaDraw);
	void resetLambdaDraw();

	Transform& getParentTransform();
	bool getUseMVP();

	void setParentTransform(Transform _pTransform);
	void setUseMVP(bool _useMVP);

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec4(const std::string& name, glm::vec4 value) const;

	virtual void init() override;
	virtual void draw() override;
	virtual void free() override;

};


/* Exceptions */

class ShaderException : std::exception {
public:
	explicit ShaderException(char const* const message);
};
