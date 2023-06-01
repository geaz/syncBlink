import { useEffect, useState } from "react";
import styled from "styled-components";

import Button from "../ui/Button";
import Loader from "../ui/Loader";
import Dropdown, { DropdownOption } from "../ui/Dropdown";

function Settings() {
    const [showLoader, setShowLoader] = useState(true);
    const [scriptList, setScriptList] = useState<Array<DropdownOption>>([]);
    const [analyzerList, setAnalyzerList] = useState<Array<DropdownOption>>([]);

    const [script, setScript] = useState<string>();
    const [analyzerId, setAnalyzerId] = useState<string>();
    const [ssid, setSsid] = useState('');
    const [wifiPw, setWifiPw] = useState('');
    const [lightMode, setLightMode] = useState(false);

    useEffect(() => {
        (async () => {
            let listResponse = await fetch("/api/scripts/list");
            let meshResponse = await fetch("/api/mesh/info");
            let wifiResponse = await fetch("/api/wifi/get");
            if(listResponse.ok && meshResponse.ok && wifiResponse.ok) {
                let scriptList = (await listResponse.json()).scripts.map((s:any) => { return { key: s.name, value: s.name }; });
                let meshInfo = await meshResponse.json();
                let wifiInfo = await wifiResponse.json();

                let analyzerList : Array<DropdownOption> = [];
                meshInfo.nodes.forEach((node: any) => {
                    if(node.isAnalyzer) {
                        analyzerList.push({value: node.label, key: node.nodeId});
                    }
                });

                setScriptList(scriptList);
                setScript(meshInfo.script);
                setLightMode(meshInfo.lightMode === "true");
                setAnalyzerList(analyzerList);
                setAnalyzerId(meshInfo.analyzer);
                setSsid(wifiInfo.ssid);
            }
            else { throw new Error("Error during requests!"); }
            setShowLoader(false);
        })();
    }, [setScriptList, setScript, setAnalyzerId, setLightMode, setAnalyzerList, setSsid, setShowLoader]);

    let onSaveScriptSettings = async () => {
        setShowLoader(true);
        await fetch(`/api/scripts/set?name=${script}`);
        await fetch(`/api/mesh/setAnalyzer?analyzerId=${analyzerId}`)
        setShowLoader(false);
    };

    let onSaveWifiSettings = async () => {
        setShowLoader(true);
        await fetch(`/api/wifi/set?ssid=${ssid}&pass=${wifiPw}`);
        setShowLoader(false);
    };

    let onSetLightMode = async () => {
        setLightMode(!lightMode);
        await fetch('/api/mesh/setLightMode?lightMode=' + (lightMode ? "false" : "true"));
    };

    return <StyledSettings>
        { showLoader && <Loader message="Loading..." transparent={false}></Loader> }
        <h2>Script Settings</h2>
        <div className="settings">
            <div className="form-line">
                <label>Active Script</label>
                <Dropdown onChanged={(e) => setScript(e.key)} value={scriptList?.filter(a => a.key === script)[0]}
                    options={scriptList} placeholder="Select script" />
            </div>
            <div className="form-line">
                <label>Analyzer</label>                
                <Dropdown onChanged={(e) => setAnalyzerId(e.key)} value={analyzerList?.filter(a => a.key === analyzerId)[0]}
                    options={analyzerList} placeholder="Select analyzer" />
            </div>
            <Button onClick={onSaveScriptSettings}>Save</Button>
            <Button onClick={onSetLightMode}>{ lightMode ? "Light Mode: ON" : "Light Mode: OFF" }</Button>
        </div>
        <h2>WiFi Settings</h2>
        <div className="settings">
            <div className="form-line">
                <label htmlFor="ssid">SSID</label>
                <input name="ssid" type="text" defaultValue={ ssid } onChange={(e) => setSsid(e.target.value) } maxLength={32}/>
            </div>
            <div className="form-line">
                <label htmlFor="pass">Pass</label>
                <input name="pass" type="text" defaultValue={ wifiPw } onChange={(e) => setWifiPw(e.target.value) } maxLength={64}/>
            </div>
            <Button onClick={onSaveWifiSettings}>Save</Button>
        </div>
    </StyledSettings>;
}

const StyledSettings = styled.div`
    height: 100vh;
    padding: 2rem;

    .settings{
        display: flex;
        flex-direction: column;
        row-gap: 0.5rem;
        margin-bottom: 2rem;
    }

    .form-line {
        display: grid;
        grid-template-areas: "left right";
        grid-template-columns: 100px 1fr;
        align-items: center;

        label { text-align: right; margin-right: 0.5rem; }
        input, select { padding: 5px; color: $text-color; }
    }
`;

export default Settings;
