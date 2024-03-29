import "codemirror/lib/codemirror.css";
import "codemirror/mode/javascript/javascript";

import CodeMirror from "codemirror";
import styled from "styled-components";
import { useEffect, useState } from "react";

interface EditorProps {
    content: string;
    disabled?: boolean;
    onChange?: (content: string) => void;
}

function Editor(props: EditorProps) {
    const [editor, setEditor] = useState<CodeMirror.EditorFromTextArea>();

    useEffect(() => {
        let codeEditorArea = document.getElementById("code") as HTMLTextAreaElement;
        if(codeEditorArea !== null) {
            let codeEditor = CodeMirror.fromTextArea(codeEditorArea, {
                lineNumbers: true,
                lineWrapping: true,
                mode: "javascript",
                indentUnit: 4
            });
            setEditor(codeEditor);
        } 
        else {
            throw new Error("Code Editor TextArea not found!");
        }
    }, [setEditor]);

    useEffect(() => {
        if(editor === undefined) return;
        editor.setSize("100%", "100%");
        editor.on("change", () => {
            if(props.onChange !== undefined && editor !== undefined)
                props.onChange(editor.getValue());
        });
    }, [editor]);

    useEffect(() => {
        editor?.setValue(props.content);
    }, [editor, props.content]);

    useEffect(() => {
        editor?.setOption("readOnly", props.disabled ? props.disabled : false);
    }, [props.disabled]);

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
