import styled from "styled-components";
import { Dispatch, SetStateAction, useEffect, useState } from "react";
import ReactFlow, { MiniMap, Controls, Background, BackgroundVariant, Elements } from 'react-flow-renderer';

import SyncBlinkNode, { SyncBlinkNodeProps } from '../nodes/SyncBlinkNode';
import SyncBlinkStation, { SyncBlinkStationProps } from '../nodes/SyncBlinkStation';
import SyncBlinkAnalyzer, { SyncBlinkAnalyzerProps } from '../nodes/SyncBlinkAnalyzer';

import Loader from './Loader';
import useDagreLayout from '../effects/DagreLayoutEffect';

const nodeTypes = {
    syncBlinkStation: SyncBlinkStation,
    syncBlinkNode: SyncBlinkNode,
    syncBlinkAnalyzer: SyncBlinkAnalyzer,
};

function Mesh() {
    const [isLoading, isLoadingData, flowElements] = useDagreLayout(loadMeshInfo);
    const [wasInitialized, setInitialized] = useState(false);
    const [flowInstance, setFlowInstance] = useState<any>(null);

    useEffect(() => { 
        if(flowInstance != null && !wasInitialized) {
            setTimeout(() => { flowInstance.fitView(); flowInstance.zoomOut(); }, 10);
            setInitialized(true);
        }
    }, [flowInstance, flowElements, wasInitialized]);
    
    return <StyledMesh isLoading={isLoading} isLoadingData={isLoadingData}>
        { isLoading && <Loader message="Loading Mesh Information ..." /> }
        { flowElements.length > 0 &&
                <ReactFlow 
                    elements={flowElements} 
                    nodeTypes={nodeTypes}
                    nodesDraggable={false}
                    nodesConnectable={false}
                    onLoad={(instance) => setFlowInstance(instance)}>
                    <MiniMap />
                    <Controls showInteractive={false} />
                    <Background variant={BackgroundVariant.Dots} gap={15} />
                </ReactFlow>
        }
    </StyledMesh>;
}

async function loadMeshInfo(setReload: Dispatch<SetStateAction<boolean>>) : Promise<Elements> 
{    
    let meshInfo: Elements = [];
    let response = await fetch("/api/mesh/info", {
        method: "GET"
    });
    if(response.ok) {        
        let responseJson = await response.json();
        meshInfo = createMeshNodeData(
            responseJson.nodes,
            responseJson.source,
            (sourceId: number) => setSource(sourceId, setReload));
    }
    return meshInfo;
}

function createMeshNodeData(nodeData: any, activeSource: number, onSetSource: (sourceId: number) => void) {
    let stationNode: any = undefined;
    for(let i = 0; i < nodeData.length; i++) {
        let node = nodeData[i];
        node.id = i;
        node.position = { x: 0, y: 0 };
        if(node.isStation) {
            let props = {} as SyncBlinkStationProps;
            props.id = node.clientId;
            props.isActive = props.id === activeSource;
            props.ledCount = node.ledCount;
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.onSetSource = onSetSource;

            node.data = props;
            node.type = 'syncBlinkStation';
            stationNode = node;
        }
        else if(node.isAnalyzer) {
            let props = {} as SyncBlinkAnalyzerProps;
            props.id = node.clientId;
            props.label = node.label;
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.isActive = props.id === activeSource;
            props.onSetSource = onSetSource;

            node.data = props;
            node.type = 'syncBlinkAnalyzer';
        }
        else {
            let props = {} as SyncBlinkNodeProps;
            props.id = node.clientId;
            props.label = node.label;
            props.majorVersion = node.majorVersion;
            props.minorVersion = node.minorVersion;
            props.ledCount = node.ledCount;
            props.onPing = (nodeId: number) => fetch('/api/mesh/ping?targetId=' + nodeId);

            node.data = props;
            node.type = 'syncBlinkNode';
        }
    }

    let edges = [];
    for(let i = 0; i < nodeData.length; i++) {
        let node = nodeData[i];
        
        if(node.isAnalyzer && !node.isStation){
            edges.push({ 
                id: node.id + '-' + stationNode.id,
                type: 'step',
                arrowHeadType: 'arrowclosed',
                source: `${node.id}`, 
                target: `${stationNode.id}`,
                animated: activeSource === node.clientId
            });
            continue;
        }

        for(let j = 0; j < nodeData.length; j++) {
            let otherNode = nodeData[j];
            if(otherNode.parentId === node.clientId) {
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

async function setSource(analyzerId: number, setReload: Dispatch<SetStateAction<boolean>>) {
    await fetch('/api/mesh/setSource?analyzerId=' + analyzerId);
    setReload(true);
}

const StyledMesh = styled.div<{isLoadingData: boolean, isLoading : boolean}>`
    width: 100vw;
    height: 100vh;

    .react-flow {
        ${ p => p.isLoading ? 'opacity:0;' : '' }
        ${ p => p.isLoadingData ? 'display:none;' : '' }
    }
`;

export default Mesh;
