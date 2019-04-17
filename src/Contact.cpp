#pragma once
#include <iostream>

#include "Contact.h"
#include "rapidjson\document.h"

using namespace User;
using namespace rapidjson;

Contacts::Contacts(bool serverupdate_): _joiner(_threads), _done(false), _serverupdate(serverupdate_)
{
	_observerlist.clear();
	_eventqueue.clear();

	StartNotifyThreads();

	if (_serverupdate)
		StartUpdateThread();
}

bool Contacts::addContact(const Contact& contact_)
{
	if (!isContactvalid(contact_))
		return false;

	bool ret = addtoContactMap(contact_);

	if (ret)
	{
		// Write to notification thread queue about the contact Addition
	//	std::cout << "\nContact: " << contact_.getfirstname() << " written to notification queue";
		writetoNotificationQueue(contact_, ContactEvents::ADD);
	}

	return ret;
}

bool Contacts::isContactvalid(const Contact& contact_)
{
	if (contact_.getfirstname().empty() || contact_.getlastname().empty() || contact_.getphone().empty())
		return false;

	return true;
}

bool Contacts::updateContact(const Contact & oldcontact_, const Contact & newcontact_)
{
	if ( !isContactvalid(oldcontact_) || !isContactvalid(newcontact_) )
		return false;

	bool ret = updateContactMap(oldcontact_, newcontact_);

	if (ret)
	{
		// write to notification thread queue about old contact being updated
		// It is easy to pass in new contact as well in ContactEventMsg struct but for simplicity
		// right now just passing old contact
		ContactEventMsg contactEvent(oldcontact_.getfirstname(), oldcontact_.getlastname(), oldcontact_.getphone(), ContactEvents::UPDATE);
		_eventqueue.push(contactEvent);
	}

	return ret;
}

// Write to notification thread queue about the update
void Contacts::writetoNotificationQueue(const Contact & contact_, ContactEvents event_)
{
	bool notify = false;

	if (!_observerseventlist.empty())
	{
		if (_observerseventlist.find(event_) != _observerseventlist.end())
			notify = true;
	}

	if (!_observerlist.empty())
	{
		notify = true;
	}

	if (notify)
	{
		ContactEventMsg contactEvent(contact_.getfirstname(), contact_.getlastname(), contact_.getphone(), event_);
		_eventqueue.push(contactEvent);
	}
}

std::list<Contact> Contacts::listContacts() const
{
	return contactLists();
}

void Contacts::registerObserver(ContactObserver *observer_)
{
	_observerlist.push_back(observer_);
}

void Contacts::unregisterObserver(ContactObserver * observer_)
{
	auto it = std::find(_observerlist.begin(), _observerlist.end(), observer_);

	if (it != _observerlist.end())
		_observerlist.erase(it);
}

void Contacts::notifyObservers()
{
	while (!_done)
	{
		ContactEventMsg data{};
		_eventqueue.wait_and_pop(data);

		if (!_observerseventlist.empty())
		{
			if (_observerseventlist.find(data.getEvent()) != _observerseventlist.end())
			{
				for (const auto& obs : _observerseventlist[data.getEvent()])
					obs();
			}
		}

		if (!_observerlist.empty())
		{
			for (const auto& obs : _observerlist)
			{
				//std::cout << "\nIn Contacts event..";

				if (data.getEvent() == ContactEvents::UPDATE)
				{
					obs->OnContactUpdated(Contact(data.getfirstname(), data.getlastname(), data.getphone()));
				}
				else if (data.getEvent() == ContactEvents::ADD)
				{
				//	std::cout << "\nIn Contacts event ADD.." << typeid(*obs).name();
					obs->OnContactAdded(Contact(data.getfirstname(), data.getlastname(), data.getphone()));
				}
			}
		}
	}
}

void Contacts::StartNotifyThreads()
{
	unsigned long const Hardwarethreads =
		std::thread::hardware_concurrency();
	static unsigned long Maxthreads = 1; // for scalability if there are millions of contacts/requests we can scale
								   // by increasing the number of threads
	unsigned long const num_threads =
		std::min(Hardwarethreads != 0 ? Hardwarethreads : 2, Maxthreads);

	std::cout << "\nNum threads: " << num_threads;
	for (unsigned long ii = 0; ii < num_threads; ++ii)
	{
		try
		{
			_threads.push_back(std::thread(&Contacts::notifyObservers, this));
		}
		catch (...)
		{
			_done = true;
			throw;
		}
	}
}

void Contacts::StartUpdateThread()
{
	try
	{

		_threads.push_back(std::thread(&Contacts::UpdateContactTimerInterval, this, getupdatetimer()));
	}
	catch (...)
	{
		_done = true;
		throw;
	}
}

void Contacts::UpdateContactTimerInterval(unsigned int interval)
{
	uint32_t ii = 0, prevpos = 0;

	while (!_done && _serverupdate)
	{
			auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
			std::this_thread::sleep_until(x);

			ii = ii % _contactmap.size(); // avoid overflow of iterators

			auto nextit = std::next(_contactmap.begin(), ii);
			ii++;

			if (nextit != _contactmap.end())
			{
				Contact oldcontact(nextit->first);
				
				std::string first = oldcontact.getfirstname() + "XXX";
				std::string phone = "+7323009261";

				Contact newcontact(first, oldcontact.getlastname(), phone);

				bool ret = updateContactMap(oldcontact, newcontact );

				if (ret)
				{
					// Write to notification thread queue about the contact Update
				//	std::cout << "\nContact: " << contact_.getfirstname() << " written to notification queue";
					writetoNotificationQueue(oldcontact, ContactEvents::UPDATE);
				}
			}
	}
}

bool Contacts::loadContactsFromJSON(const std::string & str_, size_t& count_)
{
	rapidjson::Document document;
	document.Parse(str_.c_str());

	if (str_.empty())
		return false;

	if (!document.IsArray()) {
		return false;
	}

	std::vector<std::string> customer;
	Contact customerid;
	for (Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
	{
		const Value& obj = *itr;
		customer.clear();
		for (Value::ConstMemberIterator it = obj.MemberBegin(); it != obj.MemberEnd(); ++it)
		{
			if (it->value.IsString())
			{
				customer.push_back(it->value.GetString());
				
	//			std::cout << "\nvalue of member " << it->name.GetString() << " is " << it->value.GetString();
			}
		}
		
		if ( customer.size() != MAXATTRIBUTES )
			continue;

		customerid.setfirstname(customer[CustomerAttr::FIRST]);
		customerid.setlastname(customer[CustomerAttr::LAST]);
		customerid.setphonenumber(customer[CustomerAttr::PHONE]);

		bool ret = addContact(customerid);

		if (ret)
			count_++;
	}

	return true;
}