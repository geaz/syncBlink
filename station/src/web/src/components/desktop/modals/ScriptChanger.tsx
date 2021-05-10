import { Dispatch, SetStateAction, useEffect, useState } from 'react';

import Modal from '../../ui/Modal';
import Button from '../../ui/Button';

import { ModalInfo } from '../../../effects/SyncBlinkFlowDataEffect';

export interface ScriptChangerProps {
    currentScript: string;
    setModal: Dispatch<SetStateAction<ModalInfo | undefined>>;    
    setShowLoader: Dispatch<SetStateAction<boolean>>;
    setLoaderMessage: Dispatch<SetStateAction<string>>;
    afterSave: () => void;
}

function ScriptChanger(props: ScriptChangerProps) {
    const [script, setScript] = useState(props.currentScript);
    const [scriptList, setScriptList] = useState<Array<string>>();

    useEffect(() => {
        (async () => {
            props.setLoaderMessage('Loading scripts...');
            props.setShowLoader(true);
            let response = await fetch("/api/scripts/list");
            if(response.ok) {
                setScriptList((await response.json()).scripts);
            }
            else { throw("Error during script list request!"); }
            props.setShowLoader(false);
        })();
    }, []);

    let scriptOptionList = scriptList && scriptList.length > 0 && scriptList.map((item, i) => {
      return (<option key={i} value={item}>{item}</option>)
    });

    return <div>{ scriptList && <Modal title="Select Script" onClose={() => props.setModal(undefined)}>
        <select onChange={(e) => setScript(e.target.value)} value={script}>
            {scriptOptionList}
        </select>
        <Button onClick={async () => { 
            props.setLoaderMessage('Changing script. Please wait...');
            props.setShowLoader(true);
            props.setModal(undefined);
            await fetch(`/api/scripts/setActive?name=${script}`);
            props.afterSave();
            props.setShowLoader(false);
        }}>Save</Button>
    </Modal> }</div>;
}

export default ScriptChanger;
