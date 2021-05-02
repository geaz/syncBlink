[![GPL](https://img.shields.io/github/license/syncBlink/syncBlink)](https://github.com/syncBlink/syncBlink/blob/main/LICENSE)   
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A0A01MQZP)

# syncBlink

## The Mesh

![syncBlink - Mesh](https://raw.githubusercontent.com/syncBlink/syncBlink/main/img/mesh.png)

### Connection Handshake
After a node connected itself to another node or the station it will send the **MESH_CONNECTION** message to the tcp server of its parent.
The message contains the *ID* of the node, the amount of LEDs, the installed *Firmware Version* and the node label. After the parent node or station received the message it will add its own
*ID* as the parent id to the message and will send it to its parent until it reaches the station.

The station will now know about the new connection in the mesh, the new node id, its parent and the installed firmware version of the connected node.
This will trigger a **MESH_UPDATE** request to update the LED and node information on each connected node.
After all nodes were updated a **MESH_UPDATED** message is send back to the station which will then send the currently selected led script (**DISTRIBUTE_MOD**) and waits for
**MOD_DISTRIBUTED** messages from all nodes in the mesh.

The new connection of a node is therefore done.