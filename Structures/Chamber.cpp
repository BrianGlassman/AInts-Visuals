#include <functional>

#include "Chamber.hpp"
#include "globals.hpp"
#include "Shaders.hpp"

Chamber::Chamber(unsigned char sides)
{
	type = 3;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	UnpackSides(sides);

	Create();
}

Vector3 Chamber::SetCoords(const float v0, const float v1, const float v2, int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	Vector3 coords;
	coords[i0] = (f0 ? -1 : 1) * v0;
	coords[i1] = (f1 ? -1 : 1) * v1;
	coords[i2] = (f2 ? -1 : 1) * v2;
	return coords;
}
void Chamber::FacePointHelper(std::function<Vector3(const float, const float, const float)> BoundSetCoords,
	const float v0, const float v1, const float v2)
{
	auto coords = BoundSetCoords(v0, v1, v2);
	indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
	vertices.push_back(coords);
	normals.push_back(coords);
}
/// @brief Create 1/4 of a face 
void Chamber::FaceHelper(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm)
{
	Vector3 coords;

	// FIXME generalize for n != 8

	// Ref https://en.cppreference.com/w/cpp/utility/functional/bind
	auto boundSetCoords = std::bind(SetCoords, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, i0, f0, i1, f1, i2, f2);

	// Pre-compute for readability
	float r = 0.5 * padScale;
	float xAtTunnel = sqrt(r*r - tunnelRadius*tunnelRadius);
	float tunnelCorner = sqrt(2)/2 * tunnelRadius;
	float planeCorner = sqrt(2)/2 * padScale * 0.5;
	float tripleCorner = sqrt(3)/3 * padScale * 0.5;

	// Face outside the arm - Start at 90 degrees viewed from +X, move CCW
	if (true) { // 1
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelRadius, 0); // Bottom right
		FacePointHelper(boundSetCoords, planeCorner, planeCorner, 0); // Top point
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom left
	}
	if (true) { // 2
		FacePointHelper(boundSetCoords, tripleCorner, tripleCorner, tripleCorner); // Top left
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom point - coincident with 1's Bottom left
		FacePointHelper(boundSetCoords, planeCorner, planeCorner, 0); // Top right - coincident with 1's Top point
	}
	if (true) { // 3 - like 2 with Y <--> Z
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom point - coincident with 2's Bottom point
		FacePointHelper(boundSetCoords, tripleCorner, tripleCorner, tripleCorner); // Top right - coincident with 2's Top left
		FacePointHelper(boundSetCoords, planeCorner, 0, planeCorner); // Top left - mirror to 2's Top right
	}
	if (true) { // 4 - like 1 with Y <--> Z
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom right - coincident with 3's Bottom point
		FacePointHelper(boundSetCoords, planeCorner, 0, planeCorner); // Top point - coincident with 3's Top left
		FacePointHelper(boundSetCoords, xAtTunnel, 0, tunnelRadius); // Bottom left - mirror to 1's Bottom right
	}

	// Face inside the arm, if applicable
	if (!hasArm)
	{ // Start at 90 degrees viewed from +X, move CCW
		if (true) { // A
			FacePointHelper(boundSetCoords, r, 0, 0); // Bottom point
			FacePointHelper(boundSetCoords, xAtTunnel, tunnelRadius, 0); // Top right - coincident with 1's Bottom right
			FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Top left - coincident with 1's Bottom left
		}
		if (true) { // B
			FacePointHelper(boundSetCoords, xAtTunnel, 0, tunnelRadius); // Top left - coincident with 4's Bottom left
			FacePointHelper(boundSetCoords, r, 0, 0); // Bottom point - coincident with A's Bottom point
			FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Top right - coincident with A's Top left
		}
	}
}
void Chamber::CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm)
{
	FaceHelper(i0, f0, i1, f1, i2, f2, hasArm);
	FaceHelper(i0, f0, i2, f2, i1, !f1, hasArm);
	FaceHelper(i0, f0, i2, !f2, i1, f1, hasArm);
	FaceHelper(i0, f0, i1, !f1, i2, !f2, hasArm);

	if (hasArm)
	{
		CreateArm(i0, f0, i1, f1, i2, f2);
	}
}
void Chamber::CreateArm(int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	float xAtTunnel = sqrt(radius*radius - tunnelRadius*tunnelRadius);

	float d = (0.5 - xAtTunnel) / armPanels;
	float thetaD = 360 / tunnelN;

	Vector3 coords;
	float x = xAtTunnel, y, z;
	for (int i = 0; i < armPanels; i++)
	{
		for (float theta = 0; theta < 360; theta += thetaD)
		{
			// Top-left quadrant, moving CCW
			{ // This row
				Polar2Cart(tunnelRadius, theta, &z, &y);
				z = -z;
				coords[i0] = 0;
				coords[i1] = f1 ? -y : y;
				coords[i2] = f2 ? -z : z;

				// Outer edge
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				armIndices.insert(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);

				// Inner edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);
			}
			{ // Next row
				Polar2Cart(tunnelRadius, theta + thetaD, &z, &y);
				z = -z;
				coords[i0] = 0;
				coords[i1] = f1 ? -y : y;
				coords[i2] = f2 ? -z : z;
				// Inner edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);

				// Outer edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				armIndices.insert(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);
			}
		}

		x += d;
	}
}

