import { Dispatch, SetStateAction, useEffect, useState } from "react";
import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faWifi } from '@fortawesome/free-solid-svg-icons';
import { faGithub } from '@fortawesome/free-brands-svg-icons';
import IconButton from "../ui/IconButton";

function Footer() {
    const [wifiName, setWifiName] = useState('');

    useEffect(() => { getWifiSetting(setWifiName); }, []);

    return (
        <StyledFooter>
            { wifiName && <div><FontAwesomeIcon icon={faWifi}/> <span>{wifiName}</span></div> }
            <div className="credits">
                <span>'syncBlink' created by Gerrit 'Geaz' Gazic</span>
                <IconButton icon={faGithub} onClick={() => window.open('https://github.com/syncBlink/syncBlink')}/>
            </div>
        </StyledFooter>
    );
}

async function getWifiSetting(setWifiName: Dispatch<SetStateAction<string>>) {
    let response = await fetch('/api/wifi/get');
    setWifiName((await response.json()).ssid);
}

const StyledFooter = styled.div`
    padding: 1rem 1.5rem;
    z-index:999;
    position:absolute;
    left: 5rem;
    bottom: 2rem;
    padding: 0;
    background: transparent;
    color: ${ p => p.theme.textColorFaded };
    font-style: italic;
    font-size: 0.75rem;

    .credits {
        display: flex;
        gap: 0.25rem;
    }
`;

export default Footer;
