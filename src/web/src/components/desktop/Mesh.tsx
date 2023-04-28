import styled from "styled-components";
import { useEffect, useState } from "react";
import ReactFlow, { MiniMap, Controls, Background, BackgroundVariant } from 'react-flow-renderer';

import SyncBlinkNode from '../../nodes/SyncBlinkNode';
import SyncBlinkStation from '../../nodes/SyncBlinkStation';
import SyncBlinkAnalyzer from '../../nodes/SyncBlinkAnalyzer';
import SyncBlinkRouterNode from "../../nodes/SyncBlinkRouterNode";

import Loader from '../ui/Loader';
import Renamer from './modals/Renamer';
import ScriptChanger from './modals/ScriptChanger';
import useDagreLayout from '../../effects/DagreLayoutEffect';
import useSyncBlinkFlowData, { ModalInfo, ModalType } from '../../effects/SyncBlinkFlowDataEffect';

const nodeTypes = {
    syncBlinkStation: SyncBlinkStation,
    syncBlinkNode: SyncBlinkNode,
    syncBlinkAnalyzer: SyncBlinkAnalyzer,
    syncBlinkRouterNode: SyncBlinkRouterNode,
};

function Mesh() {
    const [showLoader, setShowLoader] = useState(false);
    const [loaderMessage, setLoaderMessage] = useState('Loading Mesh Information...');

    const [isInitialized, setInitialized] = useState(false);
    const [flowInstance, setFlowInstance] = useState<any>(null);

    const [modalInfo, setModal] = useState<ModalInfo>();
    const loadMeshInfo = useSyncBlinkFlowData(setModal);
    const [dagreIsLoading, isLoadingData, flowElements, reloadData] = useDagreLayout((s) => loadMeshInfo(s));

    useEffect(() => {
        if(flowInstance != null && !isInitialized) {
            setTimeout(() => { flowInstance.fitView(); }, 250);
            setInitialized(true);
        }
    }, [flowInstance, flowElements, isInitialized]);
    
    return <StyledMesh 
        isLoading={showLoader || dagreIsLoading}
        isLoadingData={isLoadingData} 
        isModalOpen={modalInfo !== undefined}
        >
        { (showLoader || dagreIsLoading) && <Loader message={loaderMessage} transparent={isInitialized} /> }
        { modalInfo?.type === ModalType.Renamer && 
            <Renamer nodeId={modalInfo.nodeId} 
                currentLabel={modalInfo.text}
                setShowLoader={setShowLoader}
                setLoaderMessage={setLoaderMessage}
                setModal={setModal}
                afterSave={() => reloadData()} />
        }
        { modalInfo?.type === ModalType.ScriptChanger && 
            <ScriptChanger currentScript={modalInfo.text}
                setShowLoader={setShowLoader}
                setLoaderMessage={setLoaderMessage}
                setModal={setModal}
                afterSave={() => reloadData()}/>
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

const StyledMesh = styled.div<{isLoadingData: boolean, isLoading : boolean, isModalOpen: boolean}>`
    width: 100%;
    height: 100%;

    .react-flow__renderer,
    .react-flow__minimap,
    .react-flow__controls {
        ${ p => p.isLoadingData ? 'display:none;' : '' }

        ${ p => (p.isLoading || p.isModalOpen) && 
            'opacity: 0;transition: 100ms linear;'
        }

        ${ p => !p.isModalOpen && !p.isLoading && 
            'opacity: 1;transition: 400ms linear;'
        }
    }

    .react-flow__pane{
        cursor: grab;
    }
`;

export default Mesh;
