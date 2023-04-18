#include "objloader.h"

bool loadOBJ(
	const char* path,
	std::vector < float >& out_vertices,
	std::vector < float >& out_uvs,
	std::vector < float >& out_normals,
	int& licznik
) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	std::ifstream file(path);
	std::string lineHeader;
	licznik = 0;
	while (file >> lineHeader) {


		// read the first word of the line
		//int res = fscanf(file, "%s", lineHeader);
		if (lineHeader == "v") {
			glm::vec3 vertex;
			file >> vertex.x;
			file >> vertex.y;
			file >> vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if (lineHeader == "vt") {
			glm::vec2 uv;
			file >> uv.x;
			file >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (lineHeader == "vn") {
			glm::vec3 normal;
			file >> normal.x;
			file >> normal.y;
			file >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (lineHeader == "f") {
			std::string vertex1;
			for (int i = 0; i < 3; i++) {
				file >> vertex1;
				
				size_t pozycja = 0;
				int tmp;

				pozycja = vertex1.find("/");
				tmp = stoi(vertex1.substr(0, pozycja)) - 1;
				vertex1.erase(0, pozycja + 1);
				vertexIndices.push_back(tmp);

				pozycja = vertex1.find("/");
				tmp = stoi(vertex1.substr(0, pozycja)) - 1;
				vertex1.erase(0, pozycja + 1);
				uvIndices.push_back(tmp);

				tmp = stoi(vertex1)-1;
				normalIndices.push_back(tmp);
				licznik += 1;

			}





		}
		
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex];
		out_vertices.push_back(vertex.x);
		out_vertices.push_back(vertex.y);
		out_vertices.push_back(vertex.z);
		out_vertices.push_back(1.0);

	}
	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex];
		out_uvs.push_back(uv.x);
		out_uvs.push_back(uv.y);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex];
		out_normals.push_back(normal.x);
		out_normals.push_back(normal.y);
		out_normals.push_back(normal.z);
		out_normals.push_back(0.0);
	}
	file.close();
	return true;
}
