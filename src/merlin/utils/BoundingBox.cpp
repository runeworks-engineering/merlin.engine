#include "pch.h"
#include "merlin/utils/boundingBox.h"

namespace Merlin {
	BoundingBox BoundingBox::unionBox(BoundingBox a, BoundingBox b) {
        BoundingBox result;

        // Compute the new min and max
        result.min = glm::min(a.min, b.min);
        result.max = glm::max(a.max, b.max);

        // Compute the centroid as the midpoint of the new bounding box
        result.centroid = (result.min + result.max) * 0.5f;

        return result;
	}

}