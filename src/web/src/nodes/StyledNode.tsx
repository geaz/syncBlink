import styled from "styled-components";
import { Handle, Position } from "react-flow-renderer";

export interface BaseNodeProps {
    hasTopHandle: boolean;
    hasBottomHandle: boolean;
    leftBarContent?: any;
    rightBarContent?: any;
    children?: any;
}

function BaseNode(props: BaseNodeProps) {
    return <StyledNode>
        { props.hasTopHandle && <Handle
            type="target"
            position={Position.Top}
            style={{ background: '#555' }}
        /> }
        <div className="node-buttons node-buttons-left">
            { props.leftBarContent }
        </div>
        <div className="node-frame">
            { props.children }
        </div>
        <div className="node-buttons">
            { props.rightBarContent }
        </div>
        { props.hasBottomHandle && <Handle
            type="source"
            position={Position.Bottom}
            style={{ background: '#555' }}
        /> }
    </StyledNode>;
}

const StyledNode = styled.div`
    display: flex;
    cursor: default;

    .node-frame {
        text-align: center;
        padding: 10px 20px 15px 20px;
        margin: 0 3px;
        border-radius: 0.25em;
        background: white;
        box-shadow: rgba(0, 0, 0, 0.1) 0 0.1rem 0.3rem 0;
    }

    .node-buttons {
        display: flex;
        min-width: 22px;
        flex-direction: column;
        justify-content: flex-end;
    }

    .node-buttons-left {
        justify-content: flex-start;
    }

    .node-id {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.4em;
    }

    .node-label {
        margin: 0 13px;
    }

    .node-info {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.5em;
        line-height:0.7rem;
        font-style: italic;
        width:100%;
    }

    .node-script {
        display: flex;
        align-items: center;
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.5em;
        font-weight:bold;

        button {
            background: rgba(245, 245, 245, 1);
            margin-left: 5px;
            flex-grow: 1;
            font-size: 1em;
            color: ${ p => p.theme.textColorFaded };
            font-weight:bold;
            padding: 5px;

            &:hover {
                background: rgba(235, 235, 235, 1);
            }
        }

        .dropdown button, .dropdown ul, .dropdown li{
            border-radius: 2px;
        }
    }  

    .node-label-edit {
        .buttons {
            position: absolute;
            top: 10px;
            right: 28px;
        }

        button { 
            padding: 2px; 
            &:hover { background: none; }
        }
    }

    .active {
        color: red!important;
    }

    button {
        font-size: 0.6rem;
    }
`;

export default BaseNode;