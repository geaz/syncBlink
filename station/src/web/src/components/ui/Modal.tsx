import styled from "styled-components";
import { faTimes } from '@fortawesome/free-solid-svg-icons';

import IconButton from './IconButton';

interface ModalProps  {
    title: string;
    children: any;
    onClose: () => void;
}

function Loader(props: ModalProps) {
    return (
        <StyledModal>
            <div className="modal-wrapper">
                <div className="modal-container">
                    <div className="modal-close-btn">
                        <IconButton icon={faTimes} tooltip="Close" onClick={props.onClose} />
                    </div>
                    <div className="modal-header">
                        <h3>{props.title}</h3>                        
                    </div>
                    <div className="modal-content">
                        {props.children}
                    </div>
                </div>
            </div>
        </StyledModal>
    );
}

const StyledModal = styled.div`
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    position: fixed;

    z-index: 99;
    display: table;

    @keyframes showAnim {
        from { opacity: 0; }
        to { opacity: 1; }
    }

    .modal-wrapper {
        display: table-cell;
        vertical-align: middle;
        animation-name: showAnim;
        animation-duration: 1s;
    }

    .modal-container {
        width: 300px;
        margin: 0px auto;
        padding: 20px 30px;
        background-color: #fff;
        border-radius: 5px;
        box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
        transition: all 0.3s ease;
        position:relative;
    }

    .modal-close-btn {
        position:absolute;
        right: 25px;
        top: 25px;
    }

    .modal-header h3 {
        margin-bottom: 1rem;
    }

    .modal-content {
        margin: 10px 0;
    }
`;

export default Loader;
