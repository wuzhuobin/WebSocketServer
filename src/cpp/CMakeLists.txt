find_package(Boost REQUIRED COMPONENTS date_time system)
add_executable(
    ${PROJECT_NAME}
    main.cpp
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${Boost_LIBRARIES}
    rt
    pthread
)