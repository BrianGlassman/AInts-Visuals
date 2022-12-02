#include "Colony.hpp"

#include "Chamber.hpp"
#include "globals.hpp"
#include "Hill.hpp"
#include "Shaders.hpp"
#include "Tunnel.hpp"
#include "Vector.hpp"

using ChildMap = std::unordered_map<Vector3Int, std::shared_ptr<Structure>>;
using OffsetsMap = std::unordered_map<Vector3Int, int>;

Colony::Colony()
{
    type = 4;

    // Specifically do not call Create() so that it can be called after populating
}

std::vector<Vertex>* Colony::getCL()
{
	if (Toggles::Noise::showPerturbed)
		return &centerline;
	else
		return &baseCenterline;
}


void LinkEndpoints(ChildMap& children, OffsetsMap& offsets, std::vector<Vertex>& dstCL, Vector3Int srcEPdir)
{
    int srcOffset, dstOffset;

    for (auto& childPair : children)
    {
        auto& srcCoords = childPair.first;
        auto& srcChild = childPair.second;

        srcOffset = offsets[srcCoords];
        auto srcEnd = srcChild->GetEndpoint(srcEPdir);
        if (srcEnd == -1) continue;

        Vector3Int dstCoords({
            srcCoords.x + srcEPdir.x,
            srcCoords.y + srcEPdir.y,
            srcCoords.z + srcEPdir.z});
        dstOffset = offsets.at(dstCoords);
        auto& dstChild = children.at(dstCoords);

        // Source/Destination endpoints are on opposing sides
        Vector3Int dstEPdir = srcEPdir.Reversed();

        auto dstEnd = dstChild->GetEndpoint(dstEPdir);
        if (dstEnd == -1) continue;

        int offset;
        dstCL[srcEnd + srcOffset].AddNeighbor(dstEnd + dstOffset);
        // Don't need to do the reciprocal, will be caught when processing the opposite direction
    }
}

void MergeCoincident(std::vector<Vertex>& dstCL, bool debug = false)
{
    std::vector<int> coincident; // Coincident indices within CL
    for (auto& v0 : dstCL)
    {
        auto& v0_CLidx = v0.idx;
        coincident.clear();

        // Check the neighbors of v0
        for (unsigned int v0_nIdx = 0; v0_nIdx < v0.neighbors.size(); v0_nIdx++)
        {
            int v1_CLidx = v0.neighbors[v0_nIdx]; // The neighbor's index within the CL
            if (v1_CLidx < v0_CLidx) continue; // Skip lower-indexed neighbors, already processed
            auto& v1 = dstCL[v1_CLidx];
            if (abs(v0.x() - v1.x()) < 1e-6 &&
                abs(v0.y() - v1.y()) < 1e-6 &&
                abs(v0.z() - v1.z()) < 1e-6)
            {
                // Coincident point found
                coincident.push_back(v1_CLidx);
            }
        }

        // For each coincident point found...
        for (auto& v1_CLidx : coincident)
        {
            // ...remove v0's link to it... (will be replaced later)
            v0.RemoveNeighbor(v1_CLidx, debug);

            // ...relink all of v1's links to be v0's links
            for (auto& v2_CLidx : dstCL[v1_CLidx].neighbors)
            {
                // Skip v0
                if (v2_CLidx == v0_CLidx) continue;

                auto& v2 = dstCL[v2_CLidx];
                v0.AddNeighbor(v2_CLidx, debug);
                v2.ReplaceNeighbor(v1_CLidx, v0_CLidx, debug);
            }
        }
    }
}

void Colony::CenterlineHelper(std::vector<Vertex>& dstCL, bool usePerturbed)
{
    // Copy child centerlines to self
    int idxOffset;
    for (auto&& childPair : children)
    {
        auto& coords = childPair.first;
        auto& child = childPair.second;
        idxOffset = offsets.at(coords);

        std::vector<Vertex>* srcCL;
        if (usePerturbed)
        {
            srcCL = child->getPerturbedCL();
        }
        else
        {
            srcCL = child->getBaseCL();
        }

        auto& center = child->center;
        // Create the points
        for (auto&& src : *(srcCL))
        {
            int index = dstCL.size();
            Vertex dst(index);
            float worldX = src.x() + center.x;
            float worldY = src.y() + center.y;
            float worldZ = src.z() + center.z;
            // printf("%f + %f = %f\n", src.x(), center.x, worldX);
            dst.coords.x = worldX;
            dst.coords.y = worldY;
            dst.coords.z = worldZ;
            // printf("%d: %f, %f, %f\n", index, dst.x(), dst.y(), dst.z());

            dstCL.push_back(dst);
        }

        // Link, once all points have been created
        for (auto&& childVert : *(srcCL))
        {
            int cIdx = childVert.idx + idxOffset;
            for (auto&& neighbor : childVert.neighbors)
            {
                int nIdx = neighbor + idxOffset;
                dstCL[cIdx].AddNeighbor(nIdx);
                // Don't need to do the reciprocal, will be caught when processing the neighbor vertex
            }
        }
    }

    // Link endpoints from adjacent children
    LinkEndpoints(children, offsets, dstCL, Vector3Int::Forward);
    LinkEndpoints(children, offsets, dstCL, Vector3Int::Backward);
    LinkEndpoints(children, offsets, dstCL, Vector3Int::Left);
    LinkEndpoints(children, offsets, dstCL, Vector3Int::Right);
    LinkEndpoints(children, offsets, dstCL, Vector3Int::Up);
    LinkEndpoints(children, offsets, dstCL, Vector3Int::Down);

    // Merge coincident points now that all creation and linking is done
    MergeCoincident(dstCL);
}

