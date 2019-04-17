#pragma once

#include <string>
#include <iostream>

#include "Contact.h"

using namespace User;

std::string mycontacts = "[{\"first\" : \"Alexander\",\"last\" : \"Bell\",\"phone\" : \"+16170000001\"},"
"{\"first\" : \"Thomas\",\"last\" : \"Watson\",\"phone\" : \"+16170000002\"},"
"{\"first\" : \"Elisha\",\"last\" : \"Gray\",\"phone\" : \"+18476003599\"},"
"{\"first\" : \"Antonio\",\"last\" : \"Meucci\",\"phone\" : \"+17188763245\"},"
"{\"first\" : \"Guglielmo\",\"last\" : \"Marconi\",\"phone\" : \"+39051203222\"},"
"{\"first\" : \"Samuel\",\"last\" : \"Morse\",\"phone\" : \"+16172419876\"},"
"{\"first\" : \"Tim\",\"last\" : \"Berners-Lee\",\"phone\" : \"+44204549898\"},"
"{\"first\" : \"John\",\"last\" : \"Baird\",\"phone\" : \"+4408458591006\"},"
"{\"first\" : \"Thomas\",\"last\" : \"Edison\",\"phone\" : \"+19086575678\"}]";

void RunAddContactTestCase1();
void RunUpdateContactTestCase2();
void RunListContactsTestCase3();
void RunFailureToAddContactTestCase4();
void RunMultiplethreadsAddUpdateTestCase5();
void RunInvalidContactTestCase6();
void RunMultipleObserversTestCase7();
void RunMultiplethreadsAddUpdateTestCase5();

void UpdateContactThread(Contacts& mycontact);
void AddContactThread(Contacts& mycontact);

enum TESTCASE { TEST1 = 1, TEST2, TEST3, TEST4, TEST5, TEST6, TEST7 };

std::mutex  mutexg;

class MyContactObserver : public ContactObserver
{
private:
	size_t _addcount{ 0 }, _updatecount{ 0 }, _loadcount{ 0 };
	TESTCASE _testnum;

public:
	explicit MyContactObserver(TESTCASE testnum_) : _testnum(testnum_)
	{
		std::lock_guard<std::mutex> lk(mutexg);
		std::cout << "\n\nRunning Test Case " << testnum_ << "\n";
	}

	virtual void OnContactAdded(Contact contact_)
	{
		std::lock_guard<std::mutex> lk(mutexg);

		std::cout << "\nOnContactAdded: Rcvd "
			<< " First: " << contact_.getfirstname() \
			<< " Second: " << contact_.getlastname() \
			<< " Phone: " << contact_.getphone() \
			<< " Added";
		_addcount++;
	}

	void setcount(size_t loadcount_) { _loadcount = loadcount_; }

	virtual void OnContactUpdated(Contact contact_)
	{
		std::lock_guard<std::mutex> lk(mutexg);

		std::cout << "\nOnContactUpdated: Rcvd " \
			<< " First: " << contact_.getfirstname() \
			<< " Second: " << contact_.getlastname() \
			<<  "Phone: " << contact_.getphone() \
			<< " Updated";

		_updatecount++;
	}

	~MyContactObserver()
	{
		if (_testnum == TESTCASE::TEST1 || _testnum == TESTCASE::TEST4 || _testnum == TESTCASE::TEST6)
		{
			if (_addcount != _loadcount)
			{
				std::lock_guard<std::mutex> lk(mutexg);

				std::cout << "\n\nTEST CASE " << _testnum << " FAILURE, MISMATCH LOADING and CALLBACKS RCVD";
				std::cout << "\nLoaded " << _loadcount << "contacts from json , received " << _addcount << " Entries";
			}
			else if (_addcount == _loadcount)
			{
				std::lock_guard<std::mutex> lk(mutexg);
				std::cout << "\n\nTEST CASE " << _testnum << " SUCCESS";
				std::cout << "\nLoaded " << _loadcount << " contacts from json , received " << _addcount << " Entries";
			}
		}
		else if (_testnum == TESTCASE::TEST2)
		{
			if (_updatecount != 1)
			{
				std::lock_guard<std::mutex> lk(mutexg);
				std::cout << "\n\nTEST CASE " << _testnum << " FAILURE, MISMATCH UPDATING and CALLBACKS RCVD";
				std::cout << "\nUpdated 1 " << " , received " << _updatecount << " Entries";
			}
			else
			{
				std::lock_guard<std::mutex> lk(mutexg);
				std::cout << "\n\nTEST CASE " << _testnum << " SUCCESS";
				std::cout << "\nUpdated 1 contact " << " , received " << _updatecount << " update Event";
			}
		}
		else if (_testnum == TESTCASE::TEST4)
		{
			if (_addcount == _loadcount)
			{
				std::lock_guard<std::mutex> lk(mutexg);

				std::cout << "\n\nTEST CASE 4 FAILURE, LOADING and CALLBACKS RCVD ARE SAME FOR DUPLICATE CONTACTS";
				std::cout << "\nLoaded " << _loadcount << " contacts from json , received " << _addcount << " Entries";
			}
			else if (_addcount != _loadcount)
			{
				std::lock_guard<std::mutex> lk(mutexg);
				std::cout << "\n\nTEST CASE 4 SUCCESS";
				std::cout << "\nLoaded " << _loadcount << " contacts from json , received " << _addcount << " Entries";
			}
		}
	}
};