void Chamber::CreateCLHelper(std::vector<Vertex> &CLtoUse, int axis, bool flip)
{
	// FIXME should match arm panelling exactly, then handle core differently

	int panels = round(0.5 / tunnelRadius); // <-- different from Tunnel
	float d = (0.5) / panels; // <-- different from Tunnel

	int lastIdx = 0;
	int currentIdx;
	float x = tunnelRadius; // <-- different from Tunnel
	for (int i = 0; i < panels; i++)
	{
		// Create
		currentIdx = CLtoUse.size();
		Vertex vert(currentIdx);
		vert.coords[axis] = (flip ? -1 : 1) * x;

		// Link
		CLtoUse[lastIdx].AddNeighbor(currentIdx);
		vert.AddNeighbor(lastIdx);

		// Insert (MUST be after creation and linking)
		CLtoUse.push_back(vert);

		x += d;
		lastIdx = currentIdx;
	}
}
void Chamber::CreateCenterline(int axis, bool flip)
{
	CreateCLHelper(centerline, axis, flip);
	CreateCLHelper(baseCenterline, axis, flip);
}

void Chamber::PreCreate()
{
	Structure::PreCreate();
	triIndices.clear();
	quadIndices.clear();
}
void Chamber::Create()
{
	PreCreate();

	// fprintf(stdout, "panelWidth = %f\n", panelWidth);

	// Note: flip i0 to change winding direction

	// Faces
	if (true)
	{
		CreateFace(0, false, 1, false, 2, false, right);
		CreateFace(1, false, 2, false, 0, false, top);
		CreateFace(2, false, 0, false, 1, false, forward);
		CreateFace(0,  true, 1, false, 2,  true, left);
		CreateFace(1,  true, 2, false, 0,  true, bottom);
		CreateFace(2,  true, 0, false, 1,  true, back);
	}

	// Create centerlines
	centerline.push_back(Vertex(0));
	baseCenterline.push_back(Vertex(0));
	if (right)
	{
		CreateCenterline(0, false);
		endpointRight = centerline.size() - 1;
	}
	if (left)
	{
		CreateCenterline(0,  true);
		endpointLeft = centerline.size() - 1;
	}
	if (top)
	{
		CreateCenterline(1, false);
		endpointTop = centerline.size() - 1;
	}
	if (bottom)
	{
		CreateCenterline(1,  true);
		endpointBottom = centerline.size() - 1;
	}
	if (forward)
	{
		CreateCenterline(2, false);
		endpointForward = centerline.size() - 1;
	}
	if (back)
	{
		CreateCenterline(2,  true);
		endpointBack = centerline.size() - 1;
	}

	ErrCheck("Chamber::Create");

	PostCreate();
}

