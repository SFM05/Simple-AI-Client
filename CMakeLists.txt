cmake_minimum_required(VERSION 3.16)

project(Simple_AI_Client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        Main.cpp
        UI/MainWindow.cpp
        UI/MainWindow.h
        UI/MainWindow.ui
        UI/modelmanagement.cpp
        UI/modelmanagement.h
        UI/modelmanagement.ui
        UI/modelsettings.cpp
        UI/modelsettings.h
        UI/modelsettings.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Simple_AI_Client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Model/Message.h
        Model/Message.cpp
        Network/ApiClient.h
        Network/ApiClient.cpp
        Model/Conversation.h
        Model/Conversation.cpp
        Model/ModelConfig.h
        Model/ModelConfig.cpp

        Control/AiRegistry.cpp
        Control/AiRegistry.h
        Control/ConversationManager.h
        Control/ConversationManager.cpp
        Control/NetControl.h
        Control/NetControl.cpp
        UI/modelmanagement.h UI/modelmanagement.cpp UI/modelmanagement.ui
        UI/modelsettings.h UI/modelsettings.cpp UI/modelsettings.ui
        UI/ConversationListWidget.h
        UI/ConversationListWidget.cpp
        UI/ChatItemWidget.h
        UI/ChatItemWidget.cpp
        UI/ChatWidget.cpp
        UI/ChatWidget.h





    )
endif()

# 添加包含目录
target_include_directories(Simple_AI_Client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Simple_AI_Client PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Simple_AI_Client)
endif()
set_target_properties(Simple_AI_Client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Simple_AI_Client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Simple_AI_Client)
endif()
