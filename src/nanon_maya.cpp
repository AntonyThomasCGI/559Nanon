
#include "nanon_maya.hpp"

#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>


QPointer<NanonEditor>    NanonCmd::editor;

const MString            NanonCmd::commandName("nanon");


//    Destroy the button window, if it still exists.
void NanonCmd::cleanup()
{
    if (!editor.isNull()) delete editor;
}



MStatus NanonCmd::doIt(const MArgList& /* args */)
{
    if (editor.isNull()) {
        editor = new NanonEditor();
        editor->show();
    }
    else {
        editor->showNormal();
        editor->raise();
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

