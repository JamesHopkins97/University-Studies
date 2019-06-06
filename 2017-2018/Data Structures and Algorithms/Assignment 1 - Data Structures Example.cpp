// includes the iostream library
#include <iostream> 

// groups std for  shorter use of code
using namespace std;

//A class to hold the Nodes
class Node
{
public:
	// constructor for the Node class
	Node(int value, Node* nextptr = NULL, Node* prevptr = NULL, int currentpriority = 0)
	{
		//Sets the variables when the class is called
		this->value = value; // sets to the value that is passed
		next = nextptr; // sets the next node to the nextptr passed to the constructor
		prev = prevptr; // sets the prev node to the prevptr passed to the constructor
		priority = currentpriority; // sets the priority to the currentpriority passed to the constructor
	}

	int getVal(void)
	{
		return this->value; // returns the value of the node
	}

	Node* getNext(void)
	{
		return this->next; // returns the next node
	}

	Node* getPrev(void)
	{
		return this->prev; // returns the previous node
	}

	void setVal(int value)
	{
		this->value = value; // sets the value
	}

	void setPrev(Node* prevptr)
	{
		this->prev = prevptr; // sets the previous node
	}

	void setNext(Node* nextptr)
	{
		this->next = nextptr; //sets the next node
	}

	int getPriority(void)
	{
		return this->priority; //returns the priority of the node
	}


	void setPriority(int priority)
	{
		this->priority = priority; // sets the priority of the node
	}

private:
	Node* next;
	Node* prev;
	int priority;
	int value;
};

class Stack
{
public:
	Stack(void) 
	{ 
		top = 0; // sets top to 0 in the constructor
	}

	~Stack(void) {}

	void Push(int value) // pushes a new Node into the stack
	{
		Node* tmp = new Node(value, top); //points the tmp pointer to a new Node with a value and the top(the nextptr)
		top = tmp; // Sets the top to the tmp pointer to put it at the top of the stack (FIFO)
	}

	Node* NodePop(void)
	{
		Node* tmp = top; //creates a pointer to the node on the top to be deleted
		Node* ToDelete = top; //creates a pointer to the node on the top to be deleted
		this->top = top->getNext(); //sets the top node to be the next node on the stack
		delete ToDelete; //deletes the node that was on the top
		return tmp; //returns the node that was popped off with all of it's data
	}

	int Pop(void) // pops a new 
	{
		int returnTopValue = top->getVal(); //sets the returning int to the value of the top node
		Node* tmp = top; //sets the tmp pointer to the top of the stack
		top = top->getNext(); //sets the top node to the next node on the stack, removing the connection between the variable top and the address of the nodes data
		delete tmp; //deletes the node that is no longer required as it was popped off
		return returnTopValue; //returns the value of the node that was deleted
	}

private:
	Node* top;
};

class Queue
{
public:
	Queue(void)
	{
		front = back = nullptr; // sets the front and back to nullptr
	}

	~Queue(void)
	{
		while (front != nullptr) // uses a while loop to delete the values from the queue when deconstructing
			delete NodeDequeue();
	}

	void Enqueue(int i, int priority = 0)
	{
		Node* tmp = new Node(i, back, nullptr, priority); // creates a pointer to a new Node with assigned values
		back = tmp; // sets the back of the queue to the new node
		if (front == nullptr) // if the front of the queue is a nullptr the front becomes the back
			front = back;
		else 
		{
			tmp = back->getNext(); // sets tmp to the node that comes before the back Node
			tmp->setPrev(back); // sets the node that comes after tmp to be equal to the back node
		}
	}

	int Dequeue(void)
	{
		Node* tmp = NodeDequeue(); // finds the Node to Dequeue
		int returnVal = tmp->getVal(); // sets the returnVal to the returning value of NodeDequeue
		if (front == nullptr) // if the front of the queue is a nullptr it sets the front to the back
			front->setNext(nullptr);
		return returnVal; // returns the value of the dequeued node
	}

protected:

	Node* back;
	Node* front;

private:

	virtual Node* NodeDequeue(void)
	{
		Node* tmp = front; // sets the tmp node that the function returns to the front of the Queue (the value that will be dequeued)
		if (front != nullptr) // if the front is not nullptr it will set it to the value after the front in the queue
		{
			front = front->getPrev(); // sets front to the next (value behind) in the queue
			if (front != nullptr) front->setNext(nullptr); // sets the 'next' value in the queue for the node to nullptr as there is no node in front of the first node
		}
		return tmp; // returns the tmp node that was removed from the queue
	}
};

class Scheduler : public Queue
{
private:

	Node* NodeDequeue(void)
	{
		Node* ToDequeue = nullptr; // sets the Node to dequeue to a nullptr
		Node* tmpArray[100]; // sets a temporary array with a subscript of 100

		int count = 0; // counter to traverse the queue efficiently
		int maxPriority = 0; // sets the max priority to 0 so that anything above it will be prioritised higher

		bool equalNull = false; // sets to false for the while loop to run

		while (!equalNull) // while loop used to prioritise input Nodes
		{
			Node* tmp = front; // sets tmp to the front of the queue
			if (front != nullptr) // if front is not equal to a nullpointer, set front to be the previous Node's value
			{
				front = front->getPrev();
				if (front != nullptr) // if it is still a nullpointer, set front to the next Node's value
					front->setNext(nullptr);

			}


			if(tmp != nullptr) // if tmp is not a nullpointer
			{
				if (tmp->getPriority() >= maxPriority) // if tmp priority is more than the max priority, max priority sets to the tmp priority and ToDequeue is set to tmp.
				{
					if (ToDequeue != nullptr) // if ToDequeue is not a nullptr, tmpArray[count] will contain ToDequeue's value and increment count.
					{
						tmpArray[count] = ToDequeue;
						count++;
					}
					maxPriority = tmp->getPriority();
					ToDequeue = tmp;
				}
				else // else tmmpArray[count] will contain tmp and increment count
				{
					tmpArray[count] = tmp;
					count++;
				}
			}
			else // set equalNull to true, ending the while loop
			{
				equalNull = true;
			}
		}

		for (int i = count - 1; i >= 0; i--) // counting backwards from count, traverse the list and Enqueue Node where it is supposed to be
			Enqueue(tmpArray[i]->getVal(), tmpArray[i]->getPriority());

		return ToDequeue; // return ToDequeue
	}

};