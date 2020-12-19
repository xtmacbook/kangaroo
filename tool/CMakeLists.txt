
# ================================================= Image MipMapping ===================================================================================

set(image_mipmapping_Source_Files mipmapping.cpp)
add_executable(image_mipmapping ${image_mipmapping_Source_Files})
add_dependencies(image_mipmapping ${engineName})
source_group("Source Files" FILES ${image_mipmapping_Source_Files})
set_target_properties(image_mipmapping PROPERTIES PROJECT_LABEL "image_mipmapping")						 
set_target_properties(image_mipmapping PROPERTIES OUTPUT_NAME "image_mipmapping")
set_target_properties(image_mipmapping PROPERTIES FOLDER "tool/mipmapping")
target_link_libraries(image_mipmapping ${engineName}) 
#install
install(TARGETS image_mipmapping RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# ================================================= 1D Noise ===================================================================================


  