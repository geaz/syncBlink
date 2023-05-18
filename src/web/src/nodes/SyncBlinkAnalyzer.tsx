import { Node as FlowNode, Handle, NodeProps, Position } from 'react-flow-renderer';
import { faHeadphones } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';
import IconButton from '../components/ui/IconButton';
import useSyncBlinkNodesEffect from '../effects/SyncBlinkNodesEffect';

export interface SyncBlinkAnalyzerProps {
    id: number;
    label: string;
    majorVersion: number;
    minorVersion: number;
    isActive: boolean;
    refreshMeshFunc: () => void;
}

export type SyncBlinkAnalyzerNode = FlowNode<SyncBlinkAnalyzerProps>;

function SyncBlinkAnalyzer(props: NodeProps<SyncBlinkAnalyzerProps>) {
    const [,, changeAnalyzer] = useSyncBlinkNodesEffect(props.data.refreshMeshFunc);
    
    return <StyledNode>
        <div className="node-frame">
            <div className="node-label">{props.data?.label} </div>
            <div className="node-info">External Analyzer</div>
            <div className="node-id">ID {props.data?.id.toString(16).toUpperCase()}</div>
        </div>
        <div className="node-buttons">
            <IconButton icon={faHeadphones}
                active={props.data?.isActive}
                disabled={props.data?.isActive}
                tooltip="Activate Analyzer"
                onClick={() => changeAnalyzer(props.data?.id)} />
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkAnalyzer;