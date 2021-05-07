import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/ui/IconButton';
import { faPen, faAngleUp, faCrosshairs } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';

export interface SyncBlinkNodeProps {
    id: number;
    label: string;
    ledCount: number;
    majorVersion: number;
    minorVersion: number;
    onPing: (nodeId: number) => void;
    onFlash: (nodeId: number) => void;
    onRename: (nodeId: number, newLabel: string) => void;
}

function SyncBlinkNode(node: FlowNode<SyncBlinkNodeProps>) {
    return <StyledNode>
        <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        />
        <div className="node-frame">
            <div className="node-label">{node.data?.label}</div>
            <div className="node-id">ID {node.data?.id.toString(16).toUpperCase()}</div>
            <div className="node-info">LEDs {node.data?.ledCount} - v{node.data?.majorVersion}.{node.data?.minorVersion}</div>
        </div>
        <div className="node-buttons">
            <IconButton icon={faPen} tooltip="Rename" onClick={() => node.data?.onRename(node.data?.id, node.data?.label)} />
            <IconButton icon={faCrosshairs} tooltip="Ping" onClick={() => node.data?.onPing(node.data?.id)} />
            <IconButton icon={faAngleUp} tooltip="Update Firmware" onClick={() => node.data?.onFlash(node.data?.id)} />
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkNode;