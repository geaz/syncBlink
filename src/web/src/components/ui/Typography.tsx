import { createGlobalStyle } from "styled-components";

interface TypographyProps{
    backgroundColor?: string;
    textColor?: string;
    textColorFaded?: string;
    textColorDisabled?: string;
    primaryColor?: string;
}

function Typography(props: TypographyProps) {
    return <GlobalStyle backgroundColor={props.backgroundColor} textColor={props.textColor}/>;
}

const GlobalStyle = createGlobalStyle<{backgroundColor?: string, textColor?: string, textColorFaded?: string, textColorDisabled?: string, primaryColor?: string}>`
    html, body, #root {
        width: 100%;
        height: 100%;
        font-size: 100%;
    }

    #root {
        overflow: auto;
        display: flex;
        flex-direction: column;
    }

    body {
        background: ${ p => p.backgroundColor ? p.backgroundColor : '#f7f7f7' };
        color: ${ p => p.textColor? p.textColor : '#313131' };
        line-height: 1.65;
        font-weight: 400;
        font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif;
        overflow:hidden;
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

    a {
        color: ${ p => p.primaryColor? p.primaryColor : '#313131' };
        &:hover {
            color: ${ p => p.textColor? p.textColor : '#F58026' };
        }
        i {
            vertical-align: middle;
            color: ${ p => p.textColorFaded? p.textColorFaded : '#949494' };
            &:hover { color: ${ p => p.primaryColor? p.primaryColor : '#F58026' }; }
        }
    }

    input[type="text"] {
        text-align: center;
        outline: none!important;
        border: 0;
        box-sizing: border-box;
        padding: 0;
        background: rgba(240, 240, 240, 1);
        border-radius: 3px;

        color: ${ p => p.textColor? p.textColor : '#313131' };
        line-height: 1.65;
        font-size: 1em;
        font-weight: 400;
        font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif;
    }
`;

export default Typography;