import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/ui/IconButton';
import { faHeadphones, faAngleDoubleUp, faCode } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';

export interface SyncBlinkStationProps {
    id: number;
    isActive: boolean;
    ledCount: number;
    majorVersion: number;
    minorVersion: number;
    onFlash: () => void;
    onSetAnalyzer: (analyzerId: number) => void;
}

function SyncBlinkStation(node: FlowNode<SyncBlinkStationProps>) {
    return <StyledNode>
        <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        />
        <div className="node-frame">
            <div className="node-label">Station</div>
            <div className="node-info">LEDs {node.data?.ledCount} - v{node.data?.majorVersion}.{node.data?.minorVersion}</div>
        </div>
        <div className="node-buttons">
            <IconButton icon={faCode}
                tooltip="Open Editor"
                onClick={() => node.data?.onFlash()} />
            <IconButton icon={faAngleDoubleUp}
                tooltip="Update ALL nodes"
                onClick={() => node.data?.onFlash()} />            
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

export default SyncBlinkStation;