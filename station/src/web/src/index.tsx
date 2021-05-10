import React from 'react';
import ReactDOM from 'react-dom';

import { Reset } from 'styled-reset';
import { ReactFlowProvider } from 'react-flow-renderer';
import { ThemeProvider } from 'styled-components';
import { isMobileOnly } from "react-device-detect";

import DesktopApp from './DesktopApp';
import Typography from './components/ui/Typography';

const Theme = {
    textColor: "#313131",
    textColorFaded: "#5e5e5e",
    textColorMoreFaded: "#949494",
    primaryColor: "#F58026",
    secondaryColor: "#0075c4",
    backgroundColor: "#f7f7f7",
    borderColor: "rgba(0, 0, 0, 0.1)",
    colorRed: "#ff4a4a",
    colorBlue: "#4c5fff",
    colorGreen: "#2ea169"
};

ReactDOM.render(
    <React.StrictMode>
        <ReactFlowProvider>
            <ThemeProvider theme={Theme}>
                <Reset/>
                <Typography/>
                {isMobileOnly ? '' : <DesktopApp/>}
            </ThemeProvider>
        </ReactFlowProvider>
        </React.StrictMode>,
    document.getElementById('root')
);