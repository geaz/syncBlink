import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/ui/IconButton';
import { faWifi, faServer, faPen, faAngleUp, faCrosshairs, faHeadphones } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';

export interface SyncBlinkNodeProps {
    id: number;
    label: string;
    ledCount: number;
    majorVersion: number;
    minorVersion: number;
    isActive: boolean;
    isAnalyzer: boolean;
    wifiAvailable: boolean;
    connectedToMeshWiFi: boolean;
    onPing: (nodeId: number) => void;
    onSetWifi: (nodeId: number, meshWifi: boolean) => void;
    onRename: (nodeId: number, newLabel: string) => void;
    onSetAnalyzer?: (analyzerId: number) => void;
}

function SyncBlinkNode(node: FlowNode<SyncBlinkNodeProps>) {
    return <StyledNode>
        <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        />
        {node.data?.wifiAvailable && 
            <div className="node-buttons-left">
                <IconButton icon={faWifi}
                    tooltip="Connect to WiFi"
                    active={!node.data?.connectedToMeshWiFi}
                    disabled={!node.data?.connectedToMeshWiFi}
                    onClick={() => node.data?.onSetWifi(node.data?.id, !node.data?.connectedToMeshWiFi)} />
                <IconButton icon={faServer}
                    tooltip="Connect to Station WiFi"
                    active={node.data?.connectedToMeshWiFi}
                    disabled={node.data?.connectedToMeshWiFi}
                    onClick={() => node.data?.onSetWifi(node.data?.id, !node.data?.connectedToMeshWiFi)} />
            </div>
        }
        <div className="node-frame">
            <div className="node-label">{node.data?.label}</div>
            <div className="node-id">ID {node.data?.id.toString(16).toUpperCase()}</div>
            <div className="node-info">{node.data?.ledCount} LEDs - v{node.data?.majorVersion}.{node.data?.minorVersion}</div>
        </div>
        <div className="node-buttons">
            {node.data?.isAnalyzer && 
                <IconButton icon={faHeadphones}
                    active={node.data?.isActive}
                    disabled={node.data?.isActive}
                    tooltip="Activate Analyzer"
                    onClick={() => { if(node.data?.onSetAnalyzer) node.data?.onSetAnalyzer(node.data?.id); }} />
            }
            <IconButton icon={faPen} tooltip="Rename" onClick={() => node.data?.onRename(node.data?.id, node.data?.label)} />
            <IconButton icon={faCrosshairs} tooltip="Ping" onClick={() => node.data?.onPing(node.data?.id)} />
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkNode;