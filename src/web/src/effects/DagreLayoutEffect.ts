import dagre from 'dagre';
import { useEffect, useState } from "react";
import { Position, Node, Edge, useNodesState, useEdgesState, useStore } from 'react-flow-renderer';

/**
 * This effect is responsible for creating the tree layout of the react-flow-element.
 * It works in several steps:
 * 1. Set loading to true - Load the synkBlink mesh data
 * 2. Set the react-flow-renderer nodes and edges to use this data
 * 3. Wait for the first render by using the internal store to get the internal nodes.
 *    If all nodes have a width, the initial rendering is done and we are able to use the size information of the nodes in the following step
 * 4. Use dagre to layout a tree of the nodes and edges by using the now populated size information for all individual nodes
 * 5. Set the nodes and edges of the react-flow-renderer to the new calculated list of nodes and edges - set loading to false
 * 
 * @param loadSyncBlinkFlowData A loader to load the nodes and edges for the current syncBlink mesh. 
 * @returns An array of different values. 
 * The first boolean is an indicator, if the data get reloaded at the moment. This can be used to show a loading indicator.
 * The second value is a function to trigger a reload of the current mesh information.
 * The third and fourth value is a list of nodes and edges to be used in the react-flow-renderer component.
 */
export default function useDagreLayout(loadSyncBlinkFlowData: (onRefreshPressed: () => void) => Promise<[Array<Node>, Array<Edge>]>)
    : [boolean, () => void, Array<Node>, Array<Edge>]
{
    const [isLoading, setLoading] = useState<boolean>(true);
    const [isCreateLayout, setCreateLayout] = useState<boolean>(false);

    const [nodes, setNodes] = useNodesState([]);
    const [edges, setEdges] = useEdgesState([]);
    const internalNodes = useStore((store) => Array.from(store.nodeInternals.values()));
    
    useEffect(() => {
        if(!isCreateLayout || internalNodes.length === 0) return;
        if(internalNodes.find(n => n.width === undefined)) return;

        setNodes(createLayout(internalNodes, edges));
        setLoading(false);
        setCreateLayout(false);
    }, [internalNodes, isCreateLayout, edges, setNodes]);
    
    let reloadData = async () => { 
        setLoading(true);
        setNodes([]); // reset nodes to make sure that we don't cache old values
        let [blinkNodes, blinkEdges] = await loadSyncBlinkFlowData(reloadData);
        setNodes(blinkNodes);
        setEdges(blinkEdges);
        setCreateLayout(true);
    };
    useEffect(() => { (async () => reloadData())(); }, []);

    return [isLoading, reloadData, nodes, edges];
}

function createLayout(nodes: Array<Node>, edges: Array<Edge>): Array<Node> {
    let dagreGraph = new dagre.graphlib.Graph();
    dagreGraph.setDefaultEdgeLabel(() => ({}));
    dagreGraph.setGraph({ rankdir: 'TB' });

    nodes.forEach((el: any) => dagreGraph.setNode(el.id, { width: el.width, height: el.height }));
    edges.forEach((el: any) => dagreGraph.setEdge(el.source, el.target));   
    dagre.layout(dagreGraph);

    let layoutedNodes = nodes.map((el: any) => {
        const nodeWithPosition = dagreGraph.node(el.id);
        el.targetPosition = Position.Top;
        el.sourcePosition = Position.Bottom;

        el.position = {
            x: nodeWithPosition.x - nodeWithPosition.width / 2,
            y: nodeWithPosition.y - nodeWithPosition.height / 2,
        };

        return el;
    });
    
    return layoutedNodes;
}