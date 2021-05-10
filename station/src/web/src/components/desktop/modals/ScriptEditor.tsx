import { useState } from 'react';
import styled from 'styled-components';
import Editor from '../../ui/Editor';

function ScriptEditor() {
    return (
        <StyledScriptEditor>
            <div className="side">
                aoikjshdkjasdlkj
            </div>
            <div className="main">
                <Editor></Editor>
            </div>
        </StyledScriptEditor>
    );
}

const StyledScriptEditor = styled.div`
    @keyframes slide {
        from { right: -100%; opacity: 0; }
        to { right:0%; opacity: 1; }
    }

    @keyframes fadeIn {
        from { opacity: 0; }
        to { opacity: 1; }
    }

    right:0;
    width: 100%;
    height: 100%;
    z-index: 99;
    position: absolute;

    display: grid;
    grid-template-columns: 275px 1fr;
    grid-template-areas: "side main";
    animation: fadeIn 1s;

    .side {
        min-width: 300px;
        grid-area: side;
        padding: 125px 50px 20px 25px;
    }

    .main {
        margin: 20px 0;
        grid-area: main;
        background: white;
        border-left: 1px solid ${ p => p.theme.borderColor };
        box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
    }
`;

export default ScriptEditor;
