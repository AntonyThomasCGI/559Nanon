
#include "nanon_maya.hpp"

#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MMessage.h>
#include <maya/MString.h>

QPointer<NanonWindow> NanonCmd::nanon;

const MString NanonCmd::commandName("nanon");

MCallbackId outputCallbackId;

//    Destroy the button window, if it still exists.
void NanonCmd::cleanup()
{
    if (!nanon.isNull()) delete nanon;

    MMessage::removeCallback(outputCallbackId);
}



void NanonCmd::outputCallback(const MString &message, MCommandMessage::MessageType messageType)
{
    QString text = QString(message.asChar());
    nanon->appendOutput(text);
}



MStatus NanonCmd::doIt(const MArgList& /* args */)
{
    if (nanon.isNull()) {
        nanon = new NanonWindow();
        nanon->show();
        outputCallbackId = MCommandMessage::addCommandOutputCallback((MCommandMessage::MMessageFunction) outputCallback);
    }
    else {
        nanon->showNormal();
        nanon->raise();
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

    //    Register the command.
    st = pluginFn.registerCommand(NanonCmd::commandName, NanonCmd::creator);

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

    //    Make sure that there is no UI left hanging around.
    NanonCmd::cleanup();

    //    Deregister the command.
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
