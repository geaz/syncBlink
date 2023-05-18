import { Handle, NodeProps, Position } from 'react-flow-renderer';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faWifi } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';

export interface SyncBlinkRouterNodeProps {
    wifiName: string;
}

function SyncBlinkRouterNode(node: NodeProps<SyncBlinkRouterNodeProps>) {
    return <StyledNode>
        <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        />
        <div className="node-frame">
            <div className="node-label"><FontAwesomeIcon icon={faWifi}/> <span>WiFi</span></div>
            <div className="node-info">{node.data?.wifiName}</div>
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkRouterNode;