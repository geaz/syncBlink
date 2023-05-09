import { toast } from "react-toastify";
import 'react-toastify/dist/ReactToastify.css';

/**
 * This effect contains all node api commands used by the react flow nodes.
 */
export default function useSyncBlinkNodesEffect(refreshMeshFunc: () => void)
    : [(id: number, label: string) => Promise<boolean>, 
        (id: number, meshWifi: boolean) => Promise<boolean>, 
        (id: number) => Promise<boolean>,
        (id: number) => Promise<boolean>,
        (lightMode: boolean) => Promise<boolean>] 
{
    let renameNode = async (id: number, label: string): Promise<boolean> => {
        let success = true;
        let toastId =  toast.loading("ℹ️ Renaming node...");
        let response = await fetch(`/api/mesh/rename?nodeId=${id}&label=${label}`);
        toast.dismiss(toastId);
    
        if(!response.ok) {
            success = false;
            toast.error("Error during node renaming!", { autoClose: 5000 });
        } else {
            toast.info("Node renamed successfully!", { autoClose: 5000 });
        }
        return success;
    };

    let changeWifi = async (id: number, meshWifi: boolean): Promise<boolean>  => {
        let success = true;
        let toastId =  toast.loading("ℹ️ Changing node WiFi ...");
        let response = await fetch('/api/mesh/setNodeWifi?nodeId=' + id + '&meshWifi=' + (meshWifi ? "true" : "false"));
        toast.dismiss(toastId);
    
        if(!response.ok) {
            success = false;
            toast.error("Error during node WiFi change!", { autoClose: 5000 });
        } else {
            toast.info("Node WiFi changed successfully!", { autoClose: 5000 });
            refreshMeshFunc();
        }
        return success;
    };

    let changeAnalyzer = async (id: number): Promise<boolean> => {
        let success = true;
        let toastId =  toast.loading("ℹ️ Changing analyzer ...");
        let response = await fetch('/api/mesh/setAnalyzer?analyzerId=' + id);
        toast.dismiss(toastId);
        if(!response.ok) {
            toast.error("Error during analyzer change!", { autoClose: 5000 });
        } else {
            success = false;
            toast.info("Analyzer changed successfully!", { autoClose: 5000 });
            refreshMeshFunc();
        }
        return success;
    };
    
    let pingNode = async (id: number): Promise<boolean> => {
        let success = true;
        let toastId =  toast.loading("ℹ️ Pinging node ...");
        let response = await fetch('/api/mesh/ping?nodeId=' +id);
        toast.dismiss(toastId);
        if(!response.ok) {
            success = false;
            toast.error("Error during node ping!", { autoClose: 5000 });
        } else {
            toast.info("Node pinged successfully!", { autoClose: 5000 });
        }    
        return success;    
    };

    let toogleLightMode = async (lightMode: boolean): Promise<boolean> => {
        let success = true;
        let toastId =  toast.loading("ℹ️ Changing light mode ...");
        let response = await fetch('/api/mesh/setLightMode?lightMode=' + (lightMode ? "true" : "false"));
        toast.dismiss(toastId);
        if(!response.ok) {
            success = false;
            toast.error("Error during light mode change!", { autoClose: 5000 });
        } else {
            toast.info("Light mode changed successfully!", { autoClose: 5000 });
        }    
        return success;
    };

    return [renameNode, changeWifi, changeAnalyzer, pingNode, toogleLightMode];
}