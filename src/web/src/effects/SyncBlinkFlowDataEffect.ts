import { SyncBlinkNodeProps } from '../nodes/SyncBlinkNode';
import { SyncBlinkStationProps } from '../nodes/SyncBlinkStation';
import { SyncBlinkAnalyzerProps } from '../nodes/SyncBlinkAnalyzer';
import { SyncBlinkRouterNodeProps } from '../nodes/SyncBlinkRouterNode';
import { Edge, Node } from 'react-flow-renderer';

/**
 * This effect handels the loading of the mesh information. It calls the station
 * to get a json document including all information about the current mesh topography
 * and builds an array of objects (including all properties and functions to execute on the nodes) 
 * to represent this mesh.
 * 
 * @returns The effect returns a function to reload the mesh information on demand.
 * Parameter for the returned function is a function to execute, if the reload button on the
 * station node gets pressed.
 */
export default function useSyncBlinkFlowData() {
    return (refreshMeshFunc: () => void) => { return loadMeshInfo(refreshMeshFunc); };
}

async function loadMeshInfo(refreshMeshFunc: () => void) : Promise<[Array<Node>, Array<Edge>]> 
{    
    let nodes: Array<Node> = [];
    let edges: Array<Edge> = [];
    let meshResponse = await fetch("/api/mesh/info");
    if(meshResponse.ok) {
        let meshJson = await meshResponse.json();   
        let scriptName = meshJson.script;
        [nodes, edges] = createMeshNodeData(
            meshJson.nodes,
            meshJson.analyzer,
            meshJson.lightMode,
            scriptName,
            meshJson.connected,
            meshJson.ssid,
            refreshMeshFunc);
    }
    return [nodes, edges];
}

function createMeshNodeData(
    meshNodeData: any, 
    activeAnalyzer: number,
    lightMode: boolean,
    runningScript: string,
    connectedToWifi: boolean,
    ssid: string,
    refreshMeshFunc: () => void) : [Array<Node>, Array<Edge>]
{
    let wifiNode: any = undefined;
    let stationNode: any = undefined;
    let flowNodes: Array<Node> = [];
    for(let i = 0; i < meshNodeData.length; i++) {
        let meshNode = meshNodeData[i];
        let flowNode = {id: meshNode.nodeId.toString(16), position: { x: 0, y: 0 } } as Node;

        if(meshNode.isStation) {
            let props = {} as SyncBlinkStationProps;
            props.id = meshNode.nodeId;
            props.isActive = props.id === activeAnalyzer;
            props.isLightMode = lightMode;
            props.ledCount = meshNode.ledCount;
            props.majorVersion = meshNode.majorVersion;
            props.minorVersion = meshNode.minorVersion;
            props.runningScript = runningScript;
            props.refreshMeshFunc = refreshMeshFunc;

            flowNode.data = props;
            flowNode.type = 'syncBlinkStation';
            stationNode = flowNode;
        }
        else if(meshNode.isAnalyzer && !meshNode.isNode) {
            let props = {} as SyncBlinkAnalyzerProps;
            props.id = meshNode.nodeId;
            props.label = meshNode.label;
            props.majorVersion = meshNode.majorVersion;
            props.minorVersion = meshNode.minorVersion;
            props.isActive = props.id === activeAnalyzer;
            props.refreshMeshFunc = refreshMeshFunc;

            flowNode.data = props;
            flowNode.type = 'syncBlinkAnalyzer';
        }
        else {
            let props = {} as SyncBlinkNodeProps;
            props.id = meshNode.nodeId;
            props.parentId = meshNode.parentId;
            props.label = meshNode.label ? meshNode.label : "(No Label)";
            props.majorVersion = meshNode.majorVersion;
            props.minorVersion = meshNode.minorVersion;
            props.ledCount = meshNode.ledCount;
            props.wifiAvailable = connectedToWifi;
            props.connectedToMeshWifi = meshNode.connectedToMeshWifi;
            props.refreshMeshFunc = refreshMeshFunc;

            if(meshNode.isAnalyzer)
            {
                props.isAnalyzer = true;
                props.isActive = props.id === activeAnalyzer;
            }

            flowNode.data = props;
            flowNode.type = 'syncBlinkNode';
        }        
        flowNodes.push(flowNode);
    }

    if(connectedToWifi) {
        let wifiProps = {} as SyncBlinkRouterNodeProps;
        wifiProps.wifiName = ssid;

        let flowNode = {id: Date.now().toString(16), position: { x: 0, y: 0 }} as Node;
        flowNode.data = wifiProps;
        flowNode.type = 'syncBlinkRouterNode';

        wifiNode = flowNode;
        flowNodes.push(flowNode);
    }

    let edges: Array<Edge> = [];
    for(let i = 0; i < flowNodes.length; i++) {
        let flowNode = flowNodes[i];
        if(flowNode.type === "syncBlinkAnalyzer"){
            edges.push({ 
                id: flowNode.id + '-' + stationNode.id,
                type: 'step',
                markerEnd: 'arrowclosed',
                source: flowNode.id, 
                target: wifiNode.id,
                animated: activeAnalyzer === flowNode.data.id
            });
        }
        else if(flowNode.type === "syncBlinkRouterNode") {
            edges.push({ 
                id: flowNode.id + '-' + stationNode.id,
                type: 'step',
                source: flowNode.id, 
                target: stationNode.id,
                animated: true
            });
        }
        else if(flowNode.type === "syncBlinkNode" && !flowNode.data.connectedToMeshWifi) {
            edges.push({ 
                id: flowNode.id + '-' + wifiNode.id,
                type: 'step',
                animated: true,
                source: flowNode.id, 
                target: wifiNode.id
            });
        }
        else if(flowNode.type === "syncBlinkNode" && flowNode.data.connectedToMeshWifi) {
            edges.push({ 
                id: flowNode.data.parentId + '-' + flowNode.id,
                type: 'step',
                animated: true,
                source: flowNode.data.parentId.toString(16),
                target: flowNode.id
            });
        }
    }
    return [flowNodes, edges];
}