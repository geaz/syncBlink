import styled from "styled-components";
import { Dispatch, SetStateAction, useEffect, useState } from "react";
import ReactFlow, { MiniMap, Controls, Background, BackgroundVariant, Elements } from 'react-flow-renderer';

import SyncBlinkNode, { SyncBlinkNodeProps } from '../../nodes/SyncBlinkNode';
import SyncBlinkStation, { SyncBlinkStationProps } from '../../nodes/SyncBlinkStation';
import SyncBlinkAnalyzer, { SyncBlinkAnalyzerProps } from '../../nodes/SyncBlinkAnalyzer';

import Loader from '../ui/Loader';
import Renamer, { RenamerProps } from './Renamer';
import Flasher, { FlasherProps } from './Flasher';
import useDagreLayout from '../../effects/DagreLayoutEffect';

const nodeTypes = {
    syncBlinkStation: SyncBlinkStation,
    syncBlinkNode: SyncBlinkNode,
    syncBlinkAnalyzer: SyncBlinkAnalyzer,
};

function Mesh() {
    const [showLoader, setShowLoader] = useState(false);
    const [loaderMessage, setLoaderMessage] = useState('Loading Mesh Information...');

    const [isInitialized, setInitialized] = useState(false);
    const [flowInstance, setFlowInstance] = useState<any>(null);

    const [flasherProps, setFlasherProps] = useState<FlasherProps>({} as FlasherProps);
    const [renamerProps, setRenamerProps] = useState<RenamerProps>({} as RenamerProps);    
    
    const [dagreIsLoading, isLoadingData, flowElements, reloadData] = 
        useDagreLayout((s) => loadMeshInfo(s, setRenamerProps, setFlasherProps));

    useEffect(() => {
        if(flowInstance != null && !isInitialized) {
            setTimeout(() => { flowInstance.fitView(); }, 50);
            setInitialized(true);
        }
    }, [flowInstance, flowElements, isInitialized]);
    
    return <StyledMesh 
        isLoading={showLoader || dagreIsLoading}
        isLoadingData={isLoadingData} 
        isModalOpen={renamerProps?.nodeId !== undefined || flasherProps?.nodeId !== undefined}
        >
        { (showLoader || dagreIsLoading) && <Loader message={loaderMessage} transparent={!isInitialized} /> }
        { renamerProps.nodeId !== undefined && 
            <Renamer nodeId={renamerProps.nodeId} 
                currentLabel={renamerProps.currentLabel}
                onClose={() => setRenamerProps({} as RenamerProps)}
                beforeSave={() => { setLoaderMessage('Renaming node. Please wait...'); setShowLoader(true); setRenamerProps({} as RenamerProps); }}
                afterSave={() => { reloadData(); setShowLoader(false); }} />
        }
        { flasherProps.nodeId !== undefined && 
            <Flasher nodeId={flasherProps.nodeId} 
                nodeLabel={flasherProps.nodeLabel}
                onClose={() => setFlasherProps({} as FlasherProps)}
                beforeFlash={() => { 
                    setLoaderMessage('Firmware uploading, please check the status on the station display...');
                    setShowLoader(true);
                    setFlasherProps({} as FlasherProps);
                }}
                afterFlash={() => { setShowLoader(false); }} />
        }
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

async function loadMeshInfo(
    reloadData: () => void,
    setRenamerProps: Dispatch<SetStateAction<RenamerProps>>,
    setFlasherProps: Dispatch<SetStateAction<FlasherProps>>) : Promise<Elements> 
{    
    let meshInfo: Elements = [];
    let meshResponse = await fetch("/api/mesh/info");
    let scriptResponse = await fetch("/api/scripts/getActive");
    if(meshResponse.ok && scriptResponse.ok) {
        let meshJson = await meshResponse.json();   
        let scriptName = (await scriptResponse.json()).name;
        meshInfo = createMeshNodeData(
            meshJson.nodes,
            meshJson.source,
            scriptName,
            setRenamerProps,
            setFlasherProps,
            async (analyzerId: number) => { await fetch('/api/mesh/setSource?analyzerId=' + analyzerId); reloadData(); });
    }
    return meshInfo;
}

function createMeshNodeData(
    nodeData: any, 
    activeAnalyzer: number, 
    runningScript: string, 
    setRenamerProps: Dispatch<SetStateAction<RenamerProps>>,
    setFlasherProps: Dispatch<SetStateAction<FlasherProps>>,
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
            props.onSetAnalyzer = onSetAnalyzer;
            props.onFlash = () => setFlasherProps({nodeId: 0} as FlasherProps);

            node.data = props;
            node.type = 'syncBlinkStation';
            stationNode = node;
        }
        else if(node.isAnalyzer) {
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
            props.onFlash = (n) => setFlasherProps({nodeId: n, nodeLabel: props.label} as FlasherProps);
            props.onRename = (n, l) => setRenamerProps({nodeId: n, currentLabel: l} as RenamerProps);
            props.onPing = (nodeId: number) => fetch('/api/mesh/ping?nodeId=' + nodeId);

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

const StyledMesh = styled.div<{isLoadingData: boolean, isLoading : boolean, isModalOpen: boolean}>`
    width: 100vw;
    height: 100vh;

    @keyframes showAnim {
        from { opacity: 0; }
        to { opacity: 1; }
    }

    .react-flow {
        animation-name: showAnim;
        animation-duration: 1s;
    }

    .react-flow__renderer,
    .react-flow__minimap {        
        animation-name: showAnim;
        animation-duration: 1s;

        ${ p => p.isLoading ? 'opacity:0;' : '' }
        ${ p => p.isLoadingData ? 'display:none;' : '' }

        ${ p => p.isModalOpen && 
            'opacity: 0;transition: 200ms linear;'
        }
        ${ p => !p.isModalOpen && !p.isLoading && 
            'opacity: 1;transition: 300ms linear;'
        }
    }

    .react-flow__pane{
        cursor: grab;
    }
`;

export default Mesh;
