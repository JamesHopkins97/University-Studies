//15005216 - James Hopkins

#include <iostream>
#include <vector>
#include <string>
using namespace std;

#pragma region DEFAULT
// The different states of the machine, all states have different transition options
enum state{ Out_Of_Poptart, No_Credit, Has_Credit, Dispenses_Poptart };
enum stateParameter{ No_Of_Poptarts, Credit, Cost_Of_Poptart };

// The StateContext class holds a state object. Whenever the state changes in this class the state object also changes
class StateContext;

// This class holds the current state, the current state could include Out_Of_Poptart, No_Credit, Has_Credit or Dispenses_Poptart
class State
{
protected:
	
	StateContext* CurrentContext; // References StateContext

public:
	State(StateContext* Context)
	{
		CurrentContext = Context; //Sets CurrentContext to the passed in variable of Context
	}
	virtual ~State(void) {}
	virtual void transition(void) {}
};

class StateContext
{
protected:
	// An instance of the current state is set here to allow the state to access the state parameters.
	// The states are be held in a vector
	State* CurrentState = nullptr;
	int stateIndex = 0;
	
	vector<State*> availableStates; // The vector holding the different available states
	vector<int> stateParameters; // All state parameters are held in this vector 

public:
	virtual ~StateContext(void); // Destructs the class to avoid memory leaks
	virtual void setState(state newState); // This sets the first state in the state index
	virtual int getStateIndex(void); // Returns the number of the index state
	virtual void setStateParam(stateParameter SP, int value); // Sets the state parameter to the value of value
	virtual int getStateParam(stateParameter SP); // Returns the value of the state parameter
};

StateContext::~StateContext(void)
{
	for (int i = 0; i <this->availableStates.size(); i++) 
		delete this->availableStates[i]; // Clears the available states
	this->availableStates.clear(); // Clears the available states
	this->stateParameters.clear(); // Clears the stateParameters
}

// Assigns the current state to a new state, if it's not possible the current state will assign to transition
void StateContext::setState(state newState)
{
	this->CurrentState = availableStates[newState];
	this->stateIndex = newState;
	this->CurrentState->transition();
}

// Gets the current state being used in State
int StateContext::getStateIndex(void)
{
	return this->stateIndex;
}

// Sets the stateParameter to the value of value
void StateContext::setStateParam(stateParameter SP, int value)
{
	this->stateParameters[SP] = value;
}

// Returns the value of the stateParameter being used.
// For example, returning the value of stateParameter Credit will return the amount of credit the user
int StateContext::getStateParam(stateParameter SP)
{
	return this->stateParameters[SP];
}

//For each state, a defined error message has been defined. This is to prevent the program from crashing when there are errors passing null valuies
class Transition
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; }
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }
	virtual bool addPoptart(int) { cout << "Error!" << endl; return false; }
	virtual bool dispense(void) { cout << "Error!" << endl; return false; }
};

// Allows for error messages to be displayed if the state hasn't been declared
class PoptartState: public State, public Transition
{
public:
	PoptartState(StateContext* Context) : State(Context) {}
};

