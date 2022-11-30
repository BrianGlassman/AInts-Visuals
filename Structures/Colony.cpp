#include "Colony.hpp"

#include "Chamber.hpp"
#include "globals.hpp"
#include "Hill.hpp"
#include "Shaders.hpp"
#include "Tunnel.hpp"

Colony::Colony()
{
    type = 4;
}

std::vector<Vertex>* Colony::getCL()
{
	if (Toggles::Noise::showPerturbed)
		return &centerline;
	else
		return &baseCenterline;
}


void LinkEndpoints(std::vector<std::shared_ptr<Structure>>& children, std::vector<int>& offsets, std::vector<Vertex>& dstCL, int srcEPidx)
{
    int srcOffset, dstOffset;

    for (unsigned int srcSIdx = 0; srcSIdx < children.size(); srcSIdx++)
    {
        srcOffset = offsets[srcSIdx];
        auto& srcChild = children[srcSIdx];
        auto srcEnd = srcChild->GetEndpoint(srcEPidx);
        if (srcEnd[0] == -1) continue;
        if (srcEnd[1] == -1) Fatal(999, "Source VIdx set but not SIdx\n");
        int srcIdx = srcEnd[0];

        // Source/Destination endpoints are on opposing sides
        int dstEPidx = (srcEPidx % 2 == 0) ? srcEPidx + 1 : srcEPidx - 1;
        // printf("src side = %d, dst side = %d\n", srcEPidx, dstEPidx);

        int dstSIdx = srcEnd[1];
        dstOffset = offsets[dstSIdx];
        auto& dstChild = children[dstSIdx];
        auto dstEnd = dstChild->GetEndpoint(dstEPidx);
        // printf("src Vidx %d, src SIdx %d, dst VIdx %d, dst SIdx %d\n", srcEnd[0], srcEnd[1], dstEnd[0], dstEnd[1]);
        if (dstEnd[0] == -1) continue;
        if (dstEnd[1] == -1) Fatal(999, "Destination VIdx set but not SIdx\n");
        int dstIdx = dstEnd[0];

        int offset;
        dstCL[srcIdx + srcOffset].AddNeighbor(dstIdx + dstOffset);
        // Don't need to do the reciprocal, will be caught when processing the opposite direction
    }
}

void MergeCoincident(std::vector<Vertex>& centerline)
{
    std::vector<int> coincident; // Coincident indices within CL
    for (auto& v0 : centerline)
    {
        auto& v0_CLidx = v0.idx;
        coincident.clear();

        // Check the neighbors of v0
        for (unsigned int v0_nIdx = 0; v0_nIdx < v0.neighbors.size(); v0_nIdx++)
        {
            int v1_CLidx = v0.neighbors[v0_nIdx]; // The neighbor's index within the CL
            if (v1_CLidx < v0_CLidx) continue; // Skip lower-indexed neighbors, already processed
            auto& v1 = centerline[v1_CLidx];
            if (v0.x() == v1.x() &&
                v0.y() == v1.y() &&
                v0.z() == v1.z())
            {
                // Coincident point found
                coincident.push_back(v1_CLidx);
            }
        }

        // For each coincident point found...
        for (auto& v1_CLidx : coincident)
        {
            // ...remove v0's link to it... (will be replaced later)
            v0.RemoveNeighbor(v1_CLidx, false);

            // ...relink all of v1's links to be v0's links
            for (auto& v2_CLidx : centerline[v1_CLidx].neighbors)
            {
                // Skip v0
                if (v2_CLidx == v0_CLidx) continue;

                auto& v2 = centerline[v2_CLidx];
                v0.AddNeighbor(v2_CLidx, false);
                v2.ReplaceNeighbor(v1_CLidx, v0_CLidx, false);
            }
        }
    }
}

void Colony::Create()
{
    // FIXME this overload is only needed for temp motion model

    PreCreate();

    // Copy child centerlines to self
    int idxOffset = 0;
    for (auto&& child : children)
    {
        offsets.push_back(idxOffset);

        auto& center = child->center;
        // Create the points
        for (auto&& src : *(child->getPerturbedCL()))
        {
            int index = centerline.size();
            Vertex dst(index);
            float worldX = src.x() + center.x;
            float worldY = src.y() + center.y;
            float worldZ = src.z() + center.z;
            // printf("%f + %f = %f\n", src.x(), center.x, worldX);
            dst.coords.x = worldX;
            dst.coords.y = worldY;
            dst.coords.z = worldZ;
            // printf("%d: %f, %f, %f\n", index, dst.x(), dst.y(), dst.z());

            centerline.push_back(dst);
        }

        // Link, once all points have been created
        for (auto&& childVert : *(child->getPerturbedCL()))
        {
            int cIdx = childVert.idx + idxOffset;
            for (auto&& neighbor : childVert.neighbors)
            {
                int nIdx = neighbor + idxOffset;
                centerline[cIdx].AddNeighbor(nIdx);
                // Don't need to do the reciprocal, will be caught when processing the neighbor vertex
            }
        }

        idxOffset += child->getPerturbedCL()->size();
    }

    // Link endpoints from adjacent children
    for (int i = 0; i <= 5; i++)
    {
        LinkEndpoints(children, offsets, centerline, i);
    }

    // Merge coincident points now that all creation and linking is done
    MergeCoincident(centerline);

    PostCreate();
}

void Colony::ApplyNoise(float offset[])
{
    float newOffset[] = {offset[0] + center[0], offset[1] + center[1], offset[2] + center[2]};
	for (auto&& child : children)
	{
		child->ApplyNoise(newOffset);
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
		Fatal(999, "Called DrawCenterLines with no elements\n");
	}

	std::unordered_set<int> closedSet;
	std::unordered_set<int> frontier;
	frontier.insert(0);
	glBegin(GL_POINTS); glVertex3f(CLtoUse[0].x(), CLtoUse[0].y(), CLtoUse[0].z()); glEnd();

	// Depth-first iteration
	while (frontier.size() > 0)
	{
		// Pop the next point to look at
		auto currentIdx = (frontier.extract(frontier.begin())).value();
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
            child->Draw();
        }

        if(Toggles::showCenterlines) DrawCenterlines();
    } glPopMatrix();
}

void Colony::AddStructure(std::shared_ptr<Structure> structPtr, Vector3 center)
{
    structPtr->center = center;
    children.push_back(structPtr);
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