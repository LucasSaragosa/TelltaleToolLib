#pragma once

// Not mine. Modified though. Originally: 
// https://github.com/wc-duck/intrusive_LinkedList

#if defined( _INTRUSIVE_LIST_ASSERT_ENABLED )
#  if !defined( _INTRUSIVE_LIST_ASSERT )
#    include <assert.h>
#    define _INTRUSIVE_LIST_ASSERT( cond, ... ) assert( cond )
#  endif
#else
#  if !defined( _INTRUSIVE_LIST_ASSERT )
#    define _INTRUSIVE_LIST_ASSERT( ... )
#  endif
#endif

#include <type_traits>

template <typename T>
struct ListNode
{
	ListNode()
		: next(0x0)
		, prev(0x0)
	{}

	T* next;
	T* prev;
};

template<typename T> bool _DefaultLess(T* lhs, T* rhs){
	return *lhs < *rhs;
}

/**
 * intrusive double linked LinkedList.
 */
template <typename T>
class LinkedList
{
	T* head_ptr;
	T* tail_ptr;

public:

	static_assert(std::is_base_of_v<ListNode<T>, T>, "T must be a subclass of ListNode<T>");

	LinkedList()
		: head_ptr(0x0)
		, tail_ptr(0x0)
	{}
	~LinkedList() { clear(); }

	int count() {
		T* h = head();
		int counter = 0;
		while(h){
			counter++;
			h = h->next;
		}
		return counter;
	}

	/**
	 * insert item at the head of LinkedList.
	 * @param item item to insert in LinkedList.
	 */
	void insert_head(T* elem)
	{
		ListNode<T>* node = elem;

		_INTRUSIVE_LIST_ASSERT(node->next == 0x0);
		_INTRUSIVE_LIST_ASSERT(node->prev == 0x0);

		node->prev = 0;
		node->next = head_ptr;

		if (head_ptr != 0x0)
		{
			ListNode<T>* last_head = head_ptr;
			last_head->prev = elem;
		}

		head_ptr = elem;

		if (tail_ptr == 0x0)
			tail_ptr = head_ptr;
	}

	/**
	 * insert item at the tail of LinkedList.
	 * @param item item to insert in LinkedList.
	 */
	void insert_tail(T* item)
	{
		if (tail_ptr == 0x0)
			insert_head(item);
		else
		{
			ListNode<T>* tail_node = tail_ptr;
			ListNode<T>* item_node = item;
			_INTRUSIVE_LIST_ASSERT(item_node->next == 0x0);
			_INTRUSIVE_LIST_ASSERT(item_node->prev == 0x0);
			tail_node->next = item;
			item_node->prev = tail_ptr;
			item_node->next = 0x0;
			tail_ptr = item;
		}
	}

	/**
	 * insert item in LinkedList after other LinkedList item.
	 * @param item item to insert in LinkedList.
	 * @param in_LinkedList item that already is inserted in LinkedList.
	 */
	void insert_after(T* item, T* in_LinkedList)
	{
		ListNode<T>* node = item;
		ListNode<T>* in_node = in_LinkedList;

		if (in_node->next)
		{
			ListNode<T>* in_next = in_node->next;
			in_next->prev = item;
		}

		node->next = in_node->next;
		node->prev = in_LinkedList;
		in_node->next = item;

		if (in_LinkedList == tail_ptr)
			tail_ptr = item;
	}

	/**
	 * insert item in LinkedList before other LinkedList item.
	 * @param item item to insert in LinkedList.
	 * @param in_LinkedList item that already is inserted in LinkedList.
	 */
	void insert_before(T* item, T* in_LinkedList)
	{
		ListNode<T>* node = item;
		ListNode<T>* in_node = in_LinkedList;

		if (in_node->prev)
		{
			ListNode<T>* in_prev = in_node->prev;
			in_prev->next = item;
		}

		node->next = in_LinkedList;
		node->prev = in_node->prev;
		in_node->prev = item;

		if (in_LinkedList == head_ptr)
			head_ptr = item;
	}

	/**
	 * remove the first item in the LinkedList.
	 * @return the removed item.
	 */
	T* remove_head()
	{
		T* ret = head();
		remove(head());
		return ret;
	}

	/**
	 * remove the last item in the LinkedList.
	 * @return the removed item.
	 */
	T* remove_tail()
	{
		T* ret = tail();
		remove(tail());
		return ret;
	}

