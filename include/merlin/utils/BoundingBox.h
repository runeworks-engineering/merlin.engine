#pragma once
#include "merlin/core/core.h"

namespace Merlin {
	struct BoundingBox {
		glm::vec3	min = glm::vec3(0),
					max = glm::vec3(0),
					centroid = glm::vec3(0),
					size = glm::vec3(0);

		bool isEmpty() const;

		static BoundingBox unionBox(BoundingBox a, BoundingBox b);
	};
}