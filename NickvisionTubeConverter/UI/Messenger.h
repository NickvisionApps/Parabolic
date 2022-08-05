#pragma once

#include <string>
#include <functional>
#include <unordered_map>

namespace NickvisionTubeConverter::UI
{
	/// <summary>
	/// A container for managing UI messages throughout an application
	/// </summary>
	class Messenger
	{
	public:
		Messenger(const Messenger&) = delete;
		void operator=(const Messenger&) = delete;
		/// <summary>
		/// Gets the Messenger singleton object
		/// </summary>
		/// <returns>A reference to the Messenger object</returns>
		static Messenger& getInstance();
		/// <summary>
		/// Adds a message to the container
		/// </summary>
		/// <param name="name">The name of the message</param>
		/// <param name="callback">The function to run when the message is sent</param>
		/// <returns>True if the message was added, else false</returns>
		bool registerMessage(const std::string& name, const std::function<void(void* parameter)>& callback);
		/// <summary>
		/// Removes a message from the container
		/// </summary>
		/// <param name="name">The name of the message</param>
		/// <returns>True if the message was removed, else false</returns>
		bool deregisterMessage(const std::string& name);
		/// <summary>
		/// Sends a message, calling the callback function of that message
		/// </summary>
		/// <param name="name">The name of the message</param>
		/// <param name="parameter">The parameter to pass to the message's callback</param>
		/// <returns>True if the message was sent, else false</returns>
		bool sendMessage(const std::string& name, void* parameter) const;

	private:
		/// <summary>
		/// Constructs a Messenger object
		/// </summary>
		Messenger();
		std::unordered_map<std::string, std::function<void(void* parameter)>> m_messages;
	};
}

