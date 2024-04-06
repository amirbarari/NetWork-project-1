QT += widgets
QT += network

HEADERS       = button.h \
                calculator.h \
                malware.h
SOURCES       = button.cpp \
                calculator.cpp \
                main.cpp \
                malware.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/calculator
INSTALLS += target
