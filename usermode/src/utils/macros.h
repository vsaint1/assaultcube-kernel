#pragma once
#include <string>

// @note: spdlog msvc deprecated usage
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#ifdef _DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#include <spdlog/spdlog.h>

#endif

#if _DEBUG
#define BUILD_TYPE "[DEBUG] - Metrics, build "##__TIMESTAMP__
#else
#define BUILD_TYPE "[RELEASE] - Metrics, build "##__TIMESTAMP__
#endif


#define GLSL_VERSION "#version 130"

#define AC_MODULE_BASE_SIZE 0x01B9000

#define GAME_WINDOW_NAME "AssaultCube"


