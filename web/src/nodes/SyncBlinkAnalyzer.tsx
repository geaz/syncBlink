import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/ui/IconButton';
import { faHeadphones } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';

export interface SyncBlinkAnalyzerProps {
    id: number;
    label: string;
    majorVersion: number;
    minorVersion: number;
    isActive: boolean;
    onSetAnalyzer: (analyzerId: number) => void;
}

function SyncBlinkAnalyzer(node: FlowNode<SyncBlinkAnalyzerProps>) {
    return <StyledNode>
        <div className="node-frame">
            <div className="node-label">{node.data?.label} </div>
            <div className="node-info">External Analyzer</div>
            <div className="node-id">ID {node.data?.id.toString(16).toUpperCase()}</div>
        </div>
        <div className="node-buttons">
            <IconButton icon={faHeadphones}
                active={node.data?.isActive}
                disabled={node.data?.isActive}
                tooltip="Activate Analyzer"
                onClick={() => node.data?.onSetAnalyzer(node.data?.id)} />
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkAnalyzer;