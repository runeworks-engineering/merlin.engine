#pragma once
#include "merlin/core/core.h"

namespace Merlin {
	struct BoundingBox {
		glm::vec3 min, max, centroid, size;

		static BoundingBox unionBox(BoundingBox a, BoundingBox b);
	};
}