import { Dispatch, SetStateAction, useEffect, useState } from "react";
import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faWifi } from '@fortawesome/free-solid-svg-icons';
import { faGithub } from '@fortawesome/free-brands-svg-icons';
import IconButton from "./IconButton";

function DesktopFooter() {
    const [wifiName, setWifiName] = useState('');

    useEffect(() => { getWifiSetting(setWifiName); }, []);

    return (
        <StyledDesktopFooter>
            { wifiName && <div><FontAwesomeIcon icon={faWifi}/> <span>{wifiName}</span></div> }
            <div>
                <span>Created by Gerrit 'Geaz' Gazic</span>
                <IconButton icon={faGithub} onClick={() => window.open('https://github.com/syncBlink/syncBlink')}/>
            </div>
        </StyledDesktopFooter>
    );
}

async function getWifiSetting(setWifiName: Dispatch<SetStateAction<string>>) {
    let response = await fetch('/api/wifi/get');
    setWifiName((await response.json()).ssid);
}

const StyledDesktopFooter = styled.div`
    background: transparent;

    left:50px;
    bottom:30px;
    z-index:999;
    position:absolute;

    color: ${ p => p.theme.textColorFaded };
    font-style: italic;
    font-size: 0.75rem;
`;

export default DesktopFooter;
