import { Reset } from 'styled-reset'
import { ThemeProvider, createGlobalStyle } from "styled-components";

import DesktopHeader from './components/DesktopHeader';
import Mesh from './components/Mesh';
import DesktopFooter from './components/DesktopFooter';

function App() {
    return <ThemeProvider theme={Theme}>
        <Reset/>
        <GlobalStyle backgroundColor={ Theme.backgroundColor } textColor={ Theme.textColor } />
        <DesktopHeader/>
        <Mesh/>
        <DesktopFooter/>
    </ThemeProvider>;
}

export const Theme = {
    backgroundColor: "#f7f7f7",
    textColor: "#313131",
    textColorFaded: "#5e5e5e",
    textColorMoreFaded: "#949494",
    colorRed: "#ff4a4a",
    colorBlue: "#4c5fff",
    colorGreen: "#2ea169",
    borderColor: "rgba(0, 0, 0, 0.1)",
    primaryColor: "#F58026",
    secondaryColor: "#0075c4",
    contentPadding: 20
};

const GlobalStyle = createGlobalStyle<{ backgroundColor : string, textColor: string }>`
    html, body {
        width: 100%;
        height: 100%;
        background: ${ p => p.backgroundColor };
    }

    body {
        color: ${ p => p.textColor };
        line-height: 1.65;
        font-weight: 400;
        font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif; 
    }

    p {margin-bottom: 1.15rem; text-align: justify; }

    h1, h2, h3, h4, h5 {
        margin: 0 0 2rem 0;
        line-height: 1.15;
        font-weight: 100;
    }
    
    h1 {
        margin-top: 0;
        font-size: 2.488em;
    }

    h2 { font-size: 2.074em; }
    h3 { font-size: 1.728em; }
    h4 { font-size: 1.44em; }
    h5 { font-size: 1.2em; }
    small, .text-small { font-size: 0.833em; color: #777777; }

    #root {
        width: 100vw;
        height: 100vh;
    }
`;

export default App;