class MyContactObserver2 : public ContactObserver
{
private:
	size_t _addcount{ 0 }, _updatecount{ 0 }, _loadcount{ 0 };
	TESTCASE _testnum;

public:
	explicit MyContactObserver2(TESTCASE testnum_) : _testnum(testnum_)
	{
	}

	void setcount(size_t loadcount_) { _loadcount = loadcount_; }

	virtual void OnContactAdded(Contact contact_)
	{
		std::lock_guard<std::mutex> lk(mutexg);

		std::cout << "\nMyContactObserver 2 : OnContactAdded: Rcvd "
			<< " First: " << contact_.getfirstname() \
			<< " Second: " << contact_.getlastname() \
			<< " Phone: " << contact_.getphone() \
			<< " Added";
		_addcount++;
	}

	~MyContactObserver2()
	{
		if (_testnum == TESTCASE::TEST7 || _testnum == TESTCASE::TEST5 )
		{
			if (_addcount != _loadcount)
			{
				std::lock_guard<std::mutex> lk(mutexg);

				std::cout << "\n\nMyContactObserver 2  TEST CASE " << _testnum << " FAILURE, MISMATCH LOADING and CALLBACKS RCVD";
				std::cout << "\nMyContactObserver 2 : Loaded " << _loadcount << "contacts from json , received " << _addcount << " Entries";
			}
			else if (_addcount == _loadcount)
			{
				std::lock_guard<std::mutex> lk(mutexg);
				std::cout << "\n\nMyContactObserver 2  TEST CASE " << _testnum << " SUCCESS";
				std::cout << "\nMyContactObserver 2 : Loaded " << _loadcount << " contacts from json , received " << _addcount << " Entries";
			}
		}
	}
};

int main()
{
	RunAddContactTestCase1();
	RunUpdateContactTestCase2();
	RunListContactsTestCase3();
	RunFailureToAddContactTestCase4();
	RunMultiplethreadsAddUpdateTestCase5();
    RunInvalidContactTestCase6();
    RunMultipleObserversTestCase7();
	//RunLargeJsonTestCase8(); // Need to stress test the library with large number of contact add/update requests
	                           // Also need to measure the performance of the test with RDTSC or perf or intel vTune

	return 0;
}

void RunAddContactTestCase1()
{
	MyContactObserver myobserver(TESTCASE::TEST1);
	Contacts mycontact;
	size_t result{ 0 };

	mycontact.registerObserver(&myobserver);

	bool ret = mycontact.loadContactsFromJSON(mycontacts, result);
	myobserver.setcount(result);

	std::this_thread::yield();
}

void RunUpdateContactTestCase2()
{
	MyContactObserver myobserver(TESTCASE::TEST2);
	bool serverupdate = true;
	Contacts mycontact(serverupdate);
	size_t result{ 0 };

	mycontact.registerObserver(&myobserver);

	bool ret = mycontact.loadContactsFromJSON(mycontacts, result);
	myobserver.setcount(result);

	std::this_thread::yield();
	std::this_thread::sleep_for(std::chrono::milliseconds(1400)); // Wait more than what update contacts waits to
	                                                      // generate updates, currently  updates update every
	                                                      // second
}

