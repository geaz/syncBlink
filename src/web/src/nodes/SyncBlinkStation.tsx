
import { useEffect, useState } from 'react';
import { NodeProps } from 'react-flow-renderer';
import { faHeadphones, faSync, faLightbulb } from '@fortawesome/free-solid-svg-icons';

import BaseNode from './StyledNode';
import IconButton from '../components/ui/IconButton';
import Dropdown from '../components/ui/Dropdown';
import useBlinkScriptEffect, { Script } from '../effects/BlinkScriptEffect';
import useSyncBlinkNodesEffect from '../effects/SyncBlinkNodesEffect';

export interface SyncBlinkStationProps {
    id: number;
    ledCount: number;
    isActive: boolean;
    isLightMode: boolean;
    majorVersion: number;
    minorVersion: number;
    runningScript: string;
    refreshMeshFunc: () => void;
}

function SyncBlinkStation(props: NodeProps<SyncBlinkStationProps>) {    
    const [loadScriptList,,,,,, changeScript] = useBlinkScriptEffect();
    const [,, changeAnalyzer,, toogleLightMode] = useSyncBlinkNodesEffect(props.data.refreshMeshFunc);
    const [isScriptChanging, setIsScriptChanging] = useState<boolean>(false);
    const [runningScript, setRunningScript] = useState<string>(props.data?.runningScript);
    const [scriptList, setScriptList] = useState<Array<Script>>([]);

    useEffect(() => {
        (async () => { setScriptList((await loadScriptList()).filter(s => s.isCompiled)); })();
    }, [setScriptList, loadScriptList]);

    let onChangeScript = async (scriptName: string) => {
        let previousScriptName = runningScript;
        setIsScriptChanging(true);
        setRunningScript(scriptName);
        if(!(await changeScript(scriptName))) {
            setRunningScript(previousScriptName);
            setIsScriptChanging(false);
        }
        setIsScriptChanging(false);
    };

    const leftBarContent = <>
        <IconButton icon={faLightbulb}
            tooltip="Toogle light mode"
            active={props.data?.isLightMode}
            onClick={ () => toogleLightMode(!props.data?.isLightMode) } />
        <IconButton icon={faSync}
            tooltip="Refresh Mesh"
            onClick={ () => props.data?.refreshMeshFunc() } /></>;

    const rightBarContent = <IconButton icon={faHeadphones}
        active={props.data?.isActive}
        disabled={props.data?.isActive}
        tooltip="Activate Analyzer"
        onClick={() => changeAnalyzer(props.data?.id)} />;

    return <BaseNode
        hasTopHandle={true}
        hasBottomHandle={true}
        leftBarContent={leftBarContent}
        rightBarContent={rightBarContent}>
            <div className="node-label">Station</div>
            <div className="node-script">
                <span>Running:</span> 
                <Dropdown placeholder="---" value={ { key: runningScript, value: runningScript } } disabled={ isScriptChanging || scriptList.length === 0 }
                    options={ scriptList.map(s => { return { key: s.name, value: s.name }; }) } onChanged={ (option) => onChangeScript(option.key) } />
            </div>
            <div className="node-info">{props.data?.ledCount} LEDs - v{props.data?.majorVersion}.{props.data?.minorVersion}</div>
    </BaseNode>;
}

export default SyncBlinkStation;