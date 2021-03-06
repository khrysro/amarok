project(context-analyzer)

find_package(OpenGL REQUIRED)

set(analyzer_SRCS
    AnalyzerApplet.cpp
    AnalyzerBase.cpp
    BallsAnalyzer.cpp
    BlockAnalyzer.cpp
    DiscoAnalyzer.cpp
    ASCIIAnalyzer.cpp
    fht.cpp
)

include_directories(${OPENGL_INCLUDE_DIR})

add_library(amarok_context_applet_analyzer MODULE ${analyzer_SRCS})

if(APPLE)
    set_target_properties(amarok_context_applet_analyzer PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()


target_link_libraries(amarok_context_applet_analyzer
    amarokcore
    amaroklib
    KF5::Plasma
    Qt5::OpenGL
    ${OPENGL_gl_LIBRARY}
)

install(TARGETS amarok_context_applet_analyzer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES amarok-context-applet-analyzer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
