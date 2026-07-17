
#include "maya_interpreter.hpp"
#include "maya_nanon.hpp"

#include <maya/MArgParser.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MMessage.h>
#include <maya/MObject.h>
#include <maya/MQtUtil.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>

#include <iostream>

#define    kReloadFlag            "-rl"
#define    kReloadFlagLong        "-reload"


MCallbackId outputCallbackId;

// We store a pointer to the workspace control in a static QPointer so that we
// can close (and thus delete) it if the plugin is unloaded. The QPointer will
// automatically set itself to zero if the workspace control is closed on its own.
// (See the devkit plugin "workspaceControlCmd.cpp")
QPointer<QWidget> NanonCmd::workspaceControl;
QPointer<nanon::NanonWindow> NanonCmd::nanon;
const MString NanonCmd::commandName("nanon");


void NanonCmd::cleanup()
{
    if (!workspaceControl.isNull()) {
        delete nanon;
        MString closeCommand("workspaceControl -e -close nanonWorkspaceControl");
        MGlobal::executeCommand(closeCommand);
    }

    MMessage::removeCallback(outputCallbackId);
}


bool NanonCmd::hasSyntax()
{
    return true;
}

MSyntax NanonCmd::newSyntax()
{
    MSyntax syntax;

    syntax.addFlag( kReloadFlag, kReloadFlagLong );

    return syntax;
}


void NanonCmd::outputCallback(const MString &message, MCommandMessage::MessageType messageType)
{
    QString text = QString(message.asChar());
    nanon->appendOutput(text);
}



MStatus NanonCmd::doIt(const MArgList& args)
{
    if(!workspaceControl.isNull()) {
        // Call -restore on existing workspace control to make it visible from whatever previous state it was in
        MString restoreCommand("workspaceControl -e -restore nanonWorkspaceControl");
        MGlobal::executeCommand(restoreCommand);
    } else {

        MArgParser argParser( syntax(), args );
        bool doReload = argParser.isFlagSet( kReloadFlag );

        // If we're reloading a workspace, the workspace control should already be created
        if (!doReload) {
            MGlobal::executeCommand("workspaceControl -label \"559 Nanon\" -retain false -deleteLater false -loadImmediately true -floating true -initialWidth 400 -initialHeight 200 -requiredPlugin \"NanonCmd\" nanonWorkspaceControl");
        }

        // Create nanon UI
        workspaceControl = MQtUtil::getCurrentParent();
        nanon = new nanon::NanonWindow();
        NanonMayaInterpreter *interpreter = new NanonMayaInterpreter();
        nanon->setInterpreter(interpreter);

        // Add UI as a child of the workspace control
        MQtUtil::addWidgetToMayaLayout(nanon, workspaceControl);

        // Set the -uiScript (used to rebuild UI when reloading workspace) after creation so that it doesn't get executed
        if (!doReload) {
            MString uiScriptCommand("workspaceControl -e -uiScript \"workspaceControlWindow -reload\" nanonWorkspaceControl");
            MGlobal::executeCommand(uiScriptCommand);
        }

        // Callback for appending any maya script editor content to the nanon window
        outputCallbackId = MCommandMessage::addCommandOutputCallback((MCommandMessage::MMessageFunction) outputCallback);
    }

    return MS::kSuccess;
}


// ==========================================================================
//
//            Plugin load/unload
//
// ==========================================================================

MStatus initializePlugin(MObject plugin)
{
    MStatus        st;
    MFnPlugin    pluginFn(plugin, "Autodesk, Inc.", "1.0", "Any", &st);

    if (!st) {
        MGlobal::displayError(
            MString("nanon - could not initialize plugin: ")
            + st.errorString()
        );
        return st;
    }

    // Register the command.
    st = pluginFn.registerCommand(NanonCmd::commandName, NanonCmd::creator, NanonCmd::newSyntax);

    if (!st) {
        MGlobal::displayError(
            MString("nanon - could not register '")
            + NanonCmd::commandName + "' command: "
            + st.errorString()
        );
        return st;
    }

    return st;
}


MStatus uninitializePlugin(MObject plugin)
{
    MStatus        st;
    MFnPlugin    pluginFn(plugin, "Autodesk, Inc.", "1.0", "Any", &st);

    if (!st) {
        MGlobal::displayError(
            MString("NanonCmd - could not uninitialize plugin: ")
            + st.errorString()
        );
        return st;
    }

    // Make sure that there is no UI left hanging around.
    NanonCmd::cleanup();

    // Deregister the command.
    st = pluginFn.deregisterCommand(NanonCmd::commandName);

    if (!st) {
        MGlobal::displayError(
            MString("NanonCmd - could not deregister '")
            + NanonCmd::commandName + "' command: "
            + st.errorString()
        );
        return st;
    }

    return st;
}
