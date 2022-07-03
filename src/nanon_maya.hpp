#ifndef NANON_MAYA_INCLUDED
#define NANON_MAYA_INCLUDED

#include "nanon.hpp"

#include <QtCore/QPointer>
#include <maya/MPxCommand.h>
#include <maya/MCommandMessage.h>


class NanonCmd : public MPxCommand
{
public:
    static void cleanup();
    static void* creator() { return new NanonCmd(); }

    static void outputCallback(const MString &message, MCommandMessage::MessageType messageType);

    MStatus doIt(const MArgList& args);

    static QPointer<NanonWindow> nanon;
    static const MString commandName;
};

#endif
