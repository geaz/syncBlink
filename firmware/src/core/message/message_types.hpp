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
    };
}

#endif // MESSAGETYPES_H