void Chamber::ApplyNoise()
{
	// Use different noise scaling for arm portion versus main chamber
	float scale;
	for (unsigned int i = 0; i < baseVertices.size(); i++)
	{
		if (armIndices.find(i) == armIndices.end())
		{
			scale = noiseScale;
		}
		else
		{
			scale = Globals::tunnelNoiseScale;
		}

		// Perturbation at the initial vertex
		Vector3 p = noisePtr->getNoise(baseVertices[i] + center);
		// printf("p*scale = %f, %f, %f\n", p.x*scale, p.y*scale, p.z*scale);

		// --- Vertices ---
		vertices[i] = baseVertices[i] + p*scale;

		// --- Normals ---
		if (true)
		{
			// printf("base normal = %f, %f, %f\n", normals[i].x, normals[i].y, normals[i].z);

			// Construct tangent vectors by crossing the normal with each axis
			Vector3 tx = normals[i].Cross(Vector3::Right);
			Vector3 ty = normals[i].Cross(Vector3::Up);
			Vector3 tz = normals[i].Cross(Vector3::Forward);

			// Reduce magnitude to a small value
			tx = tx.Normalized() * 0.001;
			ty = ty.Normalized() * 0.001;
			tz = tz.Normalized() * 0.001;

			// printf(" original tx = %f, %f, %f\n", tx.x, tx.y, tx.z);

			// Perturbation will be slightly different because of the offset
			Vector3 px = noisePtr->getNoise(baseVertices[i] + center + tx);
			Vector3 py = noisePtr->getNoise(baseVertices[i] + center + ty);
			Vector3 pz = noisePtr->getNoise(baseVertices[i] + center + tz);

			// printf("px*scale = %f, %f, %f\n", px.x*scale, px.y*scale, px.z*scale);

			// printf(" p*scale = %f, %f, %f\n", p.x*scale, p.y*scale, p.z*scale);

			// printf("perturbed tx = original tx + px*scale - p*scale\n");

			// Compute the perturbed tangent vectors
			tx = tx + px*scale - p*scale;
			ty = ty + py*scale - p*scale;
			tz = tz + pz*scale - p*scale;

			// printf("perturbed tx = %f, %f, %f\n", tx.x, tx.y, tx.z);

			// Normalize for the sake of later computation
			tx.Normalize();
			ty.Normalize();
			tz.Normalize();

			// printf("perturbed and normalized tx = %f, %f, %f\n", tx.x, tx.y, tx.z);

			// Eliminate zero-magnitude tangents
			bool xValid = tx.Magnitude() > 0;
			bool yValid = ty.Magnitude() > 0;
			bool zValid = tz.Magnitude() > 0;

			Vector3 tUse1, tUse2;
			if (!zValid)
			{
				if (!(xValid && yValid)) Fatal(999, "Not enough valid tangents\n");
				// printf("validity xy\n");
				tUse1 = tx;
				tUse2 = ty;
			}
			else if (!yValid)
			{
				if (!(xValid && zValid)) Fatal(999, "Not enough valid tangents\n");
				// printf("validity xz\n");
				tUse1 = tx;
				tUse2 = tz;
			}
			else if (!xValid)
			{
				if (!(yValid && zValid)) Fatal(999, "Not enough valid tangents\n");
				// printf("validity yz\n");
				tUse1 = ty;
				tUse2 = tz;
			}
			else
			{
				// Use whichever two perturbed tangents are most orthogonal to each other
				float xDy = abs(tx.Dot(ty));
				float xDz = abs(tx.Dot(tz));
				float yDz = abs(ty.Dot(tz));
				if (xDy <= xDz && xDy <= yDz)
				{
					// printf("ortho xy\n");
					tUse1 = tx;
					tUse2 = ty;
				}
				else if (xDz <= xDy && xDz <= yDz)
				{
					// printf("ortho xz\n");
					tUse1 = tx;
					tUse2 = tz;
				}
				else if (yDz <= xDy && yDz <= xDz)
				{
					// printf("ortho yz\n");
					tUse1 = ty;
					tUse2 = tz;
				}
				else Fatal(999, "ApplyNoise failed somehow\n");
			}

			// Compute the new normal by crossing the perturbed tangents
			Vector3 tempN = tUse1.Cross(tUse2);

			// Normalize
			tempN.Normalize();
			if (tempN.Magnitude() == 0) printf("tx = (%f, %f, %f), ty = (%f, %f, %f), tz = (%f, %f, %f)\n",
				tx.x, tx.y, tx.z,
				ty.x, ty.y, ty.z,
				tz.x, tz.y, tz.z
				);

			// Assume normals never completely change direction, and any inversion is because of cross product ordering
			if (tempN.Dot(baseNormals[i]) < 0)
			{
				// printf("Base (%f, %f, %f), Perturbed (%f, %f, %f), Dot %f\n", baseNormals[i].x, baseNormals[i].y, baseNormals[i].z, tempN.x, tempN.y, tempN.z, tempN.Dot(baseNormals[i]));
				tempN = tempN.Reversed();
			}

			// Save the result
			normals[i] = tempN;
		}


		if (i == 0)
		{
			// fprintf(stdout, "base (%f, %f, %f), noise (%f, %f, %f)\n",
			// 	baseVertices[i][0], baseVertices[i][1], baseVertices[i][2],
			// 	p[0], p[1], p[2]);
		}
    }

	// Apply to the centerline
	for (unsigned int i = 0; i < baseCenterline.size(); i++)
	{
		float x = baseCenterline[i].x() + center.x;
		float y = baseCenterline[i].y() + center.y;
		float z = baseCenterline[i].z() + center.z;

		auto p = noisePtr->getNoise(x, y, z);

		centerline[i].coords.x = baseCenterline[i].x() + p[0]*noiseScale;
		centerline[i].coords.y = baseCenterline[i].y() + p[1]*noiseScale;
		centerline[i].coords.z = baseCenterline[i].z() + p[2]*noiseScale;
	}
}

