#pragma once

#include <unordered_map>
#include <list>
#include <functional> // for std::function
#include <utility>

#include <threadclass.h>

using namespace Threading;

namespace User
{
	class Contact
	{
	private:
		std::string _first; // firstname of the person
		std::string _last; // lastname of the person
		std::string _phone; // phone number of the person

	public:
		Contact(const std::string first_, const std::string last_, const std::string phone_) :
			_first(first_),
			_last(last_),
			_phone(phone_)
		{
		}

		Contact()
		{
		}

		const std::string& getfirstname() const
		{
			return _first;
		}

		const std::string& getlastname() const
		{
			return _last;
		}

		const std::string& getphone() const
		{
			return _phone;
		}

		void setfirstname(const std::string first_)
		{
			_first = first_;
		}

		void setlastname(const std::string last_)
		{
			_last = last_;
		}

		void setphonenumber(const std::string phone_)
		{
			_phone = phone_;
		}

		bool operator==(const Contact& p_) const
		{
			return _first == p_._first && _last == p_._last && _phone == p_._phone;
		}
	};

	class hash_name {
	 public:
		std::size_t operator()(const User::Contact& name_) const
		{
			return std::hash<std::string>()(name_.getfirstname()) ^ std::hash<std::string>()(name_.getlastname())
				^ std::hash<std::string>()(name_.getphone());
		}
	};

	enum ContactEvents { ADD, UPDATE, NONE};
	enum CustomerAttr { FIRST, LAST, PHONE };

	constexpr uint8_t numevents = 255;
	constexpr size_t MAXATTRIBUTES = 3;

	class ContactEventMsg
	{
	private:
		ContactEvents _event{ ContactEvents::NONE };
		std::string  _first;
		std::string  _last;
		std::string  _phone;
	public:
		ContactEventMsg(const std::string first_, const std::string last_, const std::string phone_, const ContactEvents event_) :_first(first_), _last(last_), _phone(phone_), _event(event_)
		{}

		ContactEventMsg() {}

		uint8_t getEventInt() const
		{
			switch (_event)
			{
				case ContactEvents::ADD: return 0;
				case ContactEvents::UPDATE: return 1;
			}
		}

		ContactEvents getEvent() const { return _event; }

		const std::string& getfirstname() const
		{
			return _first;
		}

		const std::string& getlastname() const
		{
			return _last;
		}

		const std::string& getphone() const
		{
			return _phone;
		}
	};

	class ContactObserver
	{
	public:
		virtual void OnContactAdded(Contact contact_) 
		{
			std::cout << "\nDefault Add..";
		}
		virtual void OnContactUpdated(Contact contact_) 
		{
			std::cout << "\n Default Update..";
		}
	};

	class Contacts
	{
	private:
		bool isContactvalid(const Contact& contact_);

		// observers applicable to all supported events and specific to ContactObservers only
		std::vector<ContactObserver*> _observerlist;
		// specific event observers, observer can be anything like function, lamda expression, class method etc
		std::unordered_map<ContactEvents, std::vector<std::function<void()>>> _observerseventlist;

		using ContactMap = std::unordered_map<Contact, bool, hash_name>;
		ContactMap _contactmap;

		threadsafe_queue<ContactEventMsg> _eventqueue;

		std::atomic<bool> _done = false;
		static uint8_t constexpr QUEUERETRY = 3;
		std::vector<std::thread> _threads;
		join_threads _joiner;
		std::atomic<bool> _serverupdate = false;
		mutable std::mutex  _contactmutex;
		unsigned int _updatetimer{ 1000 };

		void writetoNotificationQueue(const Contact& contact_, ContactEvents event_);
		void notifyObservers();
		void StartNotifyThreads();
		void StartUpdateThread();
		void UpdateContactTimerInterval(unsigned int interval);

		bool addtoContactMap(const Contact& contact_)
		{
			std::lock_guard<std::mutex> lk(_contactmutex);

			auto it = _contactmap.find(contact_);

			if (it == _contactmap.end())
			{
				_contactmap[contact_] = true; // just added a bit to indicate valid contact
				return true;
			}
		
			return false;
		}

		bool updateContactMap(const Contact& oldcontact_, const Contact& newcontact_)
		{
			std::lock_guard<std::mutex> lk(_contactmutex);

			auto it1 = _contactmap.find(oldcontact_); // no race condition here

			if (it1 == _contactmap.end())
				return false;// contact not found to update

			auto it2 = _contactmap.find(newcontact_); // no race condition here as well

			if (it2 != _contactmap.end())
				return false;// new contact already exists, let the user try with some other new contact

			_contactmap.erase(it1); // erase old contact that already exists

			_contactmap[newcontact_] = true; // just added a bit to indicate valid contact

			return true;
		}

		std::list<Contact> contactLists() const
		{
			std::list<Contact> contacts;
			std::lock_guard<std::mutex> lk(_contactmutex);

			for (auto& clist : _contactmap)
				contacts.emplace_back(clist.first);

			return contacts; // Return value optimization
		}

		// We can see that Observer is a template parameter of the new registerObserver template member function. 
		// Since we are in a deduced context, we can use a universal reference for the template parameter, 
		// Observer&& to allow for perfect forwarding and benefit from move semantics. 
		template <typename Observer>
		void registerObserver(const ContactEvents& event_, Observer&& observer_)
		{
			_observerseventlist[event_].push_back(std::forward<Observer>(observer_));
		}

		template <typename Observer>
		void UnregisterObserver(const ContactEvents& event_, Observer&& observer_)
		{
			auto it = _observerseventlist(event_);

			if (it == _observerseventlist.end())
				return;

			auto& vec = it->second;

			auto it1 = std::find(vec.begin(), vec.end(), observer_);

			if (it != vec.end())
				vec.erase(it1);

			return;
		}

	public:
		Contacts(bool serverupdate_ = false);

		~Contacts()
		{
			uint8_t ii = 0;
			while (!_eventqueue.empty() && ++ii <= QUEUERETRY)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			_eventqueue.stop();
			_done = true;
		}

		// Add a new contact by first name, last name, phone number
		// Returns true ( contact added ) / false ( contact already exists )
		bool addContact(const Contact& contact_);

		// Update a old contact to new contact
		// Returns true ( contact updated ) / false ( contact cannot be updated )
		bool updateContact(const Contact& oldcontact_, const Contact& newcontact_); // updates contact's first name or last name or phone number
		
		// Returns a list of Contact
		std::list<Contact> listContacts() const; // currently list all the contacts by first name, last name, phone num
										   // in the future it should list based upon first name or last name or phone number

		bool loadContactsFromJSON(const std::string& str_, size_t& count_);

		// Register a contactObserver to notify if contacts are added/updated, Multiple observers allowed
		void registerObserver(ContactObserver* observer_); // Register the ContactObserver overriden methods defined
														   // in the interface
		// Unregister a prev registered observer
		void unregisterObserver(ContactObserver* observer_); // Unregister the ContactObserver overriden methods defined
														     // in the interface
		void EnableServerupdate() 
		{
			if (!_serverupdate)
			{
				_serverupdate = true;

				StartUpdateThread();
			}
		}

		const unsigned int Contacts::getupdatetimer() const
		{
			return _updatetimer;
		}

		void Contacts::setupdateTimer(unsigned int timer_)
		{
			_updatetimer = timer_;
		}

		void DisableServerupdate() { _serverupdate = false; }
	};
}