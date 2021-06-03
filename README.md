[![GPL](https://img.shields.io/github/license/syncBlink/syncBlink)](https://github.com/syncBlink/syncBlink/blob/main/LICENSE)   
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A0A01MQZP)

# syncBlink
**syncBlink** is a WiFi connected, programmable and music reactive light system. It supports custom *Script*s to influence the light patterns produced by **syncBlink**. The station comes with a custom web application to write and control these scripts.

Furthermore the **syncBlink Station** starts a naive mesh including a custom tcp server. **syncBlink Nodes** are able to connect to this mesh and will create an additional access point and server for other nodes to connect to. This way nodes which are not in range of the **syncBlink Station** are also able to join the mesh as long as at least one node is in range. The nodes are able to receive *commands* created by the *Station*.

*Everything combined, it is possible to span a mesh of WiFi connected devices which are synchronously reacting to music.*

# Table of Contents
1. [syncBlink Station](#syncBlink-Station)
2. [syncBlink Node](#syncBlink-Node)
3. [Web Application](#web-application)
4. [blinkScript - syncBlink Script Language](#blinkScript)
3. [Mesh](#mesh)
3. [API](https://github.com/syncBlink/syncBlink/tree/main/shared)

## syncBlink Station
Please head over to the [station](https://github.com/syncBlink/syncBlink/tree/main/station) folder for more information.

## syncBlink Node
Please header over to the [node](https://github.com/syncBlink/syncBlink/tree/main/node) folder for more information.

## Web Application

## blinkScript

## Mesh

![syncBlink - Mesh](https://raw.githubusercontent.com/syncBlink/syncBlink/main/img/mesh.png)

### Connection Handshake
After a node connected itself to another node or the station it will send the **MESH_CONNECTION** message to the tcp server of its parent.
The message contains the *ID* of the node, the amount of LEDs, the installed *Firmware Version* and the node label. After the parent node or station received the message it will add its own
*ID* as the parent id to the message and will send it to its parent until it reaches the station.

The station will now know about the new connection in the mesh, the new node id, its parent and the installed firmware version of the connected node.
This will trigger a **MESH_UPDATE** request to update the LED and node information on each connected node.
After all nodes were updated a **MESH_UPDATED** message is send back to the station which will then send the currently selected led script (**DISTRIBUTE_SCRIPT**) and waits for
**SCRIPT_DISTRIBUTED** messages from all nodes in the mesh.

The new connection of a node is therefore done.