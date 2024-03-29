import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faChevronDown } from '@fortawesome/free-solid-svg-icons';
import { useEffect, useRef, useState } from "react";

export interface DropdownOption {
    key: string;
    value: string;
}

interface DropdownProps  {
    id?: string;
    disabled?: boolean;
    tooltip?: string;
    color?: string;
    value?: DropdownOption;
    onChanged?: (option: DropdownOption) => void;
    options: Array<DropdownOption>;
    placeholder: string;
}

function Dropdown(props: DropdownProps) {
    const ref = useRef<HTMLDivElement>(null);
    const [showOptions, setShowOptions] = useState<boolean>(false);

    let changeOption = (option: DropdownOption) => {
        setShowOptions(false);
        if(props.onChanged) props.onChanged(option);
    };

    useEffect(() => {
        const listener = (event: any) => {
            if (!ref.current || ref.current.contains(event.target)) {
                return;
            }
            setShowOptions(false);
        };

        document.addEventListener("mousedown", listener);
        document.addEventListener("touchstart", listener);
        document.addEventListener("pointerdown", listener);

        return () => {
            document.removeEventListener("mousedown", listener);
            document.removeEventListener("touchstart", listener);
            document.addEventListener("pointerdown", listener);
        };
    }, [ref]);

    return (
        <StyledDropdown ref={ref} id={props.id} className="dropdown">
            <StyledDropdownButton disabled={props.disabled} title={props.tooltip} onClick={ () => setShowOptions(!showOptions) }>
                { props.value && <span>{ props.value.value }</span> }
                { !props.value && <span>{ props.placeholder }</span> }
                <FontAwesomeIcon icon={faChevronDown} />
            </StyledDropdownButton>
            <StyledDropdownList visible={ showOptions }>
                { props.options.length > 0 && props.options.map(s => <li key={s.key} onClick={ (e) => changeOption(s) }>{s.value}</li> ) }
            </StyledDropdownList>
        </StyledDropdown>
    );
}

const StyledDropdown = styled.div`
    position: relative;
    background: transparent;
`;

const StyledDropdownButton = styled.button`
    display: flex;
    align-items: center;
    width: 100%;

    background: white;
    border: black;
    border-radius: 0.35em;

    padding: 0.75em 1rem;
    font-size: 1em;
    font-weight: 400;
    font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif;

    color: ${ p => p.theme.textColor? p.theme.textColor : '#313131' };

    &:hover {
        background: rgba(240, 240, 240, 1);
    }

    &:disabled, &:disabled * {        
        color: rgba(180, 180, 180, 1)!important;
    }

    span {
        flex-grow: 1;
        text-align: left;
        line-height: 1em;
        margin: 0;
        padding: 0;
        padding-right: .5rem;
    }

    svg {
        padding-top: 0.1rem;
    }
`;

const StyledDropdownList = styled.ul<{visible: boolean}>`
    display: ${ p => p.visible ? "block" : "none" };
    position: absolute;
    margin-top: 0.5rem;

    background: white;
    z-index: 999;
    border-radius: 0.35em;
    box-shadow: rgba(0, 0, 0, 0.16) 0 0.2rem 0.4rem, rgba(0, 0, 0, 0.23) 0 0.2rem 0.4rem;
    min-width: 100%;
    box-sizing: border-box;

    li {
        padding: 0.75em 1rem;
        border-radius: 0.35rem;
        &:hover { background: rgba(240, 240, 240, 1); }
    }
`;

export default Dropdown;
