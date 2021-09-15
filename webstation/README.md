[![GPL](https://img.shields.io/github/license/syncBlink/syncBlink)](https://github.com/syncBlink/syncBlink/blob/main/LICENSE)  
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A0A01MQZP)



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