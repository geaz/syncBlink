import styled from "styled-components";

function ScriptEditor() {
    return (
        <StyledScriptEditor>
            <div className="editor-button">
                <a href="#">Script Editor</a>   
            </div>       
        </StyledScriptEditor>
    );
}

const StyledScriptEditor = styled.div`
    position:absolute;
    left:0;
    bottom:0;
    width:100vw;
    display:flex;
    justify-content:center;

    box-sizing: content-box;
    margin-left:100px;
    z-index:999;

    .editor-button {        
        background: white;
        border-radius: 5px;
        box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
        padding: ${ p => p.theme.contentPadding }px;
    }
`;

export default ScriptEditor;
