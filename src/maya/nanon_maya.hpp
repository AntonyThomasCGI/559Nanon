#pragma once

#include "nanon.hpp"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <maya/MCommandMessage.h>
#include <maya/MPxCommand.h>


class NanonCmd : public MPxCommand
{
public:
    static void cleanup();
    static void* creator() { return new NanonCmd(); }
    static MSyntax	newSyntax();

	virtual bool	hasSyntax();

    static void outputCallback(const MString &message, MCommandMessage::MessageType messageType);

    MStatus doIt(const MArgList& args);

    static QPointer<QWidget> workspaceControl;
    static const MString commandName;

private:
    static QPointer<NanonWindow> nanon;
};
