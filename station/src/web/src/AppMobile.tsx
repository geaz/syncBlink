import MobileHeader from './components/Header';
import Settings from './components/mobile/Settings';

function MobileApp() {
    return <div>
        <MobileHeader showLinks={false} />
        <Settings/>
    </div>;
}

export default MobileApp;