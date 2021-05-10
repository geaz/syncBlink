import styled from "styled-components";

const StyledNode = styled.div`
    display:relative;
    cursor: default;

    .node-frame {
        text-align: center;
        padding: 10px 25px 15px 25px;
        border-radius: 5px;
        background: white;
        box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
    }

    .node-buttons {
        position:absolute;
        right:-24px;
        bottom:0;

        padding: 5px;

        display: flex;
        flex-direction: column;
    }

    .node-buttons-left {
        position:absolute;
        left:-24px;
        top:0;

        padding: 5px;

        display: flex;
        flex-direction: column;
    }

    .node-id {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.4em;
    }

    .node-info {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.5em;
        line-height:0.7rem;
        font-style: italic;
    }

    .node-script {
        color: ${ p => p.theme.textColorFaded };
        font-size: 0.5em;
        font-weight:bold;
    }

    .active {
        color: red;
    }

    button {
        font-size: 0.6rem;
    }
`;

export default StyledNode;