import { toast } from 'react-toastify';
import { Base64 } from 'js-base64';

import 'react-toastify/dist/ReactToastify.css';

import blinkScript from '../blinkScript';

export interface Script {
    name: string;
    isCompiled: boolean;
}

/**
 * This effect contains all blinkScript related api calls.
 */
export default function useBlinkScriptEffect()
    : [() => Promise<Array<Script>>, 
        (scriptContent: string) => Promise<[boolean, Uint8Array, string]>, 
        (scriptName: string, scriptContent: string, scriptBytecode: Uint8Array) => Promise<boolean>, 
        (scriptName: string) => void,
        (scriptName: string) => Promise<[boolean, string]>,
        (scriptName: string) => void,    
        (scriptName: string) => Promise<boolean>] 
{
    return [loadScriptList, compileScript, saveScript, deleteScript, getScript, addScript, changeScript];
}

async function loadScriptList() : Promise<Array<Script>> {
    let scripts = [];
    let toastId =  toast.loading("ℹ️ Loading script list ...");
    let response = await fetch("/api/scripts/list");
    if(response.ok) scripts = (await response.json()).scripts;
    else toast.error("Error during script list request!", { autoClose: 5000 });
    toast.dismiss(toastId);
    return scripts;
};

async function compileScript(scriptContent: string): Promise<[boolean, Uint8Array, string]> {
    let success = true;
    let scriptBytes = [];

    let toastId =  toast.loading("ℹ️ Compiling script ...");
    let blinkScriptModule = await blinkScript();
    let compilationResult = blinkScriptModule.compileScript(scriptContent);

    if(compilationResult.ErrorMessage !== "") {
        success = false;
        toast.error(`Line ${compilationResult.ErrorLine}: ${compilationResult.ErrorMessage}`, { autoClose: 7500 });
    } else {
        for (var i = 0; i < compilationResult.ByteCode.size(); i++) {
            scriptBytes.push(compilationResult.ByteCode.get(i));
        }
    }

    let scriptByteArray = new Uint8Array(scriptBytes);
    toast.dismiss(toastId);

    return [success, scriptByteArray, Base64.fromUint8Array(scriptByteArray)];
}

async function saveScript(scriptName:string, scriptContent: string, scriptBytecode: Uint8Array): Promise<boolean> {
    let postContent = async (content: string | Uint8Array, url: string) : Promise<boolean> => {
        let formData  = new FormData();
        formData.append(scriptName, new Blob([content]), scriptName);

        let response = await fetch(url, {
            method: "POST",
            body: formData
        });
        return response.ok;
    };

    let success = true;
    let toastId =  toast.loading("ℹ️ Saving script to station ...");

    success = await postContent(scriptContent, `/api/scripts/save`);
    if(success) success = await postContent(scriptBytecode, `/api/scripts/savebc`);
    if(!success) toast.error("Error during script save!", { autoClose: 5000 });
    toast.dismiss(toastId);

    return success;
};

async function deleteScript(scriptName: string): Promise<void> {
    let toastId =  toast.loading("ℹ️ Deleting script ...");
    let response = await fetch(`/api/scripts/delete?name=${scriptName}`, {
        method: "GET"
    });
    if(!response.ok) toast.error("Error during script deletion!", { autoClose: 5000 });
    toast.dismiss(toastId);    
};

async function getScript(scriptName: string): Promise<[boolean, string]> {
    let success = true;
    let toastId =  toast.loading("ℹ️ Loading script ...");
    let response = await fetch(`/api/scripts/get?name=${scriptName}`, {
        method: "GET"
    });
    let content = "";
    if(!response.ok) {
        success = false;
        toast.error("Error during script get request!", { autoClose: 5000 });
    }
    else content = await response.text();
    toast.dismiss(toastId);
    
    return [success, content];
};

async function addScript(scriptName: string): Promise<void> {
    let toastId =  toast.loading("ℹ️ Creating script ...");
    let response = await fetch(`/api/scripts/add?name=${scriptName}`, {
        method: "GET"
    });
    if(!response.ok) toast.error("Error during script creation!", { autoClose: 5000 });
    toast.dismiss(toastId);
};

async function changeScript(scriptName: string): Promise<boolean> {
    let success = true;
    let toastId =  toast.loading("ℹ️ Changing script ...");
    let response =  await fetch(`/api/scripts/set?name=${scriptName}`);
    if(!response.ok) {
        success = false;
        toast.error("Error during script change!", { autoClose: 5000 });
    }
    toast.dismiss(toastId);
    return success;
};