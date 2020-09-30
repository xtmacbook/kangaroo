
# ================================================= flashBuffer===================================================================================

set(TFlashBUffer_Source_Files test_flash_buffer.cpp)
add_executable(test_flash_buffer ${TFlashBUffer_Source_Files})
add_dependencies(test_flash_buffer libengine)
source_group("Source Files" FILES ${TFlashBUffer_Source_Files})
set_target_properties(test_flash_buffer PROPERTIES PROJECT_LABEL "test_flash_buffer")						# in Visual Studio
set_target_properties(test_flash_buffer PROPERTIES OUTPUT_NAME "test_flash_buffer")
set_target_properties(test_flash_buffer PROPERTIES FOLDER "test/flashbuffer")
target_link_libraries(test_flash_buffer libengine) 
 
#install
install(TARGETS test_flash_buffer RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# ================================================= drawApi===================================================================================

set(drawApi_Source_Files drawApi.cpp)
add_executable(test_drawApi ${drawApi_Source_Files})
add_dependencies(test_drawApi libengine)
source_group("Source Files" FILES ${TFlashBUffer_Source_Files})
set_target_properties(test_drawApi PROPERTIES PROJECT_LABEL "test_drawApi")						# in Visual Studio
set_target_properties(test_drawApi PROPERTIES OUTPUT_NAME "test_drawApi")
set_target_properties(test_drawApi PROPERTIES FOLDER "test/test_drawApi")
target_link_libraries(test_drawApi libengine) 
 
#install
install(TARGETS test_drawApi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)



# ================================================= multThread===================================================================================

set(multThread_Source_Files multThread.cpp render.h render.cpp)
add_executable(test_multThread ${multThread_Source_Files})
add_dependencies(test_multThread libengine)
source_group("Source Files" FILES ${TFlashBUffer_Source_Files})
set_target_properties(test_multThread PROPERTIES PROJECT_LABEL "test_multThread")						# in Visual Studio
set_target_properties(test_multThread PROPERTIES OUTPUT_NAME "test_multThread")
set_target_properties(test_multThread PROPERTIES FOLDER "test/multThread")
target_link_libraries(test_multThread libengine) 
 
#install
install(TARGETS test_multThread RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# ================================================= testImage===================================================================================

set(testImage_Source_Files   testImage.cpp)
add_executable(testImage ${testImage_Source_Files})
add_dependencies(testImage libengine)
source_group("Source Files" FILES ${TFlashBUffer_Source_Files})
set_target_properties(testImage PROPERTIES PROJECT_LABEL "testImage")						# in Visual Studio
set_target_properties(testImage PROPERTIES OUTPUT_NAME "testImage")
set_target_properties(testImage PROPERTIES FOLDER "test/testImage")
target_link_libraries(testImage libengine) 
 
#install
install(TARGETS testImage RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


