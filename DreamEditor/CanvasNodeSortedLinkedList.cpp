#include "CanvasNodeSortedLinkedList.h"
#include "CanvasNodeSortedLinkedList.h"

// List node
CanvasNodeSortedLinkedListNode::CanvasNodeSortedLinkedListNode(CanvasNodeSortedLinkedListNode* inPrev, CanvasNode* in, float inIistance)
	:currentDistance(inIistance), prev(inPrev)
{
	data = in;
}

// Linked List
CanvasNodeSortedLinkedList::~CanvasNodeSortedLinkedList()
{
	current = first;
	while (current != NULL)
	{
		CanvasNodeSortedLinkedListNode* next = current->next;
		delete current;
		current = next;
	}
}

void CanvasNodeSortedLinkedList::Add(CanvasNode* data, float distance)
{
	// Add a new ndoe
	if (first != NULL)
	{
		// Search through the list until we find someone bigger than us
		CanvasNodeSortedLinkedListNode* temp = first;
		CanvasNodeSortedLinkedListNode* prev = temp;
		while (temp && temp->currentDistance > distance)
		{
			prev = temp;
			temp = temp->next;
		}

		// If temp is NULL or temp's distance is smaller than ours
		if (temp == NULL)
		{
			prev->next = new CanvasNodeSortedLinkedListNode(prev, data, distance);
			last = prev->next;
		}
		else
		{
			if (temp == first)
			{
				first = new CanvasNodeSortedLinkedListNode(NULL, data, distance);
				first->next = temp;
			}
			else
			{
				prev->next = new CanvasNodeSortedLinkedListNode(prev, data, distance);
				temp->prev = prev->next;
				prev->next->next = temp;
			}
		}

		// Notice we didn't change Current pointer, since it might be used by others	
	}
	else // The list is empty
	{
		first = new CanvasNodeSortedLinkedListNode(NULL, data, distance);
		last = current = first;
	}

	length++;
}

unsigned int CanvasNodeSortedLinkedList::Length()
{
	return length;
}

void CanvasNodeSortedLinkedList::Clear()
{
	// Clear Nodes
	CanvasNodeSortedLinkedListNode* temp = first;
	CanvasNodeSortedLinkedListNode* tempNext;
	while (temp != NULL)
	{
		tempNext = temp->next;
		delete temp;
		temp = tempNext;
	}

	// Reset State
	length = 0;
	first = current = last = NULL;
}

CanvasNode* CanvasNodeSortedLinkedList::First()
{
	if (first != NULL)
	{
		current = first;
		return first->data;
	}
	return NULL;
}

// Return the data of next if available, also forward current one step
CanvasNode* CanvasNodeSortedLinkedList::Next()
{
	if (current == NULL)
	{
		return NULL;

	}
	else
	{
		if (current->next)
		{
			current = current->next;
			return current->data;
		}
		else
		{
			current = NULL;
			return NULL;
		}
	}
}

// Reverse Order Traversal
CanvasNode* CanvasNodeSortedLinkedList::Last()
{
	if (first != NULL)
	{
		current = last;
		return last->data;
	}
	return NULL;
}

CanvasNode* CanvasNodeSortedLinkedList::Prev()
{
	if (current == NULL)
	{
		return NULL;

	}
	else
	{
		if (current->prev)
		{
			current = current->prev;
			return current->data;
		}
		else
		{
			current = NULL;
			return NULL;
		}
	}
}