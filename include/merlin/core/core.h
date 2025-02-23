#pragma once

#include "merlin/core/glObject.h"
#include <memory>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef GLCORE_DEBUG
	#define GLCORE_ENABLE_ASSERTS
#endif

#ifdef GLCORE_ENABLE_ASSERTS
	#define GLCORE_ASSERT(x, s, m) { if(!(x)) { LOG_ERROR(s) << m << LOG_ENDL __debugbreak(); } }
#else
	#define GLCORE_ASSERT(x, s, m)
#endif

#define MERLIN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define BIT(x) (1 << x)

#define SINGLETON(className) public: \
	static className& instance() { \
		static std::unique_ptr<className> instance(new className()); \
		return *instance.get(); \
	} \
	className(const className&) = delete; \
	className(className&&) = delete; \
	className& operator=(const className&) = delete; \
	className& operator=(className&&) = delete; \



template<typename T>
using scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr scope<T> createScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using shared = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr shared<T> createShared(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

