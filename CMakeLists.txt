cmake_minimum_required( VERSION 2.8.12 )

project( spreadsheet_prj )

find_package( Boost 1.58.0 COMPONENTS system iostreams filesystem regex unit_test_framework REQUIRED )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	find_package( Threads REQUIRED )
	if( THREADS_HAVE_PTHREAD_ARG )
		set( COMPILER_SPECIFIC_LIBS, "-lpthread" )
	endif()

	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options(-std=c++14 -Weverything -Wno-c++98-compat -g -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -g -Wall -Wno-deprecated-declarations)
	endif()
endif()

link_directories( "${CMAKE_SOURCE_DIR}/glean_files/lib" )
include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/glean_files/include" )
include_directories( "${CMAKE_SOURCE_DIR}/include" )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

set( HEADER_FOLDER "include" )

set( HEADER_FILES
	${HEADER_FOLDER}/big_num_t.h
	${HEADER_FOLDER}/evaluator.h
	${HEADER_FOLDER}/impl_cell_value.h
	${HEADER_FOLDER}/spreadsheet.h
	${HEADER_FOLDER}/sheetrock.h
	${HEADER_FOLDER}/table_item.h
)

set( SOURCE_FILES
	big_num_t.cpp
	impl_cell_value.cpp
	spreadsheet.cpp
	table_item.cpp
)

add_library( spreadsheet STATIC ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( spreadsheet char_range parse_json nodepp ${Boost_LIBRARIES} )

add_executable( spreadsheet_bin main.cpp )
target_link_libraries( spreadsheet_bin spreadsheet ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )

