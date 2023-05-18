import { BrowserRouter as Router, Switch, Route } from "react-router-dom";
import { ToastContainer, toast } from "react-toastify";

import DesktopHeader from './components/Header';
import Mesh from './components/desktop/Mesh';
import DesktopFooter from './components/desktop/Footer';
import ScriptEditor from './components/desktop/ScriptEditor';

function DesktopApp() {
    return <Router>        
        <ToastContainer closeButton={false} position={toast.POSITION.TOP_CENTER} autoClose={false} />
        <Switch>
            <Route path="/editor">
                <DesktopHeader showLinks={true} />
                <ScriptEditor/>
            </Route>
            <Route path="/">
                <DesktopHeader showLinks={true} />
                <Mesh/>
                <DesktopFooter/>
            </Route>
        </Switch>
    </Router>;
}

export default DesktopApp;