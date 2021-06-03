import { Dispatch, SetStateAction, useState } from 'react';

import Modal from '../../ui/Modal';
import Button from '../../ui/Button';

import { ModalInfo } from '../../../effects/SyncBlinkFlowDataEffect';

export interface RenamerProps {
    nodeId: number;
    currentLabel: string;
    setModal: Dispatch<SetStateAction<ModalInfo | undefined>>;    
    setShowLoader: Dispatch<SetStateAction<boolean>>;
    setLoaderMessage: Dispatch<SetStateAction<string>>;
    afterSave: () => void;
}

function Renamer(props: RenamerProps) {
    const [newLabel, setNewLabel] = useState(props.currentLabel);

    return (
        <Modal title="Rename Node" onClose={() => props.setModal(undefined)}>
            <input type="text" defaultValue={ props.currentLabel } onChange={(e) => setNewLabel(e.target.value) }></input>
            <Button onClick={async () => { 
                props.setLoaderMessage('Renaming node. Please wait...');
                props.setShowLoader(true);
                props.setModal(undefined);
                await fetch(`/api/mesh/rename?nodeId=${props.nodeId}&label=${newLabel}`);
                props.afterSave();
                props.setShowLoader(false);
            }}>Save</Button>
        </Modal>
    );
}

export default Renamer;
