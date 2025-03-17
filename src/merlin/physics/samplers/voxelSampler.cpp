#include "pch.h"
#include "merlin/physics/samplers/voxelSampler.h"

namespace Merlin {

    VoxelSampler::VoxelSampler(float spacing, float thickness) : m_spacing(spacing), m_thickness(thickness){
    }

    std::vector<glm::vec4> VoxelSampler::sample(Mesh_Ptr mesh) {
        // Ensure the mesh is ready for voxelization.
        if(!mesh->hasBoundingBox());
            mesh->computeBoundingBox();

        if (m_thickness == 0) mesh->voxelize(m_spacing);
        else mesh->voxelizeSurface(m_spacing, m_thickness);

        // Retrieve the axis-aligned bounding box.
        BoundingBox aabb = mesh->getBoundingBox();
        glm::vec3 bbSize = aabb.max - aabb.min;

        // Calculate grid dimensions based on the spacing.
        int gridSizeX = static_cast<int>(std::ceil(bbSize.x / m_spacing));
        int gridSizeY = static_cast<int>(std::ceil(bbSize.y / m_spacing));
        int gridSizeZ = static_cast<int>(std::ceil(bbSize.z / m_spacing));
        int totalVoxels = gridSizeX * gridSizeY * gridSizeZ;

        // Retrieve voxel data from the mesh.
        const auto& voxels = mesh->getVoxels();

        // Reserve memory for particles; note that not every voxel will be active.
        std::vector<glm::vec4> particles;
        particles.reserve(totalVoxels);

        // Loop over each voxel in the grid.
        for (int i = 0; i < totalVoxels; i++) {
            if (voxels[i] != 0) { // If the voxel is active...
                int index = i;
                int vz = index / (gridSizeX * gridSizeY);
                index -= (vz * gridSizeX * gridSizeY);
                int vy = index / gridSizeX;
                int vx = index % gridSizeX;

                // Compute the particle position within the bounding box.
                float x = aabb.min.x + (vx + 0.5f) * m_spacing;
                float y = aabb.min.y + (vy + 0.5f) * m_spacing;
                float z = aabb.min.z + (vz + 0.5f) * m_spacing;

                particles.push_back(glm::vec4(x, y, z, 1.0));

            }
        }

        return particles;
    }


}