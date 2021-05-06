import dagre from 'dagre';
import { Dispatch, SetStateAction, useEffect, useState } from "react";
import { isNode, Elements, Position, useStoreState } from 'react-flow-renderer';

export default function useDagreLayout(dataFunc: (setReload: Dispatch<SetStateAction<boolean>>) => Promise<Elements>)
    : [boolean, boolean, Elements]
{
    const [isLoading, setLoading] = useState<boolean>(true);
    const [isLoadingData, setLoadingData] = useState<boolean>(false);    
    const [flowElements, setFlowElements] = useState<Elements>([]);

    const elements = useStoreState((store) => [...store.nodes, ...store.edges], (one:any, two:any)  => { 
        let allSetOne = true;
        let allSetTwo = true;
        for(let i = 0; i < one.length; i++) { if(isNode(one[i]) && one[i].__rf?.width === null) { allSetOne = false; break; } }
        for(let i = 0; i < two.length; i++) { if(isNode(two[i]) && two[i].__rf?.width === null) { allSetOne = false; break; } }
        return allSetOne && allSetTwo;
    });
    
    useEffect(() => {
        if(elements.length > 0) {
            let allSet = true;
            for(let i = 0; i < elements.length; i++) { if(isNode(elements[i]) && (elements[i] as any).__rf?.width === null) { allSet = false; break; } }
            if(allSet) {
                createLayout(elements, setFlowElements);
                setLoading(false);
            }
        }
    }, [elements]);
    
    useEffect(() => { reload(isLoadingData, dataFunc, setFlowElements, setLoading, setLoadingData); }, [dataFunc, isLoadingData]);
    useEffect(() => setLoadingData(true), []);

    return [isLoading, isLoadingData, flowElements];
}

async function reload(isLoadingData: boolean, 
    dataFunc: (setLoadingData: Dispatch<SetStateAction<boolean>>) => Promise<Elements>,
    setFlowElements: Dispatch<SetStateAction<Elements>>,
    setLoading: Dispatch<SetStateAction<boolean>>,
    setLoadingData: Dispatch<SetStateAction<boolean>>)
{
    if(!isLoadingData) return;
    
    setLoading(true); // Loading will get set to false after layouting effect
    setFlowElements(await dataFunc(setLoadingData));
    setLoadingData(false);
} 

function createLayout(nodes: any, setFlowElements: Dispatch<SetStateAction<any>>) {
    let dagreGraph = new dagre.graphlib.Graph();
    dagreGraph.setDefaultEdgeLabel(() => ({}));
    dagreGraph.setGraph({ rankdir: 'TB' });

    nodes.forEach((el: any) => {
        if (isNode(el)) {
            dagreGraph.setNode(el.id, { width: el.__rf.width, height: el.__rf.height });
        } else {
            dagreGraph.setEdge(el.source, el.target);
        }
    });    
    dagre.layout(dagreGraph);

    let layoutData = nodes.map((el: any) => {
        if (isNode(el)) {
          const nodeWithPosition = dagreGraph.node(el.id);
          el.targetPosition = Position.Top;
          el.sourcePosition = Position.Bottom;

          el.position = {
            x: nodeWithPosition.x - nodeWithPosition.width / 2,
            y: nodeWithPosition.y - nodeWithPosition.height / 2,
          };
          el.__rf.position.x = el.position.x;
          el.__rf.position.y = el.position.y;
        }
        return el;
    });
    setFlowElements(layoutData);
}