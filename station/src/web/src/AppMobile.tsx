import styled from 'styled-components';

import MobileHeader from './components/mobile/Header';
import Settings from './components/mobile/Settings';

function MobileApp() {
    return <StyledApp>
        <MobileHeader/>
        <Settings/>
    </StyledApp>;
}

const StyledApp = styled.div`    
    background: ${ p => p.theme.backgroundColor };
`;

export default MobileApp;
