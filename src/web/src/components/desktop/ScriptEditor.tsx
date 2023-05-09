import styled from "styled-components";
import { useEffect, useState } from 'react';
import { faPlus, faTrashAlt, faSave, faDownload } from '@fortawesome/free-solid-svg-icons';

import Editor from '../ui/Editor';
import IconButton from "../ui/IconButton";
import Dropdown, { DropdownOption } from "../ui/Dropdown";
import useBlinkScriptEffect, { Script } from "../../effects/BlinkScriptEffect";

function ScriptEditor() {
    const [loadScriptList, compileScript, saveScript, deleteScript, getScript, addScript] = useBlinkScriptEffect();
    
    const [isLoading, setIsLoading] = useState<boolean>(true);
    const [scriptChanged, setScriptChanged] = useState<boolean>(false);

    const [scriptList, setScriptList] = useState<Array<Script>>([]);
    const [currentScript, setCurrentScript] = useState<DropdownOption>();
    const [scriptContent, setScriptContent] = useState<string>("");
    const [originalScriptContent, setOriginalScriptContent] = useState<string>("");
    const [base64ByteCodeDL, setBase64ByteCodeDL] = useState<string>();

    useEffect(() => { (async () => {
        setIsLoading(true);
        setScriptList(await loadScriptList());
        setIsLoading(false);
    })(); }, [setScriptList]);

    useEffect(() => {        
        if(scriptList.length > 0 && currentScript === undefined)
            changeScript({ key: scriptList[0].name, value: scriptList[0].name });
    }, [scriptList]);

    useEffect(() => setScriptChanged(scriptContent !== originalScriptContent), [scriptContent, originalScriptContent]);

    let changeScript = async (script: DropdownOption) => {
        setIsLoading(true);
        let [success, content] = await getScript(script.key);
        if(success) {
            setBase64ByteCodeDL(undefined);
            setScriptContent(content);
            setOriginalScriptContent(content);
            setCurrentScript(script);
            setIsLoading(false);
        }        
    };

    let saveAndCompileScript = async () => {
        setIsLoading(true);
        let [success, bytecode, base64Bytecode] = await compileScript(scriptContent);
        if(success) success = await saveScript(currentScript!.key, scriptContent, bytecode);
        if(success) {
            setBase64ByteCodeDL(base64Bytecode);
            setOriginalScriptContent(scriptContent);
        }
        setIsLoading(false);
    };

    let download = () => {
        var a = document.createElement("a");
        a.href = "data:application/octet-stream;base64," + base64ByteCodeDL;
        a.download = currentScript?.key + ".b";
        a.click();
    };

    return <StyledScriptEditor>
        <StyledScriptCommandbar>
            <span className="label">Current Script:</span>
            <Dropdown placeholder="---" id="script-dropdown" value={ currentScript } onChanged={ (option) => changeScript(option) }
                options={ scriptList.map(s => { return { key: s.name, value: s.name }; }) } disabled={ isLoading } />
            <span id="spacer"></span>
            <IconButton icon={faSave} tooltip="Save and compile script" onClick={ () => saveAndCompileScript() } disabled={ isLoading || !scriptChanged } />
            <IconButton icon={faDownload} tooltip="Download Bytecode" onClick={ () => download() } disabled={ isLoading || !base64ByteCodeDL } />
            <IconButton icon={faTrashAlt} tooltip="Delete script" onClick={ () => {} } disabled={ isLoading } />
            <IconButton icon={faPlus} tooltip="Create script" onClick={ () => {} } disabled={ isLoading } />
        </StyledScriptCommandbar>
        <Editor content={originalScriptContent} onChange={ (content) => setScriptContent(content) } disabled={ isLoading } />
    </StyledScriptEditor>;
}

const StyledScriptEditor = styled.div`
    height: 100%;
    overflow: hidden;
`;

const StyledScriptCommandbar = styled.div`
    display: flex;
    align-items: center;

    border-bottom: 1px solid rgb(210, 210, 210);
    padding: 10px 15px;

    #script-dropdown {
        width: 250px;
        margin-right: 7px;
    }

    #spacer {
        flex-grow: 1;
        display:block;
    }

    .label {
        margin: 0 10px;
    }
`;

export default ScriptEditor;
