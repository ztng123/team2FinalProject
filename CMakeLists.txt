cmake_minimum_required(VERSION 3.14)
project(carproject)

# 자동 UI, MOC, RCC 처리 활성화
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11 표준 이상 사용
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 모듈 추가
find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    Positioning
    Location
    Quick
    QuickWidgets
    TextToSpeech
    REQUIRED
)

# 소스 파일 목록
set(SOURCES
    main.cpp
    mainwindow.cpp
    mainview.cpp
    locationservice.cpp
    drowsinessdetector.cpp
    alertdialog.cpp
)

# 헤더 파일 목록
set(HEADERS
    mainwindow.h
    mainview.h
    locationservice.h
    drowsinessdetector.h
    alertdialog.h
)

# UI 파일 목록
set(UI_FILES
    mainwindow.ui
    alertdialog.ui
)

# 리소스 파일 목록
set(RESOURCE_FILES
    resources.qrc
)

# 실행 파일 추가
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
    qml/map.qml
    qml/RoadSegment.qml
)

# Qt 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Positioning
    Qt6::Location
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::TextToSpeech
)
