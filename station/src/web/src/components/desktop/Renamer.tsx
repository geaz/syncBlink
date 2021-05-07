import { useState } from 'react';

import Modal from '../ui/Modal';
import Button from '../ui/Button';

export interface RenamerProps {
    nodeId: number;
    currentLabel: string;
    onClose: () => void;
    beforeSave: () => void;
    afterSave: () => void;
}

function Renamer(props: RenamerProps) {
    const [newLabel, setNewLabel] = useState(props.currentLabel);

    return (
        <Modal title="Rename Node" onClose={props.onClose}>
            <input type="text" defaultValue={ props.currentLabel } onChange={(e) => setNewLabel(e.target.value) }></input>
            <Button onClick={async () => { 
                props.beforeSave();
                await fetch(`/api/mesh/rename?nodeId=${props.nodeId}&label=${newLabel}`);
                props.afterSave(); 
            }}>Save</Button>
        </Modal>
    );
}

export default Renamer;
