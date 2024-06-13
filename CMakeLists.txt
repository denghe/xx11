cmake_minimum_required(VERSION 3.10)

set(name xx11)
project(${name})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
	src/
	3rd/nameof/include
	3rd/xx
	3rd/
)

file(GLOB sources
	src/*.h
	src/*.hpp
	src/*.cpp
	src/*.c
	3rd/xx/*.*
)
add_executable(${name} ${sources})

source_group("" FILES ${sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../ FILES ${sources})

target_link_libraries(${name} winmm d3d11 d3dcompiler dxguid)	# dxgi ?

if(MSVC)

	target_link_options(${name} PRIVATE "/SUBSYSTEM:WINDOWS")
	add_compile_definitions(UNICODE _UNICODE)
	add_compile_options("/utf-8")

	target_link_options(${name} PRIVATE
		$<$<CONFIG:MinSizeRel>:/LTCG>
		$<$<CONFIG:Release>:/LTCG>
		$<$<CONFIG:RelWithDebInfo>:/LTCG>
	)

	target_compile_options(${name} PRIVATE
		"$<$<CONFIG:MinSizeRel>:/GL>"
		"$<$<CONFIG:Release>:/GL>"
		"$<$<CONFIG:RelWithDebInfo>:/GL>"
	)

	target_link_options(${name} PRIVATE
		$<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>
		$<$<CONFIG:MinSizeRel>:/NODEFAULTLIB:libcmt.lib>
		$<$<CONFIG:Release>:/NODEFAULTLIB:libcmt.lib>
		$<$<CONFIG:RelWithDebInfo>:/NODEFAULTLIB:libcmt.lib>
	)
	
	target_compile_definitions(${name} PRIVATE
		"$<$<CONFIG:MinSizeRel>:_SECURE_SCL=0>"
		"$<$<CONFIG:MinSizeRel>:_HAS_ITERATOR_DEBUGGING=0>"
		"$<$<CONFIG:Release>:_SECURE_SCL=0>"
		"$<$<CONFIG:Release>:_HAS_ITERATOR_DEBUGGING=0>"
		"$<$<CONFIG:RelWithDebInfo>:_SECURE_SCL=0>"
		"$<$<CONFIG:RelWithDebInfo>:_HAS_ITERATOR_DEBUGGING=0>"
	)

	add_compile_options($<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
	target_compile_options(${name} PRIVATE
		$<$<PLATFORM_ID:Windows>:/fp:fast>
		$<$<PLATFORM_ID:Windows>:/std:c17>
		$<$<PLATFORM_ID:Windows>:/bigobj>
		$<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>
	)

	set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

	set_source_files_properties(src/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
	target_compile_options(${name} PRIVATE $<$<PLATFORM_ID:Windows>:/Yupch.h>)
	
else()

	target_precompile_headers(${name}
		PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>
	)

endif()
