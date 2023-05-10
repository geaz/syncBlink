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
        ScriptLoad,
        ScriptError,
        NodeCommand,
        RawBytes
    };
}

#endif // MESSAGETYPES_H