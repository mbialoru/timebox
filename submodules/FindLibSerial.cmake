find_package(PkgConfig)
pkg_check_modules(PC_libserial QUIET libserial)

find_path(LibSerial_INCLUDE_DIR
    NAMES SerialPortConstants.h SerialPort.h SerialStreamBuf.h SerialStream.h
    PATHS ${PC_libserial_INCLUDE_DIRS}
    PATH_SUFFIXES libserial
)
find_library(LibSerial_LIBRARY
  NAMES serial
  PATHS ${PC_libserial_LIBRARY_DIRS}
)

set(LibSerial_VERSION ${PC_libserial_VERSION})

mark_as_advanced(LibSerial_FOUND LibSerial_INCLUDE_DIR LibSerial_LIBRARY LibSerial_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibSerial
    REQUIRED_VARS LibSerial_INCLUDE_DIR LibSerial_LIBRARY
    VERSION_VAR LibSerial_VERSION
)

if(LibSerial_FOUND)
    set(LibSerial_INCLUDE_DIRS ${LibSerial_INCLUDE_DIR})
    set(LibSerial_LIBRARIES ${LibSerial_LIBRARY})
    set(LibSerial_DEFINITIONS ${PC_libserial_CFLAGS_OTHER})

    if(NOT TARGET LibSerial::serial)
        add_library(LibSerial::serial UNKNOWN IMPORTED)
        set_target_properties(LibSerial::serial PROPERTIES
            IMPORTED_LOCATION "${LibSerial_LIBRARY}"
            INTERFACE_COMPILE_OPTIONS "${PC_libserial_CFLAGS_OTHER}"
            INTERFACE_INCLUDE_DIRECTORIES "${LibSerial_INCLUDE_DIR}"
        )
    endif()
endif()