void Colony::Create()
{
    // FIXME this overload is only needed for temp motion model

    PreCreate();

    // Create the offsets map
    int idxOffset = 0;
    for (auto&& childPair : children)
    {
        auto& coords = childPair.first;
        auto& child = childPair.second;

        offsets.insert({coords, idxOffset});

        idxOffset += child->getCL()->size();
    }

    CenterlineHelper(centerline, true);
    CenterlineHelper(baseCenterline, false);

    PostCreate();
}

void Colony::ApplyNoise(float offset[])
{
    float newOffset[] = {offset[0] + center[0], offset[1] + center[1], offset[2] + center[2]};
	for (auto&& child : children)
	{
		child.second->ApplyNoise(newOffset);
	}
}

void Colony::DrawCenterlines()
{
	PushShader(Shader::fixedPipeline);
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPointSize(7);
	glColor3f(0, 1, 1);

	auto& CLtoUse = *getCL();
	if (CLtoUse.size() == 0)
	{ // Exit early to prevent SegFault
		fprintf(stderr, "Called DrawCenterLines with no elements\n");
		glColor3f(1, 1, 1);
		glPopAttrib();
		PopShader();
		return;
	}

	std::unordered_set<int> closedSet;
	std::unordered_set<int> frontier;
	frontier.insert(0);
	glBegin(GL_POINTS); glVertex3f(CLtoUse[0].x(), CLtoUse[0].y(), CLtoUse[0].z()); glEnd();

	// Depth-first iteration
	while (frontier.size() > 0)
	{
		// Pop the next point to look at
		auto currentIdx = *frontier.begin();
		frontier.erase(frontier.begin());
		auto current = CLtoUse[currentIdx];
		// printf("Current has %lu neighbors\n", current.neighbors.size());

		for (auto&& neighborIdx : current.neighbors)
		{
			auto&& neighbor = CLtoUse[neighborIdx];

			// Draw line from this point to the neighbor
			glBegin(GL_LINES);
			glVertex3f( current.x(),  current.y(),  current.z());
			// printf("current %f, %f, %f\n", current.x(),  current.y(),  current.z());
			glVertex3f(neighbor.x(), neighbor.y(), neighbor.z());
			// printf("neighbor %f, %f, %f\n", neighbor.x(),  neighbor.y(),  neighbor.z());
			glEnd();

			// Add the neighbor to the frontier (if not already in closedSet or frontier)
			if (closedSet.find(neighborIdx) == closedSet.end() && frontier.find(neighborIdx) == frontier.end())
			{
				frontier.insert(neighborIdx);

				// Draw a point
				glBegin(GL_POINTS); glVertex3f(neighbor.x(), neighbor.y(), neighbor.z()); glEnd();
			}

		}
		closedSet.insert(currentIdx);
	}
	
	glColor3f(1, 1, 1);
	glPopAttrib();
	PopShader();

	ErrCheck("Colony::DrawCenterlines\n");
}

void Colony::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

        for(auto& child : children)
        {
            child.second->Draw();
        }

        if(Toggles::showCenterlines) DrawCenterlines();
    } glPopMatrix();
}

void Colony::AddStructure(std::shared_ptr<Structure> structPtr, Vector3 center)
{
    structPtr->center = center;
    Vector3Int intCenter(center);
    children.insert({intCenter, structPtr});
}

void Colony::AddTunnel(Vector3 center)
{
    std::shared_ptr<Structure> ptr = std::make_shared<Tunnel>();
    AddStructure(ptr, center);
}
void Colony::AddTunnel(Vector3 center, unsigned char sides)
{
    std::shared_ptr<Structure> ptr = std::make_shared<Tunnel>(sides);
    AddStructure(ptr, center);
}
void Colony::AddTunnel(float x, float y, float z)
{
    float center[] = {x, y, z};
    AddTunnel(center);
}
void Colony::AddTunnel(float x, float y, float z, unsigned char sides)
{
    std::shared_ptr<Structure> ptr = std::make_shared<Tunnel>(sides);
    float center[] = {x, y, z};
    AddStructure(ptr, center);
}

void Colony::AddChamber(Vector3 center)
{
    std::shared_ptr<Structure> ptr = std::make_shared<Chamber>();
    AddStructure(ptr, center);
}
void Colony::AddChamber(Vector3 center, unsigned char sides)
{
    std::shared_ptr<Structure> ptr = std::make_shared<Chamber>(sides);
    AddStructure(ptr, center);
}
void Colony::AddChamber(float x, float y, float z)
{
    float center[] = {x, y, z};
    AddChamber(center);
}
void Colony::AddChamber(float x, float y, float z, unsigned char sides)
{
    float center[] = {x, y, z};
    AddChamber(center, sides);
}

void Colony::AddHill(Vector3 center)
{
    std::shared_ptr<Structure> ptr = std::make_shared<Hill>();
    AddStructure(ptr, center);
}
void Colony::AddHill(float x, float y, float z)
{
    float center[] = {x, y, z};
    AddHill(center);
}