#include "Chamber.hpp"

// Vertex Coordinates
const float _vertices[][3] = {
	{0, 0, 0}, // 0 - left bottom back
	{1, 0, 0}, // 1 - right bottom back
	{1, 1, 0}, // 2 - right top back
	{1, 0, 1}, // 3 - right bottom front
	{1, 1, 1}, // 4 - right top front
	{0, 1, 0}, // 5 - left top back
	{0, 1, 1}, // 6 - left top front
	{0, 0, 1}, // 7 - left bottom front
};

// Indices
const int _indices[][4] = {
	{7, 6, 5, 0}, // X = 0
	{1, 2, 4, 3}, // X = 1
	{0, 1, 3, 7}, // Y = 0
	{5, 6, 4, 2}, // Y = 1
	{5, 2, 1, 0}, // Z = 0
	{3, 4, 6, 7}, // Z = 1
};

Chamber::Chamber()
{
	type = 3;
	noiseScale = 0.49;

	Create();
}

void Chamber::Create()
{
	PreCreate();

	std::vector<float> v(3);
	for (auto&& vertex : _vertices)
	{
		v[0] = vertex[0] - 0.5; v[1] = vertex[1] - 0.5; v[2] = vertex[2] - 0.5;
		vertices.push_back(v);
		// fprintf(stdout, "inserting %d: (%f, %f, %f)\n", vertices.size() - 1, v[0], v[1], v[2]);
	}

	for (auto&& indexArr : _indices)
	{
		for (auto&& index : indexArr)
		{
			indices.push_back(index);
		}
	}

	PostCreate();
}

void Chamber::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		// FIXME array-ification can happen in Create, doesn't need to be in Draw
		glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
			// Convert vector of vectors to flat array
			float vertexArray[vertices.size() * 3];
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				vertexArray[i*3 + 0] = vertices[i][0];
				vertexArray[i*3 + 1] = vertices[i][1];
				vertexArray[i*3 + 2] = vertices[i][2];
				// fprintf(stdout, "vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]);
			}
			float normalArray[normals.size() * 3];
			for (unsigned int i = 0; i < normals.size(); i++)
			{
				normalArray[i*3 + 0] = normals[i][0];
				normalArray[i*3 + 1] = normals[i][1];
				normalArray[i*3 + 2] = normals[i][2];
			}

			unsigned char indexArray[indices.size()];
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				indexArray[i] = indices[i];
				// fprintf(stdout, "%d\n", indices[i]);
			}

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_BYTE, indexArray);
		} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
	} glPopMatrix();
}