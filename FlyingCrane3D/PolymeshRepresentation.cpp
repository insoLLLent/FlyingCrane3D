#include "PolymeshRepresentation.h"


PolymeshRepresentation::PolymeshRepresentation(
	std::vector<Vertex> _vertices, std::vector<GLuint> _indices) {

	this->init(_vertices, _indices);
}

std::vector<GLuint>& PolymeshRepresentation::getIndicesMesh() {
	return this->indicesMesh;
}

std::vector<Vertex>& PolymeshRepresentation::getVertices() {
	return this->vertices;
}

std::vector<Point>& PolymeshRepresentation::getPoints() {
	return this->points;
}

std::vector<Edge>& PolymeshRepresentation::getEdges() {
	return this->edges;
}

std::vector<Face>& PolymeshRepresentation::getFaces() {
	return this->faces;
}


void PolymeshRepresentation::init(
	std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices) {

	if (_vertices.size() != _indices.size()) {
		GLuint counter = 0;

		for (GLuint& i : _indices) {
			Vertex newVertex = _vertices[i];
			newVertex.index = counter;
			this->vertices.push_back(newVertex);
			this->indicesMesh.push_back(counter++);
		}//rof
	} else {
		this->vertices = _vertices;
		this->indicesMesh = _indices;
	}//fi

	this->update();
}

void PolymeshRepresentation::updateNormal(Vertex& v1, Vertex& v2, Vertex& v3) {
	glm::vec3 newNormal = glm::triangleNormal(v1.position, v2.position, v3.position);

	v1.normal = newNormal;
	v2.normal = newNormal;
	v3.normal = newNormal;
}

void PolymeshRepresentation::updatePoints(Vertex& v1, Vertex& v2, Vertex& v3) {
	for (Point point : { Point { v1 }, Point { v2 }, Point { v3 } }) {
		auto pred_p = [&point](const Point& p) {
			return point == p;
		};

		if (std::find_if(this->points.begin(), this->points.end(), pred_p) == this->points.end()) {
			this->points.push_back(point);
		}//fi
	}//rof
}

void PolymeshRepresentation::updateEdges(Vertex& v1, Vertex& v2, Vertex& v3) {
	for (Edge edge : { Edge { v1, v2 }, Edge { v2, v3 }, Edge { v3, v1 } }) {
		auto pred_e = [&edge](const Edge& e) {
			return edge == e;
		};

		if (std::find_if(this->edges.begin(), this->edges.end(), pred_e) == this->edges.end()) {
			this->edges.push_back(edge);
		}//fi
	}//rof
}

void PolymeshRepresentation::updateFaces(Vertex& v1, Vertex& v2, Vertex& v3) {
	Face face { v1, v2, v3 };

	auto pred_f = [&face](const Face& f) {
		return face == f;
	};

	if (std::find_if(this->faces.begin(), this->faces.end(), pred_f) == this->faces.end()) {
		this->faces.push_back(face);
	}//fi
}

void PolymeshRepresentation::update() {
	if (this->indicesMesh.empty()) return;

	this->faces.clear();
	this->edges.clear();
	this->points.clear();

	for (std::size_t i = 0; i < this->indicesMesh.size(); i += 3) {
		Vertex& v1 = this->vertices[this->indicesMesh[i]];
		Vertex& v2 = (i + 1) < this->indicesMesh.size() ?
			this->vertices[this->indicesMesh[i + 1]] : this->vertices[this->indicesMesh[i]];
		Vertex& v3 = (i + 2) < this->indicesMesh.size() ?
			this->vertices[this->indicesMesh[i + 2]] : this->vertices[this->indicesMesh[i + 1]];

		this->updateNormal(v1, v2, v3);

		this->updateFaces(v1, v2, v3);
		this->updateEdges(v1, v2, v3);
		this->updatePoints(v1, v2, v3);
	}//rof
}
