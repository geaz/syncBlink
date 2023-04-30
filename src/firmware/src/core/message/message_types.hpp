#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H

namespace SyncBlink
{
    enum MessageType
    {
        Base,
        AnalyzerChange,
        AnalyzerUpdate,
        MeshConnection,
        MeshUpdate,
        ScriptChange,
        ScriptError,
        NodeCommand
    };
}

#endif // MESSAGETYPES_H