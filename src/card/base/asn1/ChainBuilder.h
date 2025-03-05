/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QList>
#include <algorithm>
#include <functional>


namespace governikus
{

template<typename T>
class ChainBuilder
{
	private:
		QList<QList<T>> mChains;
		std::function<bool(const T& pChild, const T& pParent)> mIsChildFunc;

		void buildChain(const QList<T>& pAllElements, const QList<T>& pChain)
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
					QList<T> extendedChain(pChain);
					extendedChain += elem;
					buildChain(pAllElements, extendedChain);
					chainComplete = false;
				}
				else if (mIsChildFunc(pChain.first(), elem))
				{
					QList<T> extendedChain({elem});
					extendedChain += pChain;
					buildChain(pAllElements, extendedChain);
					chainComplete = false;
				}
			}

			if (chainComplete && !isSubChain(pChain))
			{
				mChains += pChain;
			}
		}


		bool isSubChain(const QList<T>& pSubChain)
		{
			return std::any_of(mChains.constBegin(), mChains.constEnd(), [&pSubChain] (const QList<T>& pChain) {
						return std::search(pChain.constBegin(), pChain.constEnd(), pSubChain.constBegin(), pSubChain.constEnd())
							   != pChain.constEnd();
					});
		}

	protected:
		QMutableListIterator<QList<T>> getChainIterator()
		{
			return QMutableListIterator<QList<T>>(mChains);
		}

	public:
		ChainBuilder(const QList<T>& pAllElements, const std::function<bool(const T& pChild, const T& pParent)>& pIsChildFunc)
			: mChains()
			, mIsChildFunc(pIsChildFunc)
		{
			for (const auto& elem : pAllElements)
			{
				const QList<T> chain({elem});
				if (!isSubChain(chain))
				{
					buildChain(pAllElements, chain);
				}
			}
		}


		const QList<QList<T>>& getChains() const
		{
			return mChains;
		}


};


} // namespace governikus
