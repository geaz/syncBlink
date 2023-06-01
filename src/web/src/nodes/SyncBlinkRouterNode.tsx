import { NodeProps } from 'react-flow-renderer';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faWifi } from '@fortawesome/free-solid-svg-icons';

import BaseNode from './StyledNode';

export interface SyncBlinkRouterNodeProps {
    wifiName: string;
}

function SyncBlinkRouterNode(node: NodeProps<SyncBlinkRouterNodeProps>) {
    return <BaseNode
        hasTopHandle={true}
        hasBottomHandle={true}>
            <div className="node-label"><FontAwesomeIcon icon={faWifi}/> <span>WiFi</span></div>
            <div className="node-info">{node.data?.wifiName}</div>
    </BaseNode>;
}

export default SyncBlinkRouterNode;