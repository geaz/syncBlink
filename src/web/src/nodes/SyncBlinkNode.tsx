import { useState } from 'react';
import {  Handle, NodeProps, Position } from 'react-flow-renderer';
import { faWifi, faServer, faPen, faCrosshairs, faHeadphones, faCheckCircle, faTimesCircle } from '@fortawesome/free-solid-svg-icons';

import StyledNode from './StyledNode';
import IconButton from '../components/ui/IconButton';
import useSyncBlinkNodesEffect from '../effects/SyncBlinkNodesEffect';

export interface SyncBlinkNodeProps {
    id: number;
    parentId: number;
    label: string;
    ledCount: number;
    majorVersion: number;
    minorVersion: number;
    isActive: boolean;
    isAnalyzer: boolean;
    wifiAvailable: boolean;
    connectedToMeshWifi: boolean;
    refreshMeshFunc: () => void;
}

function SyncBlinkNode(props: NodeProps<SyncBlinkNodeProps>) {
    const [renameNode, changeWifi, changeAnalyzer, pingNode] = useSyncBlinkNodesEffect(props.data.refreshMeshFunc);
    const [nodeLabel, setNodeLabel] = useState<string>(props.data.label);
    const [originalNodeLabel, setOriginalNodeLabel] = useState<string>(props.data.label);
    const [showRename, setShowRename] = useState<boolean>(false);

    let renameNodeWrap = async () => {
        setShowRename(false);        
        if(nodeLabel === originalNodeLabel) return;
        if(await renameNode(props.data.id, nodeLabel))
            setOriginalNodeLabel(nodeLabel);
    };

    return <StyledNode>
        <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        />
        {props.data.wifiAvailable && 
            <div className="node-buttons-left">
                <IconButton icon={faWifi}
                    tooltip="Connect to WiFi"
                    active={!props.data.connectedToMeshWifi}
                    disabled={!props.data.connectedToMeshWifi}
                    onClick={ () => changeWifi(props.data.id, false) } />
                <IconButton icon={faServer}
                    tooltip="Connect to Mesh"
                    active={props.data.connectedToMeshWifi}
                    disabled={props.data.connectedToMeshWifi}
                    onClick={() => changeWifi(props.data.id, true) } />
            </div>
        }
        <div className="node-frame">
            <div className="node-label-edit">
                { !showRename &&<div className="node-label"><span>{nodeLabel}</span></div> }
                { showRename && <>
                    <input className="node-input" type="text" size={nodeLabel.length} value={nodeLabel} onChange={(e) => setNodeLabel(e.target.value)} autoFocus />
                    <div className="buttons">
                        <IconButton icon={faCheckCircle} tooltip="Save name" onClick={ () => renameNodeWrap() } />
                        <IconButton icon={faTimesCircle} tooltip="Abort renaming" onClick={ () => { setShowRename(!showRename); setNodeLabel(originalNodeLabel); } } />
                    </div>
                    </>
                }                
            </div>            
            <div className="node-id">ID {props.data.id.toString(16).toUpperCase()}</div>
            <div className="node-info">{props.data.ledCount} LEDs - v{props.data.majorVersion}.{props.data.minorVersion}</div>
        </div>
        <div className="node-buttons">
            {props.data.isAnalyzer && 
                <IconButton icon={faHeadphones}
                    active={props.data.isActive}
                    disabled={props.data.isActive}
                    tooltip="Activate Analyzer"
                    onClick={() => { changeAnalyzer(props.data.id) }} />
            }
            <IconButton icon={faPen} tooltip="Rename" onClick={ () => setShowRename(!showRename) } />
            <IconButton icon={faCrosshairs} tooltip="Ping" onClick={ () => pingNode(props.data.id) } />  
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledNode>;
}

export default SyncBlinkNode;