// This is the class for the Out_Of_Poptart state
class OutOfPoptart: public PoptartState
{
public:
	OutOfPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// This is the class for the No_Credit state
class NoCredit: public PoptartState
{
public:
	NoCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// This is the class for the Has_Credit state
class HasCredit: public PoptartState
{
public:
	HasCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// This is the class for the Dispenses_Poptart state
class DispensesPoptart: public PoptartState
{
public:
	DispensesPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

#pragma endregion

// The class that contains the product information, like the cost
class Product
{
	friend class Filling;
protected:
	string product_description; // The product description of the product
	int itemCost = 0; // The cost of the product, this is used in Cost_Of_Poptart
public:
	virtual void consume(void); // Removes the product
	virtual int cost(void); // 
	virtual string description(void);
};


// Other classes inherit from this class instead of product, which allows for easier extension of products
class Poptart: public Product
{
public:
	Poptart(void)
	{
		this->product_description = "Poptart";
		this->itemCost = 50;
	}
};

// The class that implements the product into the states, all state changes can be done here
class Poptart_Dispenser: public StateContext, public Transition
{
	// Allows DispensesPoptart and HasCredit access to the protected information in this 
	friend class DispensesPoptart; //Allows Dispensed poptart to be accessed through Poptart_Dispenser
	friend class HasCredit; // Allows HasCredit to be accessed through PoptartDispenser
private:
	PoptartState* PoptartCurrentState = nullptr;
	bool itemDispensed = false; //indicates whether a product is there to be retrieved
	Product* DispensedItem = nullptr;
	bool itemRetrieved = false; //indicates whether a product has been retrieved
public:
	Poptart_Dispenser(int inventory_count);
	~Poptart_Dispenser(void);
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
	Product* getProduct(void);
	virtual void setStateParam(stateParameter SP, int value);
	virtual int getStateParam(stateParameter SP);
};

// If the purchase wasn't completed then delete the item
// This is to avoid memory leaks if the user stops the purchase
Poptart_Dispenser::~Poptart_Dispenser(void)
{
	if (!this->itemRetrieved)
	{
		delete this->DispensedItem;
	}
}

// Calls insertMoney from the current state and sets it to the amount of money input
// If the current state is Out_Of_Poptarts then return an error, however if it is set to No_Credit it will accept the money and update the stateParameter Credit
bool Poptart_Dispenser::insertMoney(int money)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->insertMoney(money);
}

// Sets the poptartcurrent state to the current state from PoptartState
// Calls the makeSelection from current state and sets it to option
bool Poptart_Dispenser::makeSelection(int option)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->makeSelection(option);
}

// Sets the poptart current state to the current state from PoptartState
// Calls the moneyRejected from current state and reject money
bool Poptart_Dispenser::moneyRejected(void)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->moneyRejected();
}

// Sets the poptart current state to the current state from PoptartState
// Call addPoptart from the current state and set it to number
bool Poptart_Dispenser::addPoptart(int number)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->addPoptart(number);
}

// Sets the poptart current state to the current state from PoptartState
// Call Dispense from current state and dispense the poptart
bool Poptart_Dispenser::dispense(void)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->dispense();
}

// getProduct checks to see if the item has dispensed correctly
// If item has been dispended then itemRetrieved is true and dispensed item is deleted in Dispense
// If itemRetrieved is false when poptartDispenser deconstructs it will be deleted, meaning an error would occur and the purchase will not complete
Product* Poptart_Dispenser::getProduct(void)
{
	if (this->itemDispensed)
	{
		this->itemDispensed = false;
		this->itemRetrieved = true;
		return this->DispensedItem;
	}

	return nullptr;
}

// Sets the stateParameter to No_Of_Poptarts and Credit
void Poptart_Dispenser::setStateParam(stateParameter SP, int value)
{
	if (SP == Cost_Of_Poptart) return;
	this->stateParameters[SP] = value;
}

// Returns the values of No_Of_Poptarts and Credit
// cost() is called as Cost_Of_Poptart is defined in that function
// This returns the cost of the object created (If it's a chocolate base it will return 200)
int Poptart_Dispenser::getStateParam(stateParameter SP)
{
	if (SP == Cost_Of_Poptart)
	{
		if (DispensedItem == nullptr) return 0;
		return DispensedItem->cost();
	}
	return this->stateParameters[SP];
}

// Returns the cost of the currently selected product
int Product::cost(void)
{
	return this->itemCost;
}

// Returns the description of the currently selected product
string Product::description(void)
{
	return this->product_description;
}

// Removes the product
void Product::consume(void)
{
	delete this;
}


