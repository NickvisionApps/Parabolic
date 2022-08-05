#include "Messenger.h"

namespace NickvisionTubeConverter::UI
{
	Messenger::Messenger()
	{

	}

	Messenger& Messenger::getInstance()
	{
		static Messenger instance;
		return instance;
	}

	bool Messenger::registerMessage(const std::string& name, const std::function<void(void* parameter)>& callback)
	{
		if (m_messages.contains(name))
		{
			return false;
		}
		m_messages.insert({ name, callback });
		return true;
	}

	bool Messenger::deregisterMessage(const std::string& name)
	{
		if (!m_messages.contains(name))
		{
			return false;
		}
		m_messages.erase(name);
		return true;
	}

	bool Messenger::sendMessage(const std::string& name, void* parameter) const
	{
		if (!m_messages.contains(name))
		{
			return false;
		}
		m_messages.at(name)(parameter);
		return true;
	}
}