[![GPL](https://img.shields.io/github/license/syncBlink/syncBlink)](https://github.com/syncBlink/syncBlink/blob/main/LICENSE)   
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A0A01MQZP)

# syncBlink
**syncBlink** is a WiFi connected, programmable and music reactive light system. It supports custom *script*s to influence the light patterns produced by **syncBlink**. The station comes with a custom web application to write and control these scripts.

Furthermore the **syncBlink Station** starts a naive mesh including a custom tcp server. **syncBlink Nodes** are able to connect to this mesh and will create an additional access point and server for other nodes to connect to. This way nodes which are not in range of the **syncBlink Station** are also able to join the mesh as long as at least one node is in range. The nodes are able to receive *commands* created by the *station*.

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

The **syncBlink Station** exposes a web application for mobiles and desktop.
Just connect to the access point with the following password:

```
SyncBlinkSaysLetThereBeLight!
```

By visiting the web application on **http://192.168.1.1** with a mobile browser the station will serve a *setting* page where you are able to configure a WiFi network. The station will connect to this network during the start procedure.

![syncBlink - Mobile](https://raw.githubusercontent.com/syncBlink/syncBlink/main/img/web-mobile.png)

After the station is connected to a network the station IP will appear on the bottom right of the display. By visiting the station web application with a desktop browser the station serves an overview of the current **syncBlink Mesh**.

![syncBlink - Web](https://raw.githubusercontent.com/syncBlink/syncBlink/main/img/web-1.png)

The graph provides functions to rename nodes, change the running script, change the analyzer, ping nodes and to update the firmware of the nodes. Visit the *Editor* menu to change the content of the scripts on the station or to create new ones.

![syncBlink - Web](https://raw.githubusercontent.com/syncBlink/syncBlink/main/img/web-2.png)

**Info**
```
If you get random disconnections of your nodes after connecting your station to a WiFi network,  
it is most probably because of an instable connection to the router. The problem is, that the  
ESP8266 just has one WiFi chip which is used to connect to the router AND serves as the access  
point for the nodes. If the station looses the connection to the router, it can cause a disconnection  
of the access point clients.
```

## blinkScript

**blinkScript** is a custom script language for **syncBlink**. The station compiles scripts on the fly to an intermediate language which gets executed by the integrated virtual machine.  
The script language is pretty basic and comes with the following features:

- Functions
- Variables
- Conditionals (if/else - no else if)
- for/while loops
- Arrays
- Some build in functions and variables

**Warning:** At the moment it is not possible to define new variables within loops! This is a workaround to speed up loops by a huge amount!

### Available global variables

- **maxF** *The maximum frequency which is recognizable by syncBlink*
- **nLedC** *The LED count of the current node*
- **mLedC** *The LED count of the whole mesh*
- **pLedC** *The LED count of the current route position*
- **pNodeC** *The node count of the current route position*
- **lVol** *The last analyzed volume in percentage (0-100)*
- **lFreq** *The last analyzed frequency*
- **vol** *The current analyzed volume in percentage (0-100)*
- **freq** *The current analyzed frequency*

### Available global functions

- **println(message: string)** *Prints the message to the serial output*
- **setGroups(countPerGroup: number)** *Set the LEDs of the current node to groups of the provided size. For example, if the node has 16 LEDs and setGroups(4) is called, the node will handle the 16 LEDs like four LEDs, where always four LEDs will be grouped together as one.*
- **setLinearGroups(countPerGroup: number)** *Same as above, but the internal numbering of the LEDs will be sequential. Resulting in a different group pattern.*
- **clearGroups()** *Clears all defined groups.*
- **setDelay(analyzerCount: number)** *Delays the analyzer results by the given amount of updates.*
- **getLed(led: number)** *Gets the color (Hex RGB) of the given LED.*
- **setLeds(colors: Array<number>)** *Set LEDs by defining an array of Hex colors.*
- **setAllLeds(color: number)** *Sets all LEDs to the given color (Hex RGB).*
- **map(value: number, inMin: number, inMax: number, outMin: number, outMax: number)** *Maps the value within the range of inMin and inMax to the new range defined by outMin and outMax.*
- **xrgb(r: number, g: number, b: number)** *Converts the given RGB color to a hex number representation.*
- **xhsv(h: number, s: number, v: number)** *Converts the given HSV color to a hex number representation.*

### Minimal example script

Each script has to define two functions as given in the following example.

```
let update = fun(delta) {
    // This function gets executed on each analyzer result (loop)
    // 'delta' contains the milliseconds since the last call of the loop
}

let init = fun(){
    // this function gets executed ONCE at start of the script
}
```

## Mesh

![syncBlink - Mesh](https://raw.githubusercontent.com/syncBlink/syncBlink/main/img/mesh.png)

### Connection Handshake
After a node connects to another node or the station it will send the **MESH_CONNECTION** message to the tcp server of its parent.
The message contains the *ID* of the node, the amount of LEDs, the installed *Firmware Version* and the node label. After the parent node or station received the message it will add its own
*ID* as the parent id to the message and will send it to its parent until it reaches the station.

The station will now know about the new connection in the mesh, the new node id, its parent and the installed firmware version of the connected node.
This will trigger a **MESH_UPDATE** request to update the LED and node information on each connected node.
After all nodes were updated a **MESH_UPDATED** message is send back to the station which will then send the currently selected led script (**DISTRIBUTE_SCRIPT**) and waits for
**SCRIPT_DISTRIBUTED** messages from all nodes in the mesh.