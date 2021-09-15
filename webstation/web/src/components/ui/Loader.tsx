import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faSyncAlt } from '@fortawesome/free-solid-svg-icons';

interface LoaderProps  {
    message: string;
    transparent: boolean;
}

function Loader(props: LoaderProps) {
    return (
        <StyledLoader transparent={props.transparent}>
            <div className="loader-content">
                <FontAwesomeIcon icon={faSyncAlt} spin={true} />
                <p>{props.message}</p>
            </div>
        </StyledLoader>
    );
}

const StyledLoader = styled.div<{ transparent: boolean }>`
    display:flex;

    position: absolute;
    width: 100vw;
    height: 100vh;
    top:0;
    left:0;
    z-index: 999;
    background: ${ p => p.transparent ? 'transparent' : p.theme.backgroundColor };

    @keyframes showAnim {
        from { opacity: 0; }
        to { opacity: 1; }
    }

    .loader-content {
        display:flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        margin: auto;
        
        animation-name: showAnim;
        animation-duration: 1s;
    }
`;

export default Loader;
