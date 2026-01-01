cmake_minimum_required(VERSION 3.20)

# Collect all source files
set(CORE_SOURCES
    core/GitService.cpp
    core/SnapshotManager.cpp
    core/PresetManager.cpp
    core/ProjectConfig.cpp
)

set(CORE_HEADERS
    core/GitService.h
    core/SnapshotManager.h
    core/PresetManager.h
    core/ProjectConfig.h
    core/types/Result.h
    core/types/Snapshot.h
    core/types/GamePreset.h
)

set(UI_SOURCES
    ui/MainWindow.cpp
    ui/CreateSnapshotDialog.cpp
    ui/RestoreDialog.cpp
    ui/SettingsDialog.cpp
    ui/SnapshotListWidget.cpp
    ui/models/SnapshotListModel.cpp
)

set(UI_HEADERS
    ui/MainWindow.h
    ui/CreateSnapshotDialog.h
    ui/RestoreDialog.h
    ui/SettingsDialog.h
    ui/SnapshotListWidget.h
    ui/models/SnapshotListModel.h
)

set(UTILS_SOURCES
    utils/FileUtils.cpp
    utils/PathDetector.cpp
    utils/Logger.cpp
)

set(UTILS_HEADERS
    utils/FileUtils.h
    utils/PathDetector.h
    utils/Logger.h
)

# Main executable
add_executable(vgvc
    main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${UTILS_SOURCES}
    ${UTILS_HEADERS}
)

# Link Qt libraries
target_link_libraries(vgvc
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
)

# Set executable properties
set_target_properties(vgvc PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Installation
install(TARGETS vgvc
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
