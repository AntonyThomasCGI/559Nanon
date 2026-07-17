#pragma once

#include "nanon/window.hpp"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <maya/MCommandMessage.h>
#include <maya/MPxCommand.h>


/**
 * Defines the ``nanon`` maya command.
 *
 * On launch of the UI this command will:
 *   - Register the maya script output into the nanon output window
 *   - Places the nanon main window into a workspaceControl to integrate with maya workspaces
 */
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
    static QPointer<nanon::NanonWindow> nanon;
};
