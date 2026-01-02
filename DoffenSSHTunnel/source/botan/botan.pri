# Botan 3 (amalgamation)

CONFIG += exceptions

# Botan 3 requires at least C++20
CONFIG += c++20

win32 {
    HEADERS += botan/win/botan_all.h
    SOURCES += botan/win/botan_all.cpp
}

macx {
    HEADERS += botan/mac/botan_all.h
    SOURCES += botan/mac/botan_all.cpp
}

# dl may be needed on some Unix builds, depending on enabled features
unix:!macx {
    HEADERS += botan/linux/botan_all.h
    SOURCES += botan/linux/botan_all.cpp
    LIBS += -ldl
}

HEADERS += botan/botanwrapper.h
SOURCES += botan/botanwrapper.cpp
