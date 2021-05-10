import { Dispatch, SetStateAction, useEffect, useState } from 'react';
import styled from 'styled-components';

import Modal from '../../ui/Modal';
import Button from '../../ui/Button';

import { ModalInfo } from '../../../effects/SyncBlinkFlowDataEffect';

export interface FlasherProps {
    nodeId: number;
    nodeLabel: string;
    setModal: Dispatch<SetStateAction<ModalInfo | undefined>>;    
    setShowLoader: Dispatch<SetStateAction<boolean>>;
    setLoaderMessage: Dispatch<SetStateAction<string>>;
}

function Flasher(props: FlasherProps) {
    const [selectedFile, setSelectedFile] = useState<File>();
    const [error, setError] = useState<string>()
    useEffect(() => {
        if(selectedFile && selectedFile.size > 1000000) {
            setError("File to big!");
        }
        else if(selectedFile && selectedFile.type !== "application/octet-stream") {
            setError("Wrong file type!");
        }
        else setError(undefined);
    }, [selectedFile]);

    let flashMesh = async () => {
        if(window.confirm("Are you sure?")) {
            props.setLoaderMessage('Firmware uploading, please check the status on the station display...');
            props.setShowLoader(true);
            props.setModal(undefined);
            
            let formData = new FormData();
            formData.append("firmware", selectedFile!);
            await fetch('/api/mesh/flash?nodeId=' + props.nodeId + '&size=' + selectedFile!.size, {method: "POST", body: formData, });
            props.setShowLoader(false);    
        }
    };

    return (
        <StyledFlasher>
            <Modal title="Upload Firmware" onClose={() => props.setModal(undefined)}>
                <div className="flasher">
                    <div className="flash-info"><span>Select update for</span><p>{ props.nodeLabel ? props.nodeLabel : props.nodeId.toString(16).toUpperCase() }</p></div>
                    { selectedFile && <div className="file-info">
                            <span>Selected file</span>
                            <p>{selectedFile.name}</p>
                            { error && <p className="file-error">{error}</p> }
                            <Button color={'#2ea169'} onClick={() => setSelectedFile(undefined)}>Clear</Button>
                        </div> 
                    }
                    { !selectedFile && <label className="file-button">
                            <div className="file-label">
                                <span>Select Firmware</span>
                            </div>
                            <input type="file" 
                                onChange={(e) => setSelectedFile(e.target.files !== null ? e.target.files[0] : undefined)}
                                accept=".bin"/>
                        </label>
                    }
                    <Button disabled={selectedFile === undefined || error !== undefined} onClick={flashMesh}>Flash</Button>
                </div>
            </Modal>
        </StyledFlasher>
    );
}

const StyledFlasher = styled.div`
    .flash-info, .file-info {
        text-align: center;

        p{
            margin: 0 0 15px 0;
            line-height: 1rem;
            text-align: center;
            font-weight: bold;
        }
    }

    .file-error { color: red; }

    .file-label { 
        width: 100%;
        padding: 10px;
        border-radius: 5px;
        box-sizing: border-box;
        color: white;
        border: none; 
        background: ${ p => p.theme.secondaryColor };
        cursor: pointer;
        line-height: normal;

        font-size: 1rem;
        font-family: Segoe UI, Roboto, Noto Sans, Ubuntu, Cantarell, Helvetica Neue,sans-serif!important;
    }

    .file-label:active {
        transform: scale(0.99);
    }

    .file-label:hover {
        color: black;
        background: ${ p => p.theme.primaryColor };
    }

    .file-button {
        text-align: center; 
    }

    .file-button > input[type="file"] {
        display: none;
    }

    .flasher button {
        margin-top: 5px;
    }
`;

export default Flasher;
