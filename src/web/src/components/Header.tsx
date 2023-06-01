import { NavLink } from "react-router-dom";
import styled from "styled-components";
import logo from '../assets/logo.png';

interface HeaderProps {
    showLinks: boolean
}

function Header(props: HeaderProps) {
    return (
        <StyledHeader>
            <a href="/" className="stretch"><img src={logo} alt="syncBlink Logo" /></a>
            { props.showLinks && <nav>
                <NavLink exact={true} activeClassName='active' to="/">Mesh</NavLink >
                <NavLink activeClassName='active' to="/editor">Editor</NavLink >
            </nav> }
        </StyledHeader>
    );
}

const StyledHeader = styled.div`
    display:flex;
    background: white;
    box-shadow: rgba(99, 99, 99, 0.2) 0 0.2rem 0.5rem 0;
    padding: 1rem 1.5rem;
    z-index: 999;

    img {
        width: 100px;
        vertical-align: middle;
    }

    .stretch {        
        flex:1;
    }

    nav {
        display: flex;
    }

    nav a{
        margin-left: 1.5rem;
        font-weight: 700;
        font-size: 1.2rem;
        text-decoration: none;
        color: #a1a0a0;
    }

    nav a:hover {
        color: ${ p => p.theme.primaryColor };
    }

    nav .active {        
        color: ${ p => p.theme.textColor };
    }
`;

export default Header;
