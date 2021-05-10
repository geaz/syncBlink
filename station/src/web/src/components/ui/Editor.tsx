import "codemirror/lib/codemirror.css";
import "codemirror/mode/javascript/javascript";

import CodeMirror from "codemirror";
import styled from "styled-components";
import { useEffect } from "react";

function Editor() {
    useEffect(() => {
        let codeEditorArea = document.getElementById("code") as HTMLTextAreaElement;
        if(codeEditorArea !== null) {
            let codeEditor = CodeMirror.fromTextArea(codeEditorArea, {
                lineNumbers: true,
                lineWrapping: true,
                mode: "javascript",
                indentUnit: 4
            });
            codeEditor.setSize("100%", "100%");
            //codeEditor.setValue(this.value);
            //codeEditor.on("change", () => this.$emit("input", this.codeEditor.getValue()));
        } 
        else {
            console.error("Code Editor TextArea not found!");
        }
    }, []);

    return (
        <StyledEditor>
            <textarea id="code"></textarea>
        </StyledEditor>
    );
}

const StyledEditor = styled.div`
    width: 100%;
    height: 100%;

    .CodeMirror-wrap pre {
        word-break: break-word;
    }
`;

export default Editor;
