import { useState } from 'react';

import Modal from '../ui/Modal';
import Button from '../ui/Button';

export interface FlasherProps {
    nodeId: number;
    nodeLabel: string;
    error?: string
    onClose: () => void;    
    beforeFlash: () => void;
    afterFlash: () => void;
}

function Flasher(props: FlasherProps) {
    const [selectedFile, setSelectedFile] = useState<File>();

    let checkFile = () => {
        if(selectedFile && selectedFile.size > 1000000) {
            props.error = "File to big!";
        }
        else if(selectedFile && selectedFile.type != "application/octet-stream") {
            props.error = "Wrong file type!";
        }
    }

    let flashMesh = async () => {
        if(window.confirm("Are you sure?")) {
            props.beforeFlash();
            let formData = new FormData();
            formData.append("firmware", selectedFile!);
            await fetch('/api/mesh/flash?nodeId=' + props.nodeId + '&size=' + selectedFile!.size, {method: "POST", body: formData, });    
            props.afterFlash();
        }
    };

    return (
        <Modal title="Upload Firmware" onClose={props.onClose}>
            <div>
                <div><small>Select update for</small><p>{ props.nodeLabel ? props.nodeLabel : props.nodeId.toString(16).toUpperCase() }</p></div>
                { selectedFile && <div>
                        <small>Selected file</small>
                        <p>{selectedFile.name}</p>
                        { props.error && <p>{props.error}</p> }
                        <button onClick={() => setSelectedFile(undefined)}>Clear</button>
                    </div> 
                }
                { !selectedFile && <label>
                        <div>
                            <span>Select Firmware</span>
                        </div>
                        <input type="file" 
                            onChange={(e) => { setSelectedFile(e.target.files !== null ? e.target.files[0] : undefined); checkFile(); }}
                            accept=".bin"/>
                    </label>
                }
                <Button disabled={selectedFile === undefined} onClick={flashMesh}>Flash</Button>
            </div>
        </Modal>
    );
}

export default Flasher;
