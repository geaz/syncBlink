import { BrowserRouter as Router, Switch, Route } from "react-router-dom";

import DesktopHeader from './components/Header';
import Mesh from './components/desktop/Mesh';
import DesktopFooter from './components/desktop/Footer';
import ScriptEditor from './components/desktop/ScriptEditor';

function DesktopApp() {
    return <Router>
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