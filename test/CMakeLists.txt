
# ================================================= drawApi===================================================================================

set(drawApi_Source_Files drawApi.cpp)
add_executable(test_drawApi ${drawApi_Source_Files})
add_dependencies(test_drawApi ${engineName})
source_group("Source Files" FILES ${drawApi_Source_Files})
set_target_properties(test_drawApi PROPERTIES PROJECT_LABEL "test_drawApi")						# in Visual Studio
set_target_properties(test_drawApi PROPERTIES OUTPUT_NAME "test_drawApi")
set_target_properties(test_drawApi PROPERTIES FOLDER "test")
target_link_libraries(test_drawApi ${engineName}) 
 
#install
install(TARGETS test_drawApi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# ================================================= testClipmap===================================================================================
set( testClipmap_Source_Files    testClipmap.cpp jpgd.cpp clipmap_prepprocessor.cpp jpeg_gpu.cpp)
add_executable( testClipmap ${testClipmap_Source_Files})
add_dependencies(testClipmap ${engineName})
source_group("Source Files" FILES ${testClipmap_Source_Files})
set_target_properties(testClipmap PROPERTIES PROJECT_LABEL "testClipmap")						# in Visual Studio
set_target_properties(testClipmap PROPERTIES OUTPUT_NAME "testClipmap")
set_target_properties(testClipmap PROPERTIES FOLDER "test")
target_link_libraries(testClipmap ${engineName}) 
 
#install
install(TARGETS testClipmap RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
