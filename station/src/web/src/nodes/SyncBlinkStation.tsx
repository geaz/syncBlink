import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/ui/IconButton';
import { faHeadphones, faAngleDoubleUp, faCode, faSync, faListAlt } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';

export interface SyncBlinkStationProps {
    id: number;
    isActive: boolean;
    ledCount: number;
    majorVersion: number;
    minorVersion: number;
    runningScript: string;
    onRefresh: () => void;
    onOpenEditor: () => void;
    onChangeScript: () => void;
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
        <div className="node-buttons-left">
            <IconButton icon={faSync}
                tooltip="Refresh Mesh"
                onClick={() => node.data?.onRefresh()} />
            <IconButton icon={faAngleDoubleUp}
                tooltip="Update ALL nodes"
                onClick={() => node.data?.onFlash()} />
        </div>
        <div className="node-frame">
            <div className="node-label">Station</div>
            <div className="node-script">Running: {node.data?.runningScript}</div>
            <div className="node-info">{node.data?.ledCount} LEDs - v{node.data?.majorVersion}.{node.data?.minorVersion}</div>
        </div>
        <div className="node-buttons">        
            <IconButton icon={faHeadphones}
                active={node.data?.isActive}
                disabled={node.data?.isActive}
                tooltip="Activate Analyzer"
                onClick={() => node.data?.onSetAnalyzer(node.data?.id)} />
            <IconButton icon={faCode}
                tooltip="Open Editor"
                onClick={() => node.data?.onOpenEditor()} />            
            <IconButton icon={faListAlt}
                tooltip="Change Script"
                onClick={() => node.data?.onChangeScript()} />  
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkStation;