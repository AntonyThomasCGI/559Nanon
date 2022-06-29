#ifndef REDACT_MAYA_INCLUDED
#define REDACT_MAYA_INCLUDED

#include "nanon.hpp"

#include <QtCore/QPointer>
#include <maya/MPxCommand.h>


class NanonCmd : public MPxCommand
{
public:
	static void		cleanup();
	static void*	creator()		{ return new NanonCmd(); }

	MStatus			doIt(const MArgList& args);

	static QPointer<NanonEditor>	editor;
	static const MString			commandName;
};

#endif
