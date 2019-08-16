TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lgf_complete -lJerasure -L/opt/gurobi/81/linux64/lib/ -lgurobi_c++ -lgurobi81
INCLUDEPATH += /opt/gurobi/81/linux64/include
INCLUDEPATH += /usr/local/include/jerasure/

SOURCES += main.cpp \
    utils/solution.cpp \
    formulation/binary.cpp \
    formulation/combination.cpp \
    utils/galois_stuff.cpp \
    utils/gurobi_stuff.cpp \
    formulation/formulation.cpp \
    formulation/exclusive.cpp \
    second_stage/secondstage.cpp \
    second_stage/descent.cpp \
    second_stage/dynamic.cpp \
    second_stage/ip.cpp \
    utils/parameters.cpp \
    formulation/fullexclusive.cpp

HEADERS += \
    utils/solution.h \
    formulation/binary.h \
    formulation/combination.h \
    utils/galois_stuff.h \
    utils/gurobi_stuff.h \
    formulation/formulation.h \
    formulation/exclusive.h \
    second_stage/secondstage.h \
    second_stage/descent.h \
    second_stage/dynamic.h \
    second_stage/ip.h \
    utils/parameters.h \
    utils/cxxopts.hpp \
    formulation/fullexclusive.h
