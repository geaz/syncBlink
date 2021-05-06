import styled from "styled-components";
import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/IconButton';
import { faPen, faUpload, faCrosshairs } from '@fortawesome/free-solid-svg-icons';

export interface SyncBlinkNodeProps {
    id: number;
    label: string;
    ledCount: number;
    majorVersion: number;
    minorVersion: number;
    onPing: (nodeId: number) => void;
    onFlash: (nodeId: number) => void;
    onRename: (nodeId: number) => void;
}

function SyncBlinkNode(node: FlowNode<SyncBlinkNodeProps>) {
    return <StyledSyncBlinkNode>
        <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        />
        <div className="node-frame">
            <div className="node-label">{node.data?.label} </div>
            <div className="node-id">ID {node.data?.id.toString(16).toUpperCase()}</div>
            <div className="node-infos">LEDs {node.data?.ledCount} / v{node.data?.majorVersion}.{node.data?.minorVersion}</div>
        </div>
        <div className="node-buttons">
            <IconButton icon={faPen} tooltip="Rename" onClick={() => node.data?.onRename(node.data?.id)} />
            <IconButton icon={faCrosshairs} tooltip="Ping" onClick={() => node.data?.onPing(node.data?.id)} />
            <IconButton icon={faUpload} tooltip="Update Firmware" onClick={() => node.data?.onFlash(node.data?.id)} />
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledSyncBlinkNode>;
}

const StyledSyncBlinkNode = styled.div`
    display:relative;

    .node-frame {
        text-align: center;
        padding: 10px 25px;
        border-radius: 5px;
        background: white;
        box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
    }

    .node-buttons {
        position:absolute;
        right:-30px;
        bottom:0;

        padding: 5px;
        display: flex;
        flex-direction: column;
    }

    .node-id {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.6em;
    }

    .node-infos {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.6em;
        line-height:0.7rem;
    }

    button {
        font-size: 0.6rem;
    }
`;

export default SyncBlinkNode;