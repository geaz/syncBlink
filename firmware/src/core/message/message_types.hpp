#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H

namespace SyncBlink
{
    enum MessageType
    {
        Base,
        AnalyzerUpdate,
        MeshConnection,
        MeshUpdate,
        ScriptChange,
        NodeCommand
    };
}

#endif // MESSAGETYPES_H