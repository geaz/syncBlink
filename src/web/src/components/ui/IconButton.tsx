import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { IconDefinition } from '@fortawesome/free-solid-svg-icons';

interface ButtonProps  {
    active?: boolean;
    disabled?: boolean;
    icon?: IconDefinition;
    children?: any;
    tooltip?: string;
    color?: string;
    onClick?: React.MouseEventHandler<HTMLButtonElement | undefined>;
}

function IconButton(props: ButtonProps) {
    return (
        <StyledIconButton className={ props.active ? 'active icon-button' : 'icon-button' } disabled={props.disabled} title={props.tooltip} onClick={props.onClick} >
            { props.icon != null && <FontAwesomeIcon className="fa-fw" icon={props.icon} /> }
            { props.children != null && props.children }
        </StyledIconButton>
    );
}

const StyledIconButton = styled.button<{noHoverBackground?: boolean}>`
    margin: 0;
    border: none;
    background: none;
    color: ${ p => p.theme.textColorMoreFaded };
    padding: 0.6em;
    font-size: 1em;

    display: flex;
    align-items: center;
    justify-content: center;
    box-sizing: border-box;

    :disabled {
        color: ${ p => p.theme.textColorDisabled };
    }

    :active:enabled {
        color: ${ p => p.theme.textColor };
        transform: scale(0.95);
    }

    :hover:enabled {
        cursor: pointer;
        color: ${ p => p.theme.textColorFaded };
        background: rgba(220, 220, 220, 1);
        border-radius: 0.3em;

        svg {
            transform: scale(1.2);
        }
    }
`;

export default IconButton;
