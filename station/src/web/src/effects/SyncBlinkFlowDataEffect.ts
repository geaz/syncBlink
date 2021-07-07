import { Dispatch, SetStateAction } from 'react';
import { Elements } from 'react-flow-renderer';

import { SyncBlinkNodeProps } from '../nodes/SyncBlinkNode';
import { SyncBlinkStationProps } from '../nodes/SyncBlinkStation';
import { SyncBlinkAnalyzerProps } from '../nodes/SyncBlinkAnalyzer';

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
            scriptName,
            setModal,
            reloadData,
            async (analyzerId: number) => { await fetch('/api/mesh/setAnalyzer?analyzerId=' + analyzerId); reloadData(); });
    }
    return meshInfo;
}

function createMeshNodeData(
    nodeData: any, 
    activeAnalyzer: number, 
    runningScript: string, 
    setModal: Dispatch<SetStateAction<ModalInfo | undefined>>,
    onRefresh: () => void,
    onSetAnalyzer: (analyzerId: number) => void) 
{
    let stationNode: any = undefined;
    for(let i = 0; i < nodeData.length; i++) {
        let node = nodeData[i];
        node.id = i;
        node.position = { x: 0, y: 0 };
        if(node.isStation) {
            let props = {} as SyncBlinkStationProps;
            props.id = node.nodeId;
            props.isActive = props.id === activeAnalyzer;
            props.ledCount = node.ledCount;
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.runningScript = runningScript;
            props.onSetAnalyzer = onSetAnalyzer;
            props.onRefresh = onRefresh;
            props.onChangeScript = () => setModal({ type: ModalType.ScriptChanger, text: runningScript } as ModalInfo);
            props.onFlash = () => setModal({ type: ModalType.Flasher, nodeId: 0, text: 'All Nodes' });

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
            props.onFlash = (n) => setModal({ type: ModalType.Flasher, nodeId: n, text: props.label });
            props.onRename = (n, l) => setModal({ type: ModalType.Renamer, nodeId: n, text: l });
            props.onPing = (nodeId: number) => fetch('/api/mesh/ping?nodeId=' + nodeId);

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

    let edges = [];
    for(let i = 0; i < nodeData.length; i++) {
        let node = nodeData[i];
        
        if(node.isAnalyzer && !node.isStation && !node.isNode){
            edges.push({ 
                id: node.id + '-' + stationNode.id,
                type: 'step',
                arrowHeadType: 'arrowclosed',
                source: `${node.id}`, 
                target: `${stationNode.id}`,
                animated: activeAnalyzer === node.nodeId
            });
            continue;
        }

        for(let j = 0; j < nodeData.length; j++) {
            let otherNode = nodeData[j];
            if(otherNode.parentId === node.nodeId) {
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
    return nodeData.concat(edges);
}