void Chamber::DrawHelper(std::vector<Vector3> drawVertices)
{
	// FIXME array-ification can happen in Create, doesn't need to be in Draw
	glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
		// Convert vector of vectors to flat array
		float vertexArray[drawVertices.size() * 3];
		for (unsigned int i = 0; i < drawVertices.size(); i++)
		{
			vertexArray[i*3 + 0] = drawVertices[i][0];
			vertexArray[i*3 + 1] = drawVertices[i][1];
			vertexArray[i*3 + 2] = drawVertices[i][2];
			// fprintf(stdout, "vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]);
		}
		float normalArray[normals.size() * 3];
		for (unsigned int i = 0; i < normals.size(); i++)
		{
			normalArray[i*3 + 0] = normals[i][0];
			normalArray[i*3 + 1] = normals[i][1];
			normalArray[i*3 + 2] = normals[i][2];
		}

		if (triIndices.size() > 0)
		{ // Draw with GL_TRIANGLES
			unsigned int triIndexArray[triIndices.size()];
			for (unsigned int i = 0; i < triIndices.size(); i++)
			{
				triIndexArray[i] = triIndices[i];
				// fprintf(stdout, "%d\n", triIndexArray[i]);
			}
			// fprintf(stdout, "%d: %d\n", triIndices.size()-1, triIndexArray[triIndices.size()-1]); // Check for overflow

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			glDrawElements(GL_TRIANGLES, triIndices.size(), GL_UNSIGNED_INT, triIndexArray);
		}

		if (quadIndices.size() > 0)
		{ // Draw with GL_QUADS
			unsigned int quadIndexArray[quadIndices.size()];
			for (unsigned int i = 0; i < quadIndices.size(); i++)
			{
				quadIndexArray[i] = quadIndices[i];
				// fprintf(stdout, "%d\n", quadIndexArray[i]);
			}
			// fprintf(stdout, "%d: %d\n", quadIndices.size()-1, quadIndexArray[quadIndices.size()-1]); // Check for overflow

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			glDrawElements(GL_QUADS, quadIndices.size(), GL_UNSIGNED_INT, quadIndexArray);
		}
	} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
}

void Chamber::Draw(bool hasControl)
{
	if (Toggles::showNormals) DrawNormals(0.2);

	if (hasControl)
	{
		if (Toggles::debug)
		{
			PushShader(Shader::fixedPipeline);
			glPushAttrib(GL_ENABLE_BIT); glDisable(GL_TEXTURE_2D);
		}
		else
		{
			PushShader(Shader::threeDshader);
		}
	}

	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		// Perturbed geometry
		if (Toggles::Noise::showPerturbed)
			DrawHelper(vertices);

		// Baseline geometry
		if (Toggles::Noise::showBase)
		{
			if (Toggles::Noise::showPerturbed && hasControl)
			{ // Use white, transparent wireframe
				glColor4f(1, 1, 1, 0.5);
				glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
				glDisable(GL_TEXTURE_2D);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} // else: Use colored, textured geometry

			DrawHelper(baseVertices);
			
			if (Toggles::Noise::showPerturbed && hasControl)
			{
				glPopAttrib();
				glColor4f(1, 1, 1, 1);
			}
		}
	} glPopMatrix();

	if (hasControl)
	{
		if (Toggles::debug) glPopAttrib();
		PopShader();
	}

	ErrCheck("Chamber::Draw");
}