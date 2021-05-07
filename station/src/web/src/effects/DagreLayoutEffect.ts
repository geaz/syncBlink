import dagre from 'dagre';
import { Dispatch, SetStateAction, useEffect, useState } from "react";
import { isNode, Elements, Position, useStoreState } from 'react-flow-renderer';

export default function useDagreLayout(dataFunc: (setReload: () => void) => Promise<Elements>)
    : [boolean, boolean, Elements, () => void]
{
    const [isLoading, setLoading] = useState<boolean>(true);
    // This state is to inform, that the node data got loaded and the first rendering
    // was done. The layouting is pending. The external component, which is using this effect for the react-flow-graph,
    // should hide/show (via opacity) the react-flow-graph based on this state. Otherwise graph updates will not be shown correctly.
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
    
    let reloadData = () => { reload(dataFunc, reloadData, setFlowElements, setLoading, setLoadingData); };
    useEffect(() => reloadData(), []);

    return [isLoading, isLoadingData, flowElements, reloadData];
}

async function reload(dataFunc: (reloadData: () => void) => Promise<Elements>,
    reloadDataFunc: () => void,
    setFlowElements: Dispatch<SetStateAction<Elements>>,
    setLoading: Dispatch<SetStateAction<boolean>>,
    setLoadingData: Dispatch<SetStateAction<boolean>>)
{    
    setLoading(true);
    setLoadingData(true);
    setFlowElements(await dataFunc(reloadDataFunc));
    setLoadingData(false);
    // Loading will get set to false after layouting effect
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