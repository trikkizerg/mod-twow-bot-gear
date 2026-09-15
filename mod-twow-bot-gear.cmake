# CMake hooks for mod-twow-bot-gear.
#
# We need PlayerbotFactory.h (from the TortoiseBots module) on the include path.
# In static linkage, all modules build into the single `modules` target, so
# adding the include dirs here makes PlayerbotFactory.h reachable.

if(TORTOISE_MODULE_CMAKE_PHASE STREQUAL "POST_TARGETS")
  if(TORTOISE_CURRENT_MODULE_LINKAGE STREQUAL "static")
    set(BOTGEAR_TARGET modules)
  elseif(DEFINED TORTOISE_CURRENT_MODULE_TARGET
         AND NOT "${TORTOISE_CURRENT_MODULE_TARGET}" STREQUAL "")
    set(BOTGEAR_TARGET "${TORTOISE_CURRENT_MODULE_TARGET}")
  else()
    GetModuleProjectName("${TORTOISE_CURRENT_MODULE}" BOTGEAR_TARGET)
  endif()

  if(TARGET "${BOTGEAR_TARGET}")
    # Make PlayerbotFactory.h (and everything it includes) reachable.
    target_include_directories("${BOTGEAR_TARGET}" PRIVATE
      "${CMAKE_SOURCE_DIR}/modules/TortoiseBots/ai/playerbot"
      "${CMAKE_SOURCE_DIR}/modules/TortoiseBots/ai"
      "${CMAKE_SOURCE_DIR}/modules/TortoiseBots/runtime")
  endif()
endif()
