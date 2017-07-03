/*!
 * ChainBuilder.h
 *
 * \brief Generic implementation for chain building, i.e. building ordered lists.
 * The ChainBuilder is initialized with a pool of objects and a (pointer to a) function
 * that decides if two objects have a parent child relation. Duplicates are filtered out.
 *
 * All found chains are returned by the function /ref ChainBuilder::getChains().
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include <QVector>
#include <algorithm>
#include <functional>


namespace governikus
{

template<typename T>
class ChainBuilder
{
	protected:
		QVector<QVector<T> > mChains;

	private:
		std::function<bool(const T& pChild, const T& pParent)> mIsChildFunc;

		void buildChain(const QVector<T>& pAllElements, const QVector<T>& pChain)
		{
			bool chainComplete = true;

			for (const auto& elem : pAllElements)
			{
				if (pChain.contains(elem))
				{
					continue;
				}

				if (mIsChildFunc(elem, pChain.last()))
				{
					QVector<T> extendedChain(pChain);
					extendedChain.append(elem);
					buildChain(pAllElements, extendedChain);
					chainComplete = false;
				}
				else if (mIsChildFunc(pChain.first(), elem))
				{
					QVector<T> extendedChain(pChain);
					extendedChain.prepend(elem);
					buildChain(pAllElements, extendedChain);
					chainComplete = false;
				}
			}

			if (chainComplete && !isSubChain(pChain))
			{
				mChains += pChain;
			}
		}


		bool isSubChain(const QVector<T>& pSubChain)
		{
			for (const auto& chain : qAsConst(mChains))
			{
				if (std::search(chain.begin(), chain.end(), pSubChain.begin(), pSubChain.end()) != chain.end())
				{
					return true;
				}
			}
			return false;
		}


	public:
		ChainBuilder(const QVector<T>& pAllElements, const std::function<bool(const T& pChild, const T& pParent)>& pIsChildFunc)
			: mChains()
			, mIsChildFunc(pIsChildFunc)
		{
			for (const auto& elem : pAllElements)
			{
				const QVector<T> chain({elem});
				if (!isSubChain(chain))
				{
					buildChain(pAllElements, chain);
				}
			}
		}


		const QVector<QVector<T> >& getChains() const
		{
			return mChains;
		}


};


} /* namespace governikus */
