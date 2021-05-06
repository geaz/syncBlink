import styled from "styled-components";

interface ModalProps  {
    children: any;
}

function Loader(props: ModalProps) {
    return (
        <StyledModal>
            <div className="modal-content">
                {props.children}
            </div>
        </StyledModal>
    );
}

const StyledModal = styled.div`
    display:flex;

    position: absolute;
    width: 100vw;
    height: 100vh;
    top:0;
    left:0;
    background: #${ p => p.theme.backgroundColor };
`;

export default Loader;
