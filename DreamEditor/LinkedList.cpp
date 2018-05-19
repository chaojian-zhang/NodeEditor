#include "LinkedList.h"

LinkedList::~LinkedList()
{
	current = first;
	while (current != NULL)
	{
		ListNode* next = current->next;
		delete current;
		current = next;
	}
}

void LinkedList::Add(void* data)
{
	// Add a new ndoe
	if (last != NULL)
	{
		last->next = new ListNode(data);
		last = last->next;

		// Notice here when we add new nodes, current is not automatically moving, so it will say NULL until we call First/Last/Select
	}
	else
	{
		last = new ListNode(data);
		current = last;
		first = last;
	}

	length++;
}

unsigned int LinkedList::Length()
{
	return length;
}

void LinkedList::Clear()
{
	// Clear Nodes
	ListNode* temp = first;
	ListNode* tempNext;
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

void* LinkedList::First()
{
	if (first != NULL)
	{
		current = first;
		return first->data;
	}
	return NULL;
}

void* LinkedList::Last()
{
	// By Design, Last will not be NULL in any case, unless,  the list has just be created, with no elements in it yet

	if (last)
	{
		current = last;
		return last->data;
	}
	else
	{
		return NULL;
	}
}

// Return the data of next if available, also forward current one step
void* LinkedList::Next()
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

void* LinkedList::Select(unsigned int index)
{
	current = first;
	unsigned int i = 0;
	while (current && i < index)
	{
		current = current->next;
		i++;
	}

	if (current)
	{
		return current->data;
	}
	return NULL;
}

void* LinkedList::Delete(unsigned int index)
{
	// Iterate and find the node
	ListNode* prev = NULL;	// Used to keep track backwards
	current = first;
	unsigned int i = 0;
	while (current && i < index)
	{
		// Keep Current
		prev = current;

		// Check Next
		current = current->next;
		i++;
	}

	if (current)
	{
		// Keep the data
		void* data = current->data;

		// Reconnect
		if (prev)
		{
			prev->next = current->next;
		}
		else
		{
			first = current->next;
		}

		// Delete current
		delete current;
		current = prev;
		length--;

		// Return the data
		return data;
	}
	return NULL;
}

// List node
ListNode::ListNode(void* in)
{
	data = in;
}