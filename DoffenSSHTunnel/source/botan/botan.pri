# Botan 3 (amalgamation)
# Expected files:
#   botan/botan_all.h
#   botan/botan_all.cpp

HEADERS += botan/botan_all.h
SOURCES += botan/botan_all.cpp

HEADERS += botan/botanwrapper.h
SOURCES += botan/botanwrapper.cpp

CONFIG += exceptions

# Botan 3 requires at least C++20
CONFIG += c++20

# Some older Qt/qmake+MSVC kits do not propagate c++20 correctly
win32-msvc* {
    QMAKE_CXXFLAGS += /std:c++20
}

# Platform libraries occasionally needed by Botan on Windows
win32 {
    LIBS += -ladvapi32 -luser32
}

# dl may be needed on some Unix builds, depending on enabled features
unix:!macx {
    LIBS += -ldl
}
