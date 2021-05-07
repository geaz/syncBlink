import styled from "styled-components";

interface ButtonProps  {
    children: any;
    color?: string;
    tooltip?: string;
    active?: boolean;
    disabled?: boolean;
    onClick?: React.MouseEventHandler<HTMLButtonElement | undefined>;
}

function Button(props: ButtonProps) {
    return (
        <StyledButton className={ props.active ? 'active' : '' } disabled={props.disabled} title={props.tooltip} onClick={props.onClick} >
            { props.children }
        </StyledButton>
    );
}

const StyledButton = styled.button`
    width: 100%;
    padding: 10px;
    border-radius: 5px;
    box-sizing: border-box;
    color: white;
    border: none; 
    background: ${ p => p.theme.secondaryColor };
    cursor: pointer;

    font-size: 1rem;
    font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif!important;

    :active:enabled {
        color: black;
        transform: scale(0.99);
    }

    :hover:enabled {
        color: black;
        background: ${ p => p.theme.primaryColor };
    }

    :disabled {
        background: gray;
        color: $text-color-faded;
    }
`;

export default Button;