// Adds the states and stateParameters into the correct vectors as instances
// This allows access and changes from Poptart_Dispenser
Poptart_Dispenser::Poptart_Dispenser(int inventory_count)
{
	this->availableStates.push_back(new OutOfPoptart(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesPoptart(this));
	this->stateParameters.push_back(No_Of_Poptarts);
	this->stateParameters.push_back(Credit);
	this->stateParameters.push_back(Cost_Of_Poptart);

	// Sets the default state to Out_Of_Popart
	this->setState(Out_Of_Poptart);
	if (inventory_count > 0)
		this->addPoptart(inventory_count);
}

// Classes for each poptart base
#pragma region BaseSelectionOptions
// Class to contain the cost and description for the plain poptart class
class Plain : public Poptart
{
public:
	Plain(void)
	{
		itemCost = 100;
		product_description = "Plain base";
	}
};

// Class to contain the cost and description for the Spicy poptart class
class Spicy : public Poptart
{
public:
	Spicy(void)
	{
		itemCost = 150;
		product_description = "Spicy base";
	}
};

// Class to contain the cost and description for the Chocolate poptart class
class Chocolate : public Poptart
{
public:
	Chocolate(void)
	{
		itemCost = 200;
		product_description = "Chocolate base";
	}
};

// Class to contain the cost and description for the Coconut poptart class
class Coconut : public Poptart
{
public:
	Coconut(void)
	{
		itemCost = 200;
		product_description = "Coconut base";
	}
};

// Class to contain the cost and description for the Fruity poptart class
class Fruity : public Poptart
{
public:
	Fruity(void)
	{
		itemCost = 200;
		product_description = "Fruity base";
	}
};
#pragma endregion

// Set of options used for the Out_Of_Poptart state
#pragma region OutOfPoptartStateSettings

// If user inputs money whilst there are no poptarts, reject the money
bool OutOfPoptart::insertMoney(int money)
{
	cout << "Error: No poptarts left in system!" << endl;
	this->moneyRejected();
	return false;
}

// If makeSelection is called with OutOfPoptart is the state, display error message
bool OutOfPoptart::makeSelection(int option)
{
	cout << "Error! No poptarts left in system!" << endl;
	return false;
}

// if moneyRejected is called with OutOfPoptart as the state, display rejecting money message
bool OutOfPoptart::moneyRejected(void)
{
	cout << "Rejecting money" << endl;
	return false;
}

// Adding poptarts to the machine displays confirmation message
// Changes the state to No_Credit
bool OutOfPoptart::addPoptart(int number)
{
	cout << "Stocked up. Currently " << number << " in the system." << endl;
	this->CurrentContext->setStateParam(No_Of_Poptarts, number);
	this->CurrentContext->setState(No_Credit);
	return true;
}

// Dispense returns an error message as there are no poptarts to be dispensed
bool OutOfPoptart::dispense(void)
{
	cout << "Error! Nothing to dispense!" << endl;
	return false;
}

#pragma endregion

// Set of options used for the No_Credit state
#pragma region NoCreditState

// Sets the current state to Has_Credit and adds the input money into Credit
bool NoCredit::insertMoney(int money)
{
	cout << "Money accepted, current money is " << money << "." << endl;
	this->CurrentContext->setStateParam(Credit, money);
	this->CurrentContext->setState(Has_Credit);
	return true;
}

// makeSelection will display an error message since there's no credit
bool NoCredit::makeSelection(int option)
{
	cout << "Error! Selection cannot be made with no credit" << endl;
	return false;
}

// moneyRejected can't reject money since there's no credit
bool NoCredit::moneyRejected(void)
{
	cout << "Error! Can't dispense money with no credit" << endl;
	return false;
}

// Cannot add poptarts as there may already be poptarts in the system
bool NoCredit::addPoptart(int number)
{
	cout << "Error! Poptarts have already been added." << endl;
	return false;
}

// Lack of credit means the machine cannot dispense
bool NoCredit::dispense(void)
{
	cout << "Error! insufficient Funds." << endl;
	return false;
}

#pragma endregion

// Set of options used for the Has_Credit state
#pragma region HasCreditState

// Money can be increased by user
// This adds the input money to the money in the system already
bool HasCredit::insertMoney(int money)
{
	cout << "Money accepted! Adding " << money << " makes a total of " << this->CurrentContext->getStateParam(Credit) + money << "!" << endl;
	this->CurrentContext->setStateParam(Credit, this->CurrentContext->getStateParam(Credit) + money);
	return true;
}

// Dispenses item of a product depending on the option code user inputs
bool HasCredit::makeSelection(int option)
{
	// Using a switch statement to have individual responses to user input
	// If an incorrect code has been input it returns a false
	switch (option)
	{
	// Allows HasCredit access to PoptartDispenser class information
	// Also allows DispensedItem to be called and creates a Plain base for the poptart
	case 1:
		((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Plain();
		cout << "Plain base selected!" << endl;
		break;

	// Allows HasCredit access to PoptartDispenser class information
	// Also allows DispensedItem to be called and creates a Spicy base for the poptart
	case 2:
		((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Spicy();
		cout << "Spicy base selected!" << endl;
		break;

	// Allows HasCredit access to PoptartDispenser class information
	// Also allows DispensedItem to be called and creates a Chocolate base for the poptart
	case 4:
		((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Chocolate();
		cout << "Chocolate base selected!" << endl;
		break;

	// Allows HasCredit access to PoptartDispenser class information
	// Also allows DispensedItem to be called and creates a Coconut base for the poptart
	case 8:
		((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Coconut();
		cout << "Coconut base selected!" << endl;
		break;

	// Allows HasCredit access to PoptartDispenser class information
	// Also allows DispensedItem to be called and creates a Fruity base for the poptart
	case 16:
		((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Fruity();
		cout << "Fruity base selected!" << endl;
		break;

	// if an invalid code is input, display an error
	default:
		cout << "No valid poptart base selected!" << endl;
		return false;
		break;

	}

	// if there's not enough money refuse to dispense, delete the poptart from the system
	if (this->CurrentContext->getStateParam(Credit) < this->CurrentContext->getStateParam(Cost_Of_Poptart))
	{
		delete ((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem;
		cout << "Insufficient funds! Removing poptart" << endl;
	}

	// else there will be enough for a poptart 
	else
	{
		// Takes the cost of a poptart away from the current amount of money input by the user before dispensing
		this->CurrentContext->setStateParam(Credit, this->CurrentContext->getStateParam(Credit) - this->CurrentContext->getStateParam(Cost_Of_Poptart));
		this->CurrentContext->setState(Dispenses_Poptart);
		cout << endl;
	}
	return true;
}

// Current amount of money in system is returned to the user, resetting Credit to 0
// changes current state to NoCredit
bool HasCredit::moneyRejected(void)
{
	cout << "Releasing current money!" << endl;
	this->CurrentContext->setStateParam(Credit, 0);
	this->CurrentContext->setState(No_Credit);
	return true;
}

// No need to add poptarts so display error
bool HasCredit::addPoptart(int number)
{
	cout << "Error! Already full of poptarts!" << endl;
	return false;
}

// Can't dispense as no selection has been made, display error
bool HasCredit::dispense(void)
{
	cout << "Error! Selection must be made for a poptart to be dispensed." << endl;
	return false;
}

#pragma endregion

// Set of options for the Dispenses_Poptart
#pragma region DispensesPoptartState


bool DispensesPoptart::insertMoney(int money)
{
	// Doesn't accept money as the item is dispensing, display error
	cout << "No need for more money as poptart is ready for dispensing." << endl;
	this->moneyRejected();
	return false;
}

// The poptart must have been selected already for the poptart to be dispensed, display error
bool DispensesPoptart::makeSelection(int option)
{
	cout << "Selection already made." << endl;
	return false;
}

// The change from the money input by the user will have already been ejected from the system, display message
bool DispensesPoptart::moneyRejected(void)
{
	cout << "Ejecting change." << endl;
	return false;
}

// Adding poptarts is done via another state, display error
bool DispensesPoptart::addPoptart(int number)
{
	cout << "System already has poptarts!" << endl;
	return false;
}

// When dispensing, turn itemDispensed to true and delete the item
bool DispensesPoptart::dispense(void)
{
	cout << "Dispensing poptart!" << endl;

	//turn itemDispensed to true and delete the item
	((Poptart_Dispenser*)(this->CurrentContext))->itemDispensed = true;
	delete ((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem;

	// Removes 1 from the curent poptart amount
	this->CurrentContext->setStateParam(No_Of_Poptarts, this->CurrentContext->getStateParam(No_Of_Poptarts) - 1);

	// If there is no Credit left, revert to Has_Credit state
	if (this->CurrentContext->getStateParam(Credit) > 0 && this->CurrentContext->getStateParam(No_Of_Poptarts) > 0) {
		this->CurrentContext->setState(Has_Credit);
	}

	// If there's no money left in the system, revert to NoCredit state
	else if (this->CurrentContext->getStateParam(Credit) == 0 && this->CurrentContext->getStateParam(No_Of_Poptarts) > 0) {
		this->CurrentContext->setState(No_Credit);
	}

	// If there are no poptarts left then revert to Out_Of_Poptart state
	else if (this->CurrentContext->getStateParam(No_Of_Poptarts) == 0) {
		this->CurrentContext->setState(Out_Of_Poptart);
	}
	return true;
}

#pragma endregion

