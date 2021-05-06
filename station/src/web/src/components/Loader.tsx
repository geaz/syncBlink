import styled from "styled-components";

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faSyncAlt } from '@fortawesome/free-solid-svg-icons';

interface LoaderProps  {
    message: string;
}

function Loader(props: LoaderProps) {
    return (
        <StyledLoader>
            <div className="loader-content">
                <FontAwesomeIcon icon={faSyncAlt} spin={true} />
                <p>{props.message}</p>
            </div>
        </StyledLoader>
    );
}

const StyledLoader = styled.div`
    display:flex;

    position: absolute;
    width: 100vw;
    height: 100vh;
    top:0;
    left:0;
    background: #${ p => p.theme.backgroundColor };

    .loader-content {
        display:flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        margin: auto;
    }
`;

export default Loader;
