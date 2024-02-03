#ifndef __Big_array__
#define __Big_array__
/*
	This data structure dedicated for big sized objects
	it has Node that holds data and pointer to next Node(like single linked list)
	It uses static array to hold Nodes ,
	so its cheap to insert and remove elements from array

	The size of array must be initialized at the begining of declaration
	and sincec its using raw array , its capasity cannot be changed
*/
template<class T>

class Big_array
{
	struct Node
	{
		Node* prew = nullptr;
		Node* next = nullptr;
		T data;
	};
public:
	
	class Iterator
	{
	private:
		Node* node = nullptr;
		inline Iterator(Node* node):
		node(node)
		{
		}

		inline void Set(Node* node)
		{
			this->node = node;
		}
	public:

		inline Iterator()
		{
			this->node = this->prew = nullptr;
		};

		inline T* GetData()
		{
			return &this->node->data;
		}

		inline void operator++()
		{
			this->node = this->node->next;
		}

		inline void operator--()
		{
			this->node = this->node->prew;
		}

		inline bool operator==(const Iterator& other) const
		{
			if (this->node == other.node)
				return true;

			return false;
		}
		inline bool operator!=(const Iterator& other) const
		{
			if (this->node == other.node)
				return false;

			return true;
		}
		friend class Big_array;
	};
private:
	int size = 0;//number of elements
	int capacity = 0;

	Node* front;// front will be pointting to firts elemnt in list
	Node* back;// back will be pointing to node next last node, so back is 1 node next to last
	Node* bucket = nullptr;
public:

	Big_array<T>(int capacity)
	{
		capacity++;
		if (capacity < 2)
		{
			throw - 1;// capacity must be at least 2
		}

		this->capacity = capacity-1;
		this->bucket = new Node[capacity];
		this->size = 0;

		for (int i = 0; i < capacity; i++)
		{
			bucket[i].prew = bucket + i - 1;
			bucket[i].next = bucket + i + 1;
		}
		
		bucket[capacity - 1].next = bucket;
		bucket[0].prew = bucket + capacity - 1;

		this->front = bucket;
		this->back = bucket;
	}

	inline int GetSize()
	{
		return this->size;
	}

	inline bool Full()
	{
		return this->size >= this->capacity;
	}

	inline bool Empty()
	{
		return size == 0;
	}

	inline Iterator End()
	{
		return Iterator(this->back->prew);
	}

	inline Iterator Begin()
	{
		return Iterator(this->front);
	}

	inline T* GetData(int index)
	{
		if (index >= size)
			throw - 1;

		Node* p = front;
		while (index--)
			p = p->next;

		return &p->data;
	}

	inline void Remove(Iterator& iter)
	{
		Node* p = iter.node;
		size--;

		if (p == this->front)
		{
			this->front = this->front->next;
			return;
		}
		
		if (p == back->prew)
		{
			back = back->prew;
			return;
		}

		p->prew->next = p->next;

		p->next->prew = p->prew;

		p->next = back->next;

		p->prew = back;

		back->next = p;

		p->data.T::~T();// calling expilicitly destructor of object in array
	}

	template<typename...ARGS>
	inline void EmplaceBack(ARGS&&...args)// creates object at the and of the list. no need to create temorary object
	{
		if (size >= capacity)
			return;
		size++;

		new(&this->back->data)T(args...);

		this->back = this->back->next;
	}

	inline T& Front()
	{
		return front->data;
	}

	~Big_array()
	{
		while (capacity--)
			bucket[capacity].data.T::~T();

		delete[] this->bucket;
	}
};

#endif // !__Big_array__
