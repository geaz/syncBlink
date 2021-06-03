import styled from "styled-components";
import { useEffect, useState } from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faPlus, faTrashAlt, faSave } from '@fortawesome/free-solid-svg-icons';

import Loader from '../ui/Loader';
import Editor from '../ui/Editor';

function ScriptEditor() {
    const [showLoader, setShowLoader] = useState(false);
    const [showAddScript, setShowAddScript] = useState(false);
    const [scriptChanged, setScriptChanged] = useState(false);
    const [loaderMessage, setLoaderMessage] = useState('Loading scripts...');

    const [script, setScript] = useState<string>();  
    const [newScriptName, setNewScriptName] = useState<string>('');  
    const [scriptList, setScriptList] = useState<Array<string>>();
    const [scriptContent, setScriptContent] = useState<string>('');
    const [originalScriptContent, setOriginalScriptContent] = useState<string>('');  

    let loadScriptList = async (loadScript: boolean) => {
        setLoaderMessage('Loading script list...');
        setShowLoader(true);
        let response = await fetch("/api/scripts/list");
        if(response.ok) {
            let scripts = (await response.json()).scripts;
            setScriptList(scripts);
            if(loadScript && scripts && scripts.length > 0) setScript(scripts[0]);
        }
        else { throw new Error("Error during script list request!"); }
        setShowLoader(false);
    };

    let saveScript = async () => {
        let scriptInfo = {
            name: script,
            content: scriptContent
        };
        let response = await fetch(`/api/scripts/save`, {
            method: "POST",
            body: JSON.stringify(scriptInfo)
        });
        if(response.ok) {
            setOriginalScriptContent(scriptContent);
        }
        else {
            alert("Error during script save!");
        }
    };

    let deleteScript = async (scriptName: string) => {
        if(window.confirm(`Do you really want to delete the "${scriptName}" script?`)) {
            let response = await fetch(`/api/scripts/delete?name=${scriptName}`, {
                method: "GET"
            });
            if(response.ok) {
                setScript(script === scriptName ? "" : script);
                await loadScriptList(false);
            }
            else {
                alert("Error during MOD deletion!");
            }
        }
    };

    let addScript = async () => {
        setLoaderMessage('Creating script...');
        if(newScriptName !== '') {
            let response = await fetch(`/api/scripts/add?name=${newScriptName}`, {
                method: "GET"
            });
            if(response.ok) {
                setShowAddScript(false);
                setScript(newScriptName);
                setNewScriptName('');
                await loadScriptList(false);
            }
            else {
                alert("Error during script creation!");
            }                
        }
    };

    useEffect(() => { (async () => await loadScriptList(true))(); }, []);    
    useEffect(() => setScriptChanged(scriptContent !== originalScriptContent), [scriptContent, originalScriptContent]);

    useEffect(() => {
        if(script === undefined) return;
        (async () => {
            let response = await fetch("/api/scripts/get?name=" + script);
            if(response.ok) {
                let script = (await response.json());
                setScriptContent(script.content);
                setOriginalScriptContent(script.content);
            }
            else { throw new Error("Error during script get request!"); }
        })();
    }, [script]);

    let scriptItemList = scriptList && scriptList.length > 0 && scriptList.map((item, i) => {
        return (<li key={i} onClick={() => setScript(item) } className={ script === item ? 'active' : '' }>
            <a href="#" className="main-link" onClick={() => setScript(item) }>{item}</a>

            { script === item && !scriptChanged && 
                <span className="deactivated-tool"><FontAwesomeIcon icon={faSave}/></span>
            }
            { script === item && scriptChanged && 
                <a href="#" onClick={saveScript} className="tool"><FontAwesomeIcon icon={faSave}/></a>
            }            

            <a href="#" onClick={ () => deleteScript(item) } className={'tool hover-hidden ' + (script === item ? 'visible' : '') }><FontAwesomeIcon className="hover-red" icon={faTrashAlt}/></a>
        </li>)
      });

    return <>{ (showLoader) && <Loader message={loaderMessage} transparent={false} /> }
        { scriptList && <StyledScriptEditor>
        <aside className="sidebar">
            <div id="avail-scripts">
                <label>Available Scripts</label>
                <a href="#" onClick={ () => setShowAddScript(!showAddScript) }><FontAwesomeIcon icon={faPlus}/></a>
            </div>
            <ul>
                { scriptItemList }
            </ul>
            { showAddScript && <div id="add-script">
                <input onChange={ (e) => setNewScriptName(e.target.value) } onKeyUp={ (e) => { if(e.key === 'Enter') addScript(); } } type="text" placeholder="New script name ('Enter' to save)"/>
            </div> }
        </aside>
        <div className="main">
            <Editor content={originalScriptContent} onChange={ (content) => setScriptContent(content) } />
        </div>
    </StyledScriptEditor>}</>;
}

const StyledScriptEditor = styled.div`
    flex: 1;
    display: flex;
    
    .main {
        flex: 1;
    }

    .sidebar {
        width: 300px;
        box-sizing: border-box;
        padding: 1.5rem 0.5rem 1.5rem 2rem;

        ul {
            margin: 0;
            padding: 0;
            list-style: none;
        }

        li {
            display:flex;
            padding: 5px 25px;
            .main-link {
                flex-grow: 1;
                display: inline-block;
                box-sizing: border-box;
                text-decoration: none;
            }
        }

        li:hover {  
            .main-link { color: ${ p => p.theme.primaryColor }; }          
            .hover-hidden { visibility: visible; }
        }

        .tool,
        .deactivated-tool {
            padding-right: 5px;
            svg { color: #727272; }
            svg:hover { color: ${ p => p.theme.primaryColor }; }
            .blue { color: ${ p => p.theme.colorBlue }; }
            .hover-red:hover { color: ${ p => p.theme.colorRed }; }
        }

        .deactivated-tool {
            svg { color: #aaaaaa }
            svg:hover { color: #aaaaaa; }
        }

        .active { background: #e3e3e3; }
        .hover-hidden { visibility: hidden; }
        .visible { visibility: visible; }
    }

    #avail-scripts {
        display: flex;
        align-items: center;

        label { 
            flex-grow: 1;
            font-weight: lighter;
            font-size: 1.1rem;
            color: ${ p => p.theme.textColorFaded };
        }

        a {
            padding: 5px;
        }
    }

    #add-script {
        box-sizing: border-box;
        margin-top: 1rem;

        input { 
            padding: 5px; 
            width: 100%;
            box-sizing: border-box;
        }
    }
`;

export default ScriptEditor;
