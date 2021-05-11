import { useEffect, useState } from "react";
import styled from "styled-components";

import Button from "../ui/Button";
import Loader from "../ui/Loader";

function Settings() {
    const [showLoader, setShowLoader] = useState(true);
    const [scriptList, setScriptList] = useState<Array<string>>();

    const [script, setScript] = useState('');
    const [analyzer, setAnalyzer] = useState(0);

    useEffect(() => {
        (async () => {
            let listResponse = await fetch("/api/scripts/list");
            let meshResponse = await fetch("/api/mesh/info");
            let wifiResponse = await fetch("/api/wifi/get");
            if(listResponse.ok && meshResponse.ok && wifiResponse.ok) {
                let scriptList = (await listResponse.json()).scripts;
                let meshInfo = await meshResponse.json();
                let wifiInfo = await wifiResponse.json();

                setScriptList(scriptList);
                setScript(meshInfo.script);
                setAnalyzer(meshInfo.analyzer);
            }
            else { throw new Error("Error during requests!"); }
            setShowLoader(false);
        })();
    }, []);

    return <StyledSettings>
        { showLoader && <Loader message="Loading..." transparent={false}></Loader> }
        <h2>Script Settings</h2>
        <div className="settings">
            <div className="form-line">
                <label>Active Script</label>
                <select></select>
            </div>
            <div className="form-line">
                <label>Analyzer</label>
                <select></select>
            </div>
        </div>
        <Button>Save</Button>
        <h2>WiFi Settings</h2>
        <div className="settings">
            <div className="form-line">
                <label htmlFor="ssid">SSID</label><input v-model="ssid" maxLength={32} />
            </div>
            <div className="form-line">
                <label htmlFor="pass">Pass</label><input v-model="pass" maxLength={64} />
            </div>
        </div>
        <Button>Save</Button>
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