	/**
	 * remove item from LinkedList.
	 * @param item the element to remove
	 * @note item must exist in LinkedList!
	 */
	void remove(T* item)
	{
		ListNode<T>* node = item;

		T* next = node->next;
		T* prev = node->prev;

		ListNode<T>* next_node = next;
		ListNode<T>* prev_node = prev;

		if (item == head_ptr) head_ptr = next;
		if (item == tail_ptr) tail_ptr = prev;
		if (prev != 0x0) prev_node->next = next;
		if (next != 0x0) next_node->prev = prev;

		node->next = 0x0;
		node->prev = 0x0;
	}

	/**
	 * return first item in LinkedList.
	 * @return first item in LinkedList or 0x0 if LinkedList is empty.
	 */
	T* head() { return head_ptr; }

	/**
	 * return last item in LinkedList.
	 * @return last item in LinkedList or 0x0 if LinkedList is empty.
	 */
	T* tail() const { return tail_ptr; }

	/**
	 * return next element in LinkedList after argument element or 0x0 on end of LinkedList.
	 * @param item item to get next element in LinkedList for.
	 * @note item must exist in LinkedList!
	 * @return element after item in LinkedList.
	 */
	T* next(T* item)
	{
		ListNode<T>* node = item;
		return node->next;
	}

	/**
	 * return next element in LinkedList after argument element or 0x0 on end of LinkedList.
	 * @param item item to get next element in LinkedList for.
	 * @note item must exist in LinkedList!
	 * @return element after item in LinkedList.
	 */
	const T* next(const T* item)
	{
		const ListNode<T>* node = item;
		return node->next;
	}

	/**
	 * return previous element in LinkedList after argument element or 0x0 on start of LinkedList.
	 * @param item item to get previous element in LinkedList for.
	 * @note item must exist in LinkedList!
	 * @return element before item in LinkedList.
	 */
	T* prev(T* item)
	{
		ListNode<T>* node = item;
		return node->prev;
	}

	/**
	 * return previous element in LinkedList after argument element or 0x0 on start of LinkedList.
	 * @param item item to get previous element in LinkedList for.
	 * @note item must exist in LinkedList!
	 * @return element before item in LinkedList.
	 */
	const T* prev(const T* item)
	{
		const ListNode<T>* node = item;
		return node->prev;
	}

	/**
	 * clear queue.
	 */
	void clear()
	{
		while (!empty())
			remove(head());
	}

	/**
	 * check if the LinkedList is empty.
	 * @return true if LinkedList is empty.
	 */
	bool empty() { return head_ptr == 0x0; }

	void sortedInsert(T** head_ref, T* newNode, bool(*less)(T*,T*))
	{
		T* current;

		// if list is empty
		if (*head_ref == NULL)
			*head_ref = newNode;

		// if the node is to be inserted at the beginning
		// of the doubly linked list
		else if (less(newNode, (*head_ref))) {//if ((*head_ref)->data >= newNode->data)
			newNode->next = *head_ref;
			newNode->next->prev = newNode;
			*head_ref = newNode;
		}

		else {
			current = *head_ref;

			// locate the node after which the new node
			// is to be inserted
			while (current->next != NULL &&
				less(current->next,newNode))
				current = current->next;

			/*Make the appropriate links */

			newNode->next = current->next;

			// if the new node is not inserted
			// at the end of the list
			if (current->next != NULL)
				newNode->next->prev = newNode;

			current->next = newNode;
			newNode->prev = current;
		}
	}

	// function to sort a doubly linked list using insertion sort
	void insertionSort()
	{
		// Initialize 'sorted' - a sorted doubly linked list
		T* sorted = NULL;

		// Traverse the given doubly linked list and
		// insert every node to 'sorted'
		T* current = head_ptr;
		while (current != NULL) {

			// Store next for next iteration
			T* next = current->next;

			// removing all the links so as to create 'current'
			// as a new node for insertion
			current->prev = current->next = NULL;

			// insert current in 'sorted' doubly linked list
			sortedInsert(&sorted, current, &_DefaultLess<T>);

			// Update current
			current = next;
		}

		// Update head_ref to point to sorted doubly linked list
		head_ptr = sorted;

		//update tailptr
		T* t = head_ptr;
		while(t){
			if (!t->next)
				tail_ptr = t;
			t = t->next;
		}
	}


};