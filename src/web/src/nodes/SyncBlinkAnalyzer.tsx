import { NodeProps } from 'react-flow-renderer';
import { faHeadphones } from '@fortawesome/free-solid-svg-icons';

import BaseNode from './StyledNode';
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

function SyncBlinkAnalyzer(props: NodeProps<SyncBlinkAnalyzerProps>) {
    const [,, changeAnalyzer] = useSyncBlinkNodesEffect(props.data.refreshMeshFunc);
    
    const rightBarContent = <IconButton icon={faHeadphones}
        active={props.data?.isActive}
        disabled={props.data?.isActive}
        tooltip="Activate Analyzer"
        onClick={() => changeAnalyzer(props.data?.id)} />;

    return <BaseNode
        hasTopHandle={false}
        hasBottomHandle={true}
        rightBarContent={ rightBarContent }
        >        
            <div className="node-label">{props.data?.label} </div>
            <div className="node-info">External Analyzer</div>
            <div className="node-id">ID {props.data?.id.toString(16).toUpperCase()}</div>
        </BaseNode>;
}

export default SyncBlinkAnalyzer;