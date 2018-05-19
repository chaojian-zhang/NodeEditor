#pragma once
#include <stdlib.h>
class CanvasNode;
class Canvas;

// A Clean, C Style Linked List Specifically for CanvasNodes; Sorted Using Distance From Far to Close
// Usage: During Each Sorted update, the Canvas will first clear out everything, and add in the things that need to be sorted and rendered

class CanvasNodeSortedLinkedListNode
{
public:
	CanvasNodeSortedLinkedListNode(CanvasNodeSortedLinkedListNode* inPrev, CanvasNode* in, float inIistance);	// Data Reference Only, we are not manageing their lifetime

	CanvasNode* data = NULL;
	CanvasNodeSortedLinkedListNode* prev = NULL;
	CanvasNodeSortedLinkedListNode* next = NULL;
	float currentDistance = 0.f;
};

class CanvasNodeSortedLinkedList
{
	friend class Canvas;
public:
	~CanvasNodeSortedLinkedList();	// Release the space allocated by the listnodes

	void Add(CanvasNode* data, float distance);	// Allocate space for the nodes; also put current to the first if its NULL
	unsigned int Length();	// Return total length of list
	void Clear();	// Clear all existing nodes

	// Selection Related Functions: return the data pointer
	// Order Traversal
	CanvasNode* First();	// Select First Node, return data in first node
	CanvasNode* Next();	// Move selection; Return Next
	// Reverse Order Traversal
	CanvasNode* Last();
	CanvasNode* Prev();	// Return NULL if we have no more to return

protected:
	unsigned int length = 0;	// Current Number of elements
	CanvasNodeSortedLinkedListNode* first = NULL;
	CanvasNodeSortedLinkedListNode* current = NULL;	// Current Selection
	CanvasNodeSortedLinkedListNode* last = NULL;	// Used in reverse order traversal
};