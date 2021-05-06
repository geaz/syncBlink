import styled from "styled-components";
import { Node as FlowNode, Handle, Position } from 'react-flow-renderer';

import IconButton from '../components/IconButton';
import { faHeadphones } from '@fortawesome/free-solid-svg-icons';

export interface SyncBlinkAnalyzerProps {
    id: number;
    label: string;
    majorVersion: number;
    minorVersion: number;
    isActive: boolean;
    onSetSource: (sourceId: number) => void;
}

function SyncBlinkAnalyzer(node: FlowNode<SyncBlinkAnalyzerProps>) {
    return <StyledSyncBlinkAnalyzer>
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
                onClick={() => node.data?.onSetSource(node.data?.id)} />
        </div>
        <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        />
    </StyledSyncBlinkAnalyzer>;
}

const StyledSyncBlinkAnalyzer = styled.div`
    display:relative;

    .node-frame {
        text-align: center;
        padding: 10px 25px;
        border-radius: 5px;
        background: white;
        box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
    }

    .node-buttons {
        position:absolute;
        right:-30px;
        bottom:0;

        padding: 5px;
        display: flex;
        flex-direction: column;
    }

    .active {
        color: red;
    }

    .node-id {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.5em;
    }

    .node-info {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.5em;
        line-height:0.7rem;
        font-style: italic;
    }

    button {
        font-size: 0.6rem;
    }
`;

export default SyncBlinkAnalyzer;