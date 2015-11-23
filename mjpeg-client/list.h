#ifndef _SL_LIST__H
#define _SL_LIST__H

/***************************************************************************
*
*  Project libmjpeg
*
* Copyright (C) 2014-2015, Changer, <dingjinchengyx@163.com>.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/

#include "slconfig.h"

template <class T>
class list
{
public:
	class node {
	public:
		node(const T& e):prev(0),next(0),element(e){}
		node():prev(0),next(0){}
		virtual ~node(){}
	public:
		node* prev;
		node* next;
		T element;
	};
	
	list():m_size(0){
		head = new node();
		tail = new node();
		head->prev = NULL;
		tail->next = NULL;
		head->next = tail;
		tail->prev = head;
	}

	~list() {
		clear();
		delete head;
		delete tail;
	}

	u32 getSize() const {
		return m_size;
	}

	void clear() {
		node* p = head->next;
		while (p != tail)
		{	
			head->next = p->next;
			delete p;
			p = head->next;
			--m_size;
		}
		tail->prev = head;
	}

	bool empty() {
		return m_size == 0;
	}

	void push_back(const T& element) {
		node *p_node = new node(element);
		
		tail->prev->next = p_node;
		p_node->prev = tail->prev;
		tail->prev = p_node;
		p_node->next = tail;
		m_size++;
	}

	void push_front(const T& element) {
		node *p_node = new node(element);

		p_node->next = head->next;
		head->next->prev = p_node;
		head->next  = p_node;
		p_node->prev = head;
		m_size++;
	}

	void insert(const T& element, s32 i) {
		if (i > m_size)
			return;
		node *p_node = new node(element);
		
		node* p  = head;
		for (int index = 0; index < i; index++)
			p = p->next;
		p_node->next = p->next;
		p->next->prev = p_node;
		p_node->prev = p;
		p->next = p_node;
		m_size++;
	}

	node* begin() const{
		return head->next;
	}

	node* end() const {
		return tail;
	}

	void erase(s32 i) {
		if (i >=(s32)m_size)
			return;
		node* p = head->next;
		for (int index = 0; index < i;index++)
			p = p->next;
		
		erase(p);
	}

	void erase(node* p) {
		p->prev->next = p->next;
		p->next->prev = p->prev;
		delete p;
		m_size--;
	}
private:
	node* head;
	node* tail;
	u32 m_size;
};

#endif