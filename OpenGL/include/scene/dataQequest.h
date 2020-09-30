

#ifndef _DATA_QE_QUEST_H_
#define _DATA_QE_QUEST_H_

template<class T>
class Node
{
public:
	T* data_ = nullptr;
	struct Node<T> *pre_ = nullptr;
	struct Node<T> * next_ = nullptr;
};

template<class T>
class DataQequest
{
public:
	void					addRequest(T * request);
	T*						firstRequest()const;
	void					removeFistNode();
	bool					empty()const;
	void					removeRequest(T * request);

private:
	void					removeNode(Node<T>*);
	void					pushBack(Node<T>*);
private:
	Node<T>* head_ = nullptr;
	Node<T>* back_ = nullptr;
};


template<class T>
void DataQequest<T>::pushBack(Node<T>*node)
{
	if (back_)
	{
		back_->next_ = node;
		node->pre_ = back_;
		back_ = node;
	}
	else
	{
		back_ = head_ = node;
	}
}

template<class T>
void DataQequest<T>::removeNode(Node<T>*node)
{
	if (node)
	{
		if (node->pre_) 
		{
			node->pre_->next_ = node->next_;
			if (node->next_)
			{
				node->next_->pre_ = node->pre_;
			}
			else
			{
				//node is back
				back_ = node->pre_;
			}
		}
		else
		{
			if (node->next_)
			{
				node->next_->pre_ = node->pre_;
			}
			else
			{
				head_ = back_ = nullptr;
			}
		}

		node->pre_ = node->next_ = nullptr;
	}
}

template<class T>
void DataQequest<T>::removeRequest( T * request)
{
	struct Node<T>* t = head_;
	while (t)
	{
		if (t->data_ != request) t = t->next_;
	}
	if (t)
	{
		t->data_->isNode_ = false;
		removeNode(t);
	}
}


template<class T>
void DataQequest<T>::removeFistNode()
{
	head_->data_->isNode_ = false;
	removeNode(head_);
}

template<class T>
T*  DataQequest<T>::firstRequest() const
{
	if (head_)
	{
		return head_->data_;
	}
	return nullptr;
}

template<class T>
bool DataQequest<T>::empty() const
{
	return (head_ == back_)&& (head_ == nullptr);
}

template<class T>
void DataQequest<T>::addRequest(T * request)
{
	struct Node<T>* newNode = new Node<T>;
	newNode->data_ = request;
	newNode->data_->isNode_ = true;
	pushBack(newNode);
}

#endif
