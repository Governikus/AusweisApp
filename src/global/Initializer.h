/*
 * \brief Initializer to register a lambda that will be executed if QCoreApplication is ready.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>
#include <list>

class test_Initializer;

namespace governikus
{

class Initializer
{
	private:
		friend class ::test_Initializer;
		std::list<std::function<void()> > mRegisteredFunctions;

	protected:
		Initializer() = default;
		~Initializer() = default;

	public:
		static Initializer& getInstance();

		struct Entry final
		{
			Entry(const std::function<void()>& pRegister)
			{
				Initializer::getInstance().add(pRegister);
			}


			Entry(const Entry& pCopy) = delete;
			Entry(const Entry&& pCopy) = delete;
			Entry& operator=(const Entry& pCopy) = delete;
			Entry& operator=(const Entry&& pCopy) = delete;
		};

		void init(); // No need to call this!
		void add(const std::function<void()>& pRegister);
};

} /* namespace governikus */
