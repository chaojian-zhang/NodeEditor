#pragma once
#include <stdlib.h>

// A Clean, C Style Linked List

class ListNode
{
public:
	ListNode(void* in);	// Keep Data reference
	// The linklist, hense the listNode, doesn't and never destroy the actual data

	void* data = NULL;
	ListNode* next = NULL;
};

class LinkedList
{
public:
	~LinkedList();	// Release the space allocated by the listnodes

	void Add(void* data);	// Allocate space for the nodes; also put current to the first if its NULL
	unsigned int Length();	// Return total length of list
	void Clear();	// Clear all existing nodes

	// Selection Related Functions: return the data pointer
	void* First();	// Select First Node, return data in first node
	void* Last();// Select Last Node, return data in last node
	void* Next();	// Move selection; Return Next
	void* Select(unsigned int index);	// Directly select a node using index, also return the node being selected
	// The return value can be NULL, if the List doesn't have any element yet
	void* Delete(unsigned int index);	// Delete the node, return the data the node was holding, notice we do not delete the data, just the node
	// If index doesn't exist, return NULL

protected:
	unsigned int length = 0;	// Current Number of elements
	ListNode* first = NULL;
	ListNode* current = NULL;	// Current Selection
	ListNode* last = NULL;
};

// Usage Tip: The user of the linked list first create a list, most of the time statically. Then whenever he needs to add an element to it, 
// he first allocate space manually for the element, then call list.Add(), and so on.
// When he is ready to dispose the list, he shall call list.Next() untill he gets NULL, and release all the data there manually.
// Finally when the list itself is deleted, the list only release the space occupied by the list object, since all data now are already released by the user.