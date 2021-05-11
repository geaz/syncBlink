import styled from 'styled-components';
import { Dispatch, SetStateAction, useEffect, useState } from 'react';
import { faSave, faTrashAlt } from '@fortawesome/free-solid-svg-icons';

import Editor from '../../ui/Editor';
import { ModalInfo } from '../../../effects/SyncBlinkFlowDataEffect';
import IconButton from '../../ui/IconButton';

export interface ScriptEditorProps {
    setModal: Dispatch<SetStateAction<ModalInfo | undefined>>;    
    setShowLoader: Dispatch<SetStateAction<boolean>>;
    setLoaderMessage: Dispatch<SetStateAction<string>>;
}

function ScriptEditor(props: ScriptEditorProps) {
    const [scriptList, setScriptList] = useState<Array<string>>();
    const [script, setScript] = useState<string>();
    const [scriptContent, setScriptContent] = useState<string>('');

    useEffect(() => {
        (async () => {
            props.setLoaderMessage('Loading scripts...');
            props.setShowLoader(true);
            let response = await fetch("/api/scripts/list");
            if(response.ok) {
                let scripts = (await response.json()).scripts;
                setScriptList(scripts);
                if(scripts && scripts.length > 0) setScript(scripts[0]);
            }
            else { throw new Error("Error during script list request!"); }
            props.setShowLoader(false);
        })();
    }, []);

    useEffect(() => {
        if(script === undefined) return;
        (async () => {
            props.setLoaderMessage('Loading script...');
            props.setShowLoader(true);
            let response = await fetch("/api/scripts/get?name=" + script);
            if(response.ok) {
                let script = (await response.json());
                setScriptContent(script.content);
            }
            else { throw new Error("Error during script get request!"); }
            props.setShowLoader(false);
        })();
    }, [script]);
    
    let scriptOptionList = scriptList && scriptList.length > 0 && scriptList.map((item, i) => {
        return (<option key={i} value={item}>{item}</option>)
      });

    return <div>{ scriptList && <StyledScriptEditor>
        <div className="top">
            <select onChange={(e) => setScript(e.target.value)}>
                {scriptOptionList}
            </select>
        </div>
        <div className="main">
            <Editor content={scriptContent} />
        </div>
    </StyledScriptEditor>}</div>;
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
    grid-template-rows: auto 1fr;
    grid-template-areas: "top" "main";
    animation: fadeIn 1s;

    .side {
        min-width: 300px;
        grid-area: side;
        box-sizing: border-box;
        padding: 125px 5px 20px 25px;

        ul {
            margin: 0;
            padding: 0;
            list-style: none;
        }
        li {
            display:flex;
            padding: 10px 1rem 10px 3rem;

            .script-link {
                color: ${p => p.theme.textColorFaded};
                flex-grow: 1;
                font-size: 1.1rem;
                display: inline-block;
                box-sizing: border-box;
                text-decoration: none;
            }
        }
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