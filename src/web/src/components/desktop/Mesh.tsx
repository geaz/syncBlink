import styled from "styled-components";
import { useEffect, useState } from "react";
import ReactFlow, { MiniMap, Controls, Background, BackgroundVariant, useReactFlow } from 'react-flow-renderer';

import SyncBlinkNode from '../../nodes/SyncBlinkNode';
import SyncBlinkStation from '../../nodes/SyncBlinkStation';
import SyncBlinkAnalyzer from '../../nodes/SyncBlinkAnalyzer';
import SyncBlinkRouterNode from "../../nodes/SyncBlinkRouterNode";

import useDagreLayout from '../../effects/DagreLayoutEffect';
import useSyncBlinkFlowData from '../../effects/SyncBlinkFlowDataEffect';
import { Id, toast } from "react-toastify";

const nodeTypes = {
    syncBlinkStation: SyncBlinkStation,
    syncBlinkNode: SyncBlinkNode,
    syncBlinkAnalyzer: SyncBlinkAnalyzer,
    syncBlinkRouterNode: SyncBlinkRouterNode,
};

function Mesh() {
    const { fitView } = useReactFlow();
    const loadSyncBlinkFlowData = useSyncBlinkFlowData();
    const [isLoading,, nodes, edges ] = useDagreLayout(loadSyncBlinkFlowData);
    
    const [toastId, setToastId] = useState<Id>();

    useEffect(() => {
        if(isLoading) {
            setToastId(toast.loading("ℹ️ Loading mesh info..."));
        } else {
            toast.dismiss(toastId);
            fitView();
        }
    }, [isLoading]);
    
    return <StyledMesh isLoading={isLoading}>
        <ReactFlow 
            nodes={nodes}
            edges={edges}
            nodeTypes={nodeTypes}
            nodesDraggable={false}
            nodesConnectable={false}>
            <MiniMap />
            <Controls showInteractive={false} />
            <Background variant={BackgroundVariant.Dots} gap={15} />
        </ReactFlow>
    </StyledMesh>;
}

const StyledMesh = styled.div<{isLoading : boolean}>`
    width: 100%;
    height: 100%;

    .react-flow__renderer,
    .react-flow__minimap,
    .react-flow__controls {
        ${ p => (p.isLoading) && 
            'opacity: 0;transition: 100ms linear;'
        }

        ${ p => !p.isLoading && 
            'opacity: 1;transition: 400ms linear;'
        }
    }

    .react-flow__pane{
        cursor: grab;
    }
`;

export default Mesh;