void RunListContactsTestCase3()
{
	Contacts mycontact;
	size_t result{ 0 };

	{
		std::lock_guard<std::mutex> lk(mutexg);
		std::cout << "\n\nRunning Test Case " << "3\n";
	}

	bool ret = mycontact.loadContactsFromJSON(mycontacts, result);

	std::list<Contact>& mylist = mycontact.listContacts();
	
	if (mylist.size() != result)
	{
		std::lock_guard<std::mutex> lk(mutexg);
		std::cout << "\n\nTEST CASE 3 FAILURE, Failure to add all contacts";
		std::cout << "\nLoaded " << mylist.size() << " contacts from json , received " << result << " Entries";
	}
	else if (mylist.size() == result)
	{
		std::lock_guard<std::mutex> lk(mutexg);
		std::cout << "\n\nTEST CASE 3 SUCCESS";
		std::cout << "\nLoaded " << mylist.size() << " contacts from json , received " << result << " Entries";
	}
}

void RunFailureToAddContactTestCase4()
{
	Contacts mycontact;
	size_t result{ 0 };
	MyContactObserver myobserver(TESTCASE::TEST4);

	mycontact.registerObserver(&myobserver);
	std::string mycontactsdup = " [{\"first\" : \"Alexander\",\"last\" : \"Bell\",\"phone\" : \"+16170000001\"},"
		"{\"first\" : \"Thomas\",\"last\" : \"Watson\",\"phone\" : \"+16170000002\"},"
		"{\"first\" : \"Elisha\",\"last\" : \"Gray\",\"phone\" : \"+18476003599\"},"
		"{\"first\" : \"Thomas\",\"last\" : \"Watson\",\"phone\" : \"+16170000002\"}]";

	bool ret = mycontact.loadContactsFromJSON(mycontactsdup, result);
	myobserver.setcount(result); // only three contacts should have been loaded

	std::this_thread::yield();
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void RunInvalidContactTestCase6()
{
	Contacts mycontact;
	size_t result{ 0 };
	MyContactObserver myobserver(TESTCASE::TEST6);

	mycontact.registerObserver(&myobserver);
	std::string mycontactsdup = " [{\"\" : \"Alexander\",\"last\" : \"Bell\",\"phone\" : \"+16170000001\"},"
		"{\"first\" : \"Thomas\",\"last\" : \"Watson\",\"phone\" : \"+16170000002\"},"
		"{\"first\" : \"Elisha\",\"last\" : \"Gray\",\"phone\" : \"+18476003599\"},"
		"{\"\" : \"Thomas\",\"last\" : \"Watson\",\"phone\" : \"+16170000002\"}]";

	bool ret = mycontact.loadContactsFromJSON(mycontactsdup, result);
	myobserver.setcount(result); // only three contacts should have been loaded

	std::this_thread::yield();
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void RunMultipleObserversTestCase7()
{
	MyContactObserver myobserver(TESTCASE::TEST7);
	MyContactObserver2 myobserver2(TESTCASE::TEST7); // second observer

	Contacts mycontact;
	size_t result{ 0 };

	mycontact.registerObserver(&myobserver);
	mycontact.registerObserver(&myobserver2);

	bool ret = mycontact.loadContactsFromJSON(mycontacts, result);
	myobserver2.setcount(result);

	std::this_thread::yield();
}

void RunMultiplethreadsAddUpdateTestCase5()
{
	Contacts mycontact(true);
	size_t result{ 0 };

	std::thread t1(AddContactThread, std::ref(mycontact));
	std::thread t2(UpdateContactThread, std::ref(mycontact));

	t1.join();
	t2.join();

	std::this_thread::yield();
}

void AddContactThread(Contacts& mycontact)
{
	MyContactObserver myobserver(TESTCASE::TEST5);
	size_t result;
	std::string mycontactsupd = " [{\"first\" : \"Alexander\",\"last\" : \"Bell\",\"phone\" : \"+16170000001\"}]";
	
	bool ret = mycontact.loadContactsFromJSON(mycontactsupd, result);
	myobserver.setcount(result);
	std::this_thread::sleep_for(std::chrono::milliseconds(1400));
}

void UpdateContactThread(Contacts& mycontact)
{
	MyContactObserver2 myobserver2(TESTCASE::TEST5);

	mycontact.registerObserver(&myobserver2);
	myobserver2.setcount(1);
	std::this_thread::sleep_for(std::chrono::milliseconds(1400)); // Wait more than what update contacts waits
																  // generate updates
}