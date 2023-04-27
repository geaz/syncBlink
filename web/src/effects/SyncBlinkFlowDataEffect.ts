import { Dispatch, SetStateAction } from 'react';
import { Elements } from 'react-flow-renderer';

import { SyncBlinkNodeProps } from '../nodes/SyncBlinkNode';
import { SyncBlinkStationProps } from '../nodes/SyncBlinkStation';
import { SyncBlinkAnalyzerProps } from '../nodes/SyncBlinkAnalyzer';
import { SyncBlinkRouterNodeProps } from '../nodes/SyncBlinkRouterNode';

export enum ModalType {
    Renamer,
    Flasher,
    ScriptChanger
}

export interface ModalInfo {
    type: ModalType;
    nodeId: number;
    text: string;
}

export default function useSyncBlinkFlowData(setModal: Dispatch<SetStateAction<ModalInfo | undefined>>) {
    return (reloadData: () => void) => { return loadMeshInfo(reloadData, setModal); };
}

async function loadMeshInfo(reloadData: () => void, setModal: Dispatch<SetStateAction<ModalInfo | undefined>>) : Promise<Elements> 
{    
    let meshInfo: Elements = [];
    let meshResponse = await fetch("/api/mesh/info");
    if(meshResponse.ok) {
        let meshJson = await meshResponse.json();   
        let scriptName = meshJson.script;
        meshInfo = createMeshNodeData(
            meshJson.nodes,
            meshJson.analyzer,
            meshJson.lightMode,
            scriptName,
            meshJson.connected,
            meshJson.ssid,
            setModal,
            reloadData,
            async (analyzerId: number) => { await fetch('/api/mesh/setAnalyzer?analyzerId=' + analyzerId); reloadData(); });
    }
    return meshInfo;
}

function createMeshNodeData(
    nodeData: any, 
    activeAnalyzer: number,
    lightMode: boolean,
    runningScript: string,
    connectedToWifi: boolean,
    ssid: string,
    setModal: Dispatch<SetStateAction<ModalInfo | undefined>>,
    onRefresh: () => void,
    onSetAnalyzer: (analyzerId: number) => void) 
{
    let wifiNode: any = undefined;
    let stationNode: any = undefined;
    for(let i = 0; i < nodeData.length; i++) {
        let node = nodeData[i];
        node.id = i;
        node.position = { x: 0, y: 0 };
        if(node.isStation) {
            let props = {} as SyncBlinkStationProps;
            props.id = node.nodeId;
            props.isActive = props.id === activeAnalyzer;
            props.isLightMode = lightMode;
            props.ledCount = node.ledCount;
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.runningScript = runningScript;
            props.onSetAnalyzer = onSetAnalyzer;
            props.onRefresh = onRefresh;
            props.onChangeScript = () => setModal({ type: ModalType.ScriptChanger, text: runningScript } as ModalInfo);
            props.onLightMode = async () => { await fetch('/api/mesh/setLightMode?lightMode=' + (lightMode ? "false" : "true")); onRefresh(); }

            node.data = props;
            node.type = 'syncBlinkStation';
            stationNode = node;
        }
        else if(node.isAnalyzer && !node.isNode) {
            let props = {} as SyncBlinkAnalyzerProps;
            props.id = node.nodeId;
            props.label = node.label;
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.isActive = props.id === activeAnalyzer;
            props.onSetAnalyzer = onSetAnalyzer;

            node.data = props;
            node.type = 'syncBlinkAnalyzer';
        }
        else {
            let props = {} as SyncBlinkNodeProps;
            props.id = node.nodeId;
            props.label = node.label ? node.label : "(No Label)";
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.ledCount = node.ledCount;
            props.wifiAvailable = connectedToWifi;
            props.connectedToMeshWiFi = node.connectedToMeshWifi;
            props.onRename = (n, l) => setModal({ type: ModalType.Renamer, nodeId: n, text: l });
            props.onPing = (nodeId: number) => fetch('/api/mesh/ping?nodeId=' + nodeId);
            props.onSetWifi = (nodeId: number, meshWifi: boolean) => fetch('/api/mesh/setNodeWifi?nodeId=' + nodeId + '&meshWifi=' + (meshWifi ? "true" : "false"));

            if(node.isAnalyzer)
            {
                props.isAnalyzer = true;
                props.isActive = props.id === activeAnalyzer;                
                props.onSetAnalyzer = onSetAnalyzer;
            }

            node.data = props;
            node.type = 'syncBlinkNode';
        }
    }

    if(connectedToWifi) {
        let props = {} as SyncBlinkRouterNodeProps;
        props.wifiName = ssid;

        wifiNode = {} as any;
        wifiNode.id = Date.now();
        wifiNode.isAnalyzer = false;
        wifiNode.isStation = false;
        wifiNode.isNode = false;
        wifiNode.isWifiNode = true;
        wifiNode.position = { x: 0, y: 0 };
        wifiNode.data = props;
        wifiNode.type = 'syncBlinkRouterNode';

        nodeData.push(wifiNode);
    }

    let edges = [];
    for(let i = 0; i < nodeData.length; i++) {
        let node = nodeData[i];
        if(node.isAnalyzer && !node.isStation && !node.isNode){
            edges.push({ 
                id: node.id + '-' + stationNode.id,
                type: 'step',
                arrowHeadType: 'arrowclosed',
                source: `${node.id}`, 
                target: node.connectedToMeshWifi ? `${stationNode.id}` : `${wifiNode.id}`,
                animated: activeAnalyzer === node.nodeId
            });
        }
        else if(node.isWifiNode) {
            edges.push({ 
                id: node.id + '-' + stationNode.id,
                type: 'step',
                source: `${node.id}`, 
                target: `${stationNode.id}`,
                animated: true
            });
        }
        else if(node.isNode && !node.connectedToMeshWifi) {
            edges.push({ 
                id: node.id + '-' + wifiNode.id,
                type: 'step',
                animated: true,
                source: `${node.id}`, 
                target: `${wifiNode.id}`
            });
        }
        else if(node.isStation) {
            for(let j = 0; j < nodeData.length; j++) {
                let otherNode = nodeData[j];
                if(otherNode.parentId === node.nodeId && otherNode.connectedToMeshWifi) {
                    edges.push({ 
                        id: node.id + '-' + otherNode.id,
                        type: 'step',
                        animated: true,
                        source: `${node.id}`, 
                        target: `${otherNode.id}`
                    });
                }
            }
        }
    }
    return nodeData.concat(edges);
}