#include "Colony.hpp"

#include "Chamber.hpp"
#include "globals.hpp"
#include "Hill.hpp"
#include "Mine.hpp"
#include "Shaders.hpp"
#include "Tunnel.hpp"
#include "Vector.hpp"

using ChildMap = std::unordered_map<Vector3Int, std::shared_ptr<Structure>>;
using OffsetsMap = std::unordered_map<Vector3Int, int>;

Colony::Colony()
{
    type = StructureType::Colony;

    // Specifically do not call Create() so that it can be called after populating
}

std::vector<Vertex>* Colony::getCL()
{
	if (Toggles::Noise::showPerturbed)
		return &centerline;
	else
		return &baseCenterline;
}

void Colony::PreCreate()
{
	Model::PreCreate();
	centerline.clear();
	baseCenterline.clear();
}

void LinkEndpoints(ChildMap& children, OffsetsMap& offsets, std::vector<Vertex>& dstCL, Vector3Int srcEPdir)
{
    int srcOffset, dstOffset;

    for (auto& childPair : children)
    {
        auto& srcCoords = childPair.first;
        auto& srcChild = childPair.second;

        if (offsets.count(srcCoords) == 0) Fatal(999, "Offsets does not contain srcCoords (%d, %d, %d)\n", srcCoords.x, srcCoords.y, srcCoords.z);
        srcOffset = offsets.at(srcCoords);
        auto srcEnd = srcChild->GetEndpoint(srcEPdir);
        if (srcEnd == -1) continue;

        Vector3Int dstCoords = srcCoords + srcEPdir;
        if (offsets.count(dstCoords) == 0) Fatal(999, "Offsets does not contain dstCoords (%d, %d, %d)\n", dstCoords.x, dstCoords.y, dstCoords.z);
        dstOffset = offsets.at(dstCoords);
        if (children.count(dstCoords) == 0) Fatal(999, "Children does not contain dstCoords (%d, %d, %d)\n", dstCoords.x, dstCoords.y, dstCoords.z);
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
            // printf("%f + %f = %f\n", src.x(), center.x, worldX); // NORELEASE
            dst.coords.x = worldX;
            dst.coords.y = worldY;
            dst.coords.z = worldZ;
            // printf("%d: %f, %f, %f\n", index, dst.x(), dst.y(), dst.z()); // NORELEASE

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
    for (auto& direction : Vector3Int::directions)
    {
        LinkEndpoints(children, offsets, dstCL, direction);
    }

    // Merge coincident points now that all creation and linking is done
    MergeCoincident(dstCL);
}

void Colony::Create()
{
    PreCreate();

    // Update the sides for each Structure
    Vector3Int testCoords;
    bool val;
    for (auto&& childPair : children)
    {
        auto& coords = childPair.first;
        auto& child = childPair.second;

        // Update each side
        for (auto& direction : Vector3Int::directions)
        {
            testCoords = coords + direction;
            val = (children.count(testCoords) > 0);
            if (val && children.at(testCoords)->sideType == SideType::hill && direction != Vector3Int::Up)
            {
                // Hills can only connect on the bottom (i.e. direction from neighbor is Up)
                val = false;
            }
            child->SetSide(direction, val);
        }

        // Re-create the Structure with updated sides
        child->Create();
    }

    // Create the offsets map
    int idxOffset = 0;
    for (auto&& childPair : children)
    {
        auto& coords = childPair.first;
        auto& child = childPair.second;

        if (offsets.count(coords) > 0)
        {
            // Replace existing value
            offsets.at(coords) = idxOffset;
        }
        else
        {
            // Add new value
            auto ret = offsets.insert({coords, idxOffset});
            if (!ret.second) Fatal(999, "Failed to insert offset\n");
        }

        idxOffset += child->getCL()->size();
    }

    CenterlineHelper(centerline, true);
    CenterlineHelper(baseCenterline, false);

    PostCreate();
}

void Colony::ApplyNoise()
{
    // Apply to each child
	for (auto&& child : children)
	{
		child.second->ApplyNoise();
	}

	// Apply to the centerline
	for (unsigned int i = 0; i < baseCenterline.size(); i++)
	{
		Vector3 p = noisePtr->getNoise(baseCenterline[i].coords + center);

		centerline[i].coords = baseCenterline[i].coords + p*noiseScale;
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
		// printf("Current has %lu neighbors\n", current.neighbors.size()); // NORELEASE

		for (auto&& neighborIdx : current.neighbors)
		{
			auto&& neighbor = CLtoUse[neighborIdx];

			// Draw line from this point to the neighbor
			glBegin(GL_LINES);
			glVertex3f( current.x(),  current.y(),  current.z());
			// printf("current %f, %f, %f\n", current.x(),  current.y(),  current.z()); // NORELEASE
			glVertex3f(neighbor.x(), neighbor.y(), neighbor.z());
			// printf("neighbor %f, %f, %f\n", neighbor.x(),  neighbor.y(),  neighbor.z()); // NORELEASE
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

	ErrCheck("Colony::DrawCenterlines");
}

void Colony::Draw(bool hasControl)
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

        for(auto& child : children)
        {
            child.second->Draw(hasControl);
        }

        if(Toggles::showCenterlines) DrawCenterlines();
    } glPopMatrix();

    ErrCheck("Colony::Draw");
}

void Colony::AddStructure(Vector3Int center, StructureType type)
{
    std::shared_ptr<Structure> ptr;
    switch(type)
    {
    case StructureType::Tunnel:
        ptr = std::make_shared<Tunnel>();
        break;
    case StructureType::Chamber:
        ptr = std::make_shared<Chamber>();
        break;
    case StructureType::Mine:
        ptr = std::make_shared<Mine>();
        break;
    case StructureType::Hill:
        ptr = std::make_shared<Hill>();
        break;
    default:
        Fatal(999, "Colony::AddStructure Unrecognized StructureType %d\n", type);
    }
    ptr->center = center;

    if (children.count(center) > 0)
    {
        // Replace existing child
        children.at(center) = ptr;
    }
    else
    {
        // Add new child
        auto ret = children.insert({center, ptr});
        if (!ret.second) Fatal(999, "Failed to insert child\n");
    }
}
void Colony::AddStructure(int x, int y, int z, StructureType type)
{
    AddStructure({x, y, z}, type);
}

void Colony::DeleteStructure(Vector3Int center)
{
    if (children.count(center) > 0)
    {
        children.erase(center);
    }
    else
    {
        Fatal(999, "Trying to delete structure that doesn't exist\n");
    }
}
void Colony::DeleteStructure(int x, int y, int z)
{
    DeleteStructure({x, y, z});
}

std::shared_ptr<Structure> Colony::GetChild(Vector3Int coords)
{
    if (children.count(coords) > 0)
    {
        return children.at(coords);
    }
    else
    {
        return nullptr;
    }
}