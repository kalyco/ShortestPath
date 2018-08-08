#include <fstream>
#include <set>
#include <utility> // for pair
#include "Graph.h"
#include "Heap.h"

void Graph::outputNodes(std::ostream& s) const
{
  if (mNodes.size() == 0) {
    return;
  }
  for (const SLLNode<Node*>* curr = mNodes.head();
       curr != NULL; curr = curr->next()) {
    Node* n = curr->value();
    s << n->id() 
      << " [label="
      << "\""
      << n->id()
      << " (" << n->shortestDistance() << ")"
      << "\"" 
      << "]"
      << " [shape=box];"
      << std::endl;
  }
}

void Graph::outputEdges(std::ostream& s) const
{
  if (mEdges.size() == 0) {
    return;
  }
  for (const SLLNode<Edge*>* curr = mEdges.head();
       curr != NULL; curr = curr->next()) {
    Edge* e = curr->value();
    s << e->source()->id() << " -> " << e->sink()->id() 
      << " [label="
      << "\""
      << e->name()
      << " (" << e->weight() << ")"
      << "\"" 
      << "];"
      << std::endl;
  }
}

void Graph::output(std::ostream& s) const
{
  ofstream ofs("graph.dot");
  streambuf* strm_buffer = s.rdbuf(); // save output buffer of the stream
  s.rdbuf (ofs.rdbuf()); // redirect ouput into the file
  s << "digraph{" << std::endl;
  outputNodes(s);
  outputEdges(s);
  s << "}" << std::endl;
  s.rdbuf (strm_buffer);
}

void Graph::initializeShortestPath(int aStartNodeId)
{
  for (const SLLNode<Node*>* curr = mNodes.head();
    curr != NULL; curr = curr->next()) {
    Node* currNode = curr->value();
    currNode->setShortestDistance(INFINITY);
    currNode->setShortestParent(NULL);
  }
  Node* startNode = find(aStartNodeId);
  startNode->setShortestDistance(0);
}

void Graph::relax(Node* u, Edge* E) {
  Node* v = E->sink();
  int w = u->shortestDistance() + E->weight();
  if (v->shortestDistance() > w) {
    v->setShortestDistance(w);
    v->setShortestParent(E);
  }
}

void Graph::computeShortestPaths(int aNodeId)
{
  initializeShortestPath(aNodeId);
  Heap<Node*> heap(numNodes());
  for (const SLLNode<Node*>* curr = mNodes.head();
    curr != NULL; curr = curr->next()) {
    Node* currNode = curr->value();
    heap.insertIgnoringHeapOrder(currNode);
  }

  heap.bottomUpMinHeap();

  while (!heap.isEmpty()) {
    Node* u = heap.removeMin();
    SLinkedList<Edge*>& edges = u->getEdges();
    for (const SLLNode<Edge*>* curr = edges.head(); curr != NULL; curr = curr->next()) {
      Edge * E = curr->value();
      relax(u, E);
    }  
    heap.bottomUpMinHeap();
  }
}

void Graph::printDirectionsTo(Node* aNode) const
{
  Edge* parentEdge = aNode->shortestParent();
  if (parentEdge == NULL) {
    return;
  }
  Node* source = parentEdge->source();
  printDirectionsTo(source);
  
  cout << "Drive for " << parentEdge->weight() << " meters";
  cout << " on " << parentEdge->name() << endl;
}

Graph::~Graph()
{
  SLLNode<Edge*>* curr = mEdges.head();
  while (curr != NULL) {
    Edge* temp = curr->value();
    curr = curr->next();
    delete temp;
  }
  SLLNode<Node*>* curr2 = mNodes.head();
  while (curr2 != NULL) {
    Node* temp = curr2->value();
    curr2 = curr2->next();
    delete temp;
  }
}

std::ostream& operator<<(std::ostream& s, const Graph& g)
{
  g.output(s);
  return s;
}
