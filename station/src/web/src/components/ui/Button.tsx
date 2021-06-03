import styled from "styled-components";

interface ButtonProps  {
    children: any;
    color?: string;
    hoverColor?: string;
    tooltip?: string;
    active?: boolean;
    disabled?: boolean;
    onClick?: React.MouseEventHandler<HTMLButtonElement | undefined>;
}

function Button(props: ButtonProps) {
    return (
        <StyledButton color={props.color}
            hoverColor={props.hoverColor}
            className={ props.active ? 'active' : '' }
            disabled={props.disabled}
            title={props.tooltip}
            onClick={props.onClick}
        >
            { props.children }
        </StyledButton>
    );
}

const StyledButton = styled.button<{color?: string, hoverColor?: string}>`
    width: 100%;
    padding: 10px;
    border-radius: 5px;
    box-sizing: border-box;
    color: white;
    border: none; 
    background: ${ p => p.color !== undefined ? p.color : p.theme.secondaryColor };
    cursor: pointer;

    font-size: 1rem;
    font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif!important;

    :active:enabled {
        color: black;
        transform: scale(0.99);
    }

    :hover:enabled {
        color: black;
        background: ${ p => p.hoverColor !== undefined ? p.hoverColor : p.theme.primaryColor };
    }

    :disabled {
        background: gray;
    }
`;

export default Button;
