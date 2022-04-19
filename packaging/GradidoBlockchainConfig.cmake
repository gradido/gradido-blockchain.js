cmake_minimum_required(VERSION 3.18.2)

set(GradidoBlockchain_known_comps static shared)
set(GradidoBlockchain_comp_static NO)
set(GradidoBlockchain_comp_shared NO)
foreach (GradidoBlockchain_comp IN LISTS ${CMAKE_FIND_PACKAGE_NAME}_FIND_COMPONENTS)
    if (GradidoBlockchain_comp IN_LIST GradidoBlockchain_known_comps)
        set(GradidoBlockchain_comp_${GradidoBlockchain_comp} YES)
    else ()
        set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
            "GradidoBlockchain does not recognize component `${GradidoBlockchain_comp}`.")
        set(${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
        return()
    endif ()
endforeach ()

if (GradidoBlockchain_comp_static AND GradidoBlockchain_comp_shared)
    set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
        "GradidoBlockchain `static` and `shared` components are mutually exclusive.")
    set(${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    return()
endif ()

set(GradidoBlockchain_static_targets "${CMAKE_CURRENT_LIST_DIR}/GradidoBlockchain-static-targets.cmake")
set(GradidoBlockchain_shared_targets "${CMAKE_CURRENT_LIST_DIR}/GradidoBlockchain-shared-targets.cmake")

macro(GradidoBlockchain_load_targets type)
    if (NOT EXISTS "${GradidoBlockchain_${type}_targets}")
        set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
            "GradidoBlockchain `${type}` libraries were requested but not found.")
        set(${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
        return()
    endif ()
    include("${GradidoBlockchain_${type}_targets}")
endmacro()

if (GradidoBlockchain_comp_static)
    GradidoBlockchain_load_targets(static)
elseif (GradidoBlockchain_comp_shared)
    GradidoBlockchain_load_targets(shared)
elseif (DEFINED GradidoBlockchain_SHARED_LIBS AND GradidoBlockchain_SHARED_LIBS)
    GradidoBlockchain_load_targets(shared)
elseif (DEFINED GradidoBlockchain_SHARED_LIBS AND NOT GradidoBlockchain_SHARED_LIBS)
    GradidoBlockchain_load_targets(static)
elseif (BUILD_SHARED_LIBS)
    if (EXISTS "${GradidoBlockchain_shared_targets}")
        GradidoBlockchain_load_targets(shared)
    else ()
        GradidoBlockchain_load_targets(static)
    endif ()
else ()
    if (EXISTS "${GradidoBlockchain_static_targets}")
        GradidoBlockchain_load_targets(static)
    else ()
        GradidoBlockchain_load_targets(shared)
    endif ()
endif ()