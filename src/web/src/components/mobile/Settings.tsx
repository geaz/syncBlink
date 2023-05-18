import { useEffect, useState } from "react";
import styled from "styled-components";

import Button from "../ui/Button";
import Loader from "../ui/Loader";

function Settings() {
    const [showLoader, setShowLoader] = useState(true);
    const [scriptList, setScriptList] = useState<Array<string>>();
    const [analyzerList, setAnalyzerList] = useState<Array<{name: string, id: string}>>();

    const [script, setScript] = useState('');
    const [analyzer, setAnalyzer] = useState('');
    const [ssid, setSsid] = useState('');
    const [wifiPw, setWifiPw] = useState('');
    const [lightMode, setLightMode] = useState(false);

    useEffect(() => {
        (async () => {
            let listResponse = await fetch("/api/scripts/list");
            let meshResponse = await fetch("/api/mesh/info");
            let wifiResponse = await fetch("/api/wifi/get");
            if(listResponse.ok && meshResponse.ok && wifiResponse.ok) {
                let scriptList = (await listResponse.json()).scripts.map((s:any) => s.name);
                let meshInfo = await meshResponse.json();
                let wifiInfo = await wifiResponse.json();

                let analyzerList : Array<{name: string, id: string}> = [];
                meshInfo.nodes.forEach((node: any) => {
                    if(node.isAnalyzer) {
                        analyzerList.push({name: node.label, id: node.nodeId});
                    }
                });

                setScriptList(scriptList);
                setScript(meshInfo.script);
                setAnalyzer(meshInfo.analyzer);
                setLightMode(meshInfo.lightMode === "true");
                setAnalyzerList(analyzerList);
                setSsid(wifiInfo.ssid);
            }
            else { throw new Error("Error during requests!"); }
            setShowLoader(false);
        })();
    }, []);

    let scriptOptionList = scriptList && scriptList.length > 0 && scriptList.map((item, i) => {
        return (<option key={i} value={item}>{item}</option>)
    });

    let analyzerOptionList = analyzerList && analyzerList.length > 0 && analyzerList.map((item, i) => {
        return (<option key={item.id} value={item.id}>{item.name}</option>)
    });

    let onSaveScriptSettings = async () => {
        setShowLoader(true);
        await fetch(`/api/scripts/set?name=${script}`);
        await fetch(`/api/mesh/setAnalyzer?analyzerId=${analyzer}`)
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
                <select onChange={(e) => setScript(e.target.value)} value={script}>
                    {scriptOptionList}
                </select>
            </div>
            <div className="form-line">
                <label>Analyzer</label>
                <select onChange={(e) => setAnalyzer(e.target.value)} value={analyzer}>
                    {analyzerOptionList}
                </select>
            </div>
        </div>
        <Button onClick={onSaveScriptSettings}>Save</Button>
        <Button onClick={onSetLightMode}>{ lightMode ? "Light Mode: ON" : "Light Mode: OFF" }</Button>
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
        </div>
        <Button onClick={onSaveWifiSettings}>Save</Button>
    </StyledSettings>;
}

const StyledSettings = styled.div`
    height: 100vh;
    padding: 20px;

    .form-line {
        margin-bottom: 0.75rem;
        display: grid;
        grid-template-areas: "left right";
        grid-template-columns: 100px 1fr;

        label { text-align: right; }
        input, select { margin-left: 0.75rem; padding: 5px; color: $text-color; }
    }


    button {
        margin-bottom: 20px;
    }
`;

export default Settings;
