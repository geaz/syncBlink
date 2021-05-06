import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { IconDefinition } from '@fortawesome/free-solid-svg-icons';

interface ButtonProps  {
    active?: boolean;
    disabled?: boolean;
    icon?: IconDefinition;
    tooltip?: string;
    color?: string;
    onClick?: React.MouseEventHandler<HTMLButtonElement | undefined>;
}

function IconButton(props: ButtonProps) {
    return (
        <StyledIconButton className={ props.active ? 'active' : '' } disabled={props.disabled} title={props.tooltip} onClick={props.onClick} >
            { props.icon != null && <FontAwesomeIcon icon={props.icon} /> }
        </StyledIconButton>
    );
}

const StyledIconButton = styled.button`
    border: none;
    background: none;
    color: ${ p => p.theme.textColorMoreFaded };

    :hover:enabled {
        color: ${ p => p.theme.textColorFaded };
    }

    :active {
        color: black;
    }
`;

export default IconButton;
