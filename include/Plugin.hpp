#ifndef INCLUDE_PLUGIN_HPP_
#define INCLUDE_PLUGIN_HPP_


#include <string>
#include <list>
#include "ComPoint.hpp"

using namespace std;


/**
 * class Plugin
 * Contains information about a real plugin. This information contains the
 * name, pluginNumber, path to the unix domain socket file and all known methods by
 * full name. With the information of a plugin instance , it is possible to create a
 * valid instance of UdsComClient which is a connection to this plugin.
 */
class Plugin{

	public:

		/**
		 * Constructor.
		 * \param name Contains the unique name of this plugin. E.g. "Aardvark" or "I2c".
		 * \param pluginNumber Contains the unique number of this plugin.
		 * \param path Contains the path to the corresponding unix domain socket file.
		 * \note Instances of plugin are generated during the registration process.
		 */
		Plugin(const char* name, int pluginNumber, const char* path)
		{
			this->name = new string(name);
			this->pluginNumber = pluginNumber;
			this->udsFilePath = new string(path);
			comPoint = NULL;
		}

		Plugin(Plugin* plugin)
		{
			this->name = new string(*(plugin->getName()));
			this->pluginNumber = plugin->getPluginNumber();
			this->udsFilePath = new string(*(plugin->getUdsFilePath()));
			comPoint = NULL;
		}

		/**
		 * Destructor.
		 */
		virtual ~Plugin()
		{
			delete name;
			delete udsFilePath;
			deleteMethodList();
			if(comPoint != NULL)
				delete comPoint;
		}

		ComPoint* getComPoint(){return this->comPoint;}

		void setComPoint(ComPoint* comPoint)
		{
			this->comPoint = comPoint;
		}


		/**
		 * Adds a method to the intern list of methods.
		 * \param methodName The name of the method.
		 * \note The methodName will not be copied, the list saves pointers.
		 * Deleting the plugin will also delallocate alls string contained in the list.
		 */
		void addMethod(string* methodName)
		{
			methods.push_back(methodName);
		}

		/**
		 * \return The unique pluginName.
		 */
		string* getName(){return this->name;}

		/**
		 * \return The path to the unix domain socket file.
		 */
		string* getUdsFilePath(){return this->udsFilePath;}


		/**
		 * \return The unique pluginNumber.
		 */
		int getPluginNumber(){return this->pluginNumber;}


		/**
		 * \return Return a pointer to the intern list of string*, which contains all known methodnames.
		 */
		list<string*>* getMethods(){return &(this->methods);}




	private:

		/*! Unique name of this plugin.*/
		string* name;
		/*! Unique number of this plugin.*/
		int pluginNumber;
		/*! Path to the unix domain socket file.*/
		string* udsFilePath;
		/*! Contains all methodnames which where added via addMethod().*/
		list<string*> methods;
		/*! This variable will be used by connection-context for connection to a plugin.*/
		ComPoint* comPoint;


		/**
		 * Deletes all entrys of the methodList. The entrys are pointer
		 * to strings and will be deallocated, too.
		 */
		void deleteMethodList()
		{
			list<string*>::iterator i = methods.begin();

			while(i != methods.end())
			{
				delete *i;
				i = methods.erase(i);
			}
		}
};



#endif /* INCLUDE_PLUGIN_HPP_ */
