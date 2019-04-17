- Contact Manager to manage various contacts 

- Uses RapidJSON interface to manage contacts 

- All header files are in include directory

- All src files are in src directory

- Contact.cpp contains main interface for the Contact Manager

Description:-

  Thread safe Contact Manager library basically allows three functionalities add, update, list respectively. It has its contract definition in the header file. It also allows asynchronous call back mechanisms based upon the appropriate action. 

   It reads the contacts to be added from JSON file and used rapidJSON high performance header only definitions to parse the json. It keeps the contacts internally in an hash map with key values based upon hash of the attributes, first name,last name, phone number together defines the uniqueness of a valid contact. It uses C++ STL unordered_map hash implementation.

   It has various observers that can receive async notification whenever add/update is triggerred. The library just need to create an instance of Contact class. It is scalable depending upon the number of contacts that need to be managed. It can be configured to spawn notification multiple threads based upon the load.

   It also simulates an update action by periodically triggering the update actions based upon some contact update randomly. Several test cases have been written to test the contact manager. It also supports concurrent addition/update of the underlying contact by multiple client side threads.