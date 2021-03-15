/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Provider 1.0

BaseProviderView {
	id: baseItem

	titleBarOpacity: headerItem.titleBarOpacity
	contentBehindTitlebar: true
	rightTitleBarAction: searchBar
	searchBar: SearchBar {
		availableWidth: baseItem.width - Style.dimens.menubar_width
	}

	headerComponent: Component {
		ProviderHeader {
			width: baseItem.width

			selectedCategory: baseItem.category

			onCategorySelected: ProviderCategoryFilterModel.setCategorySelection(category)
		}
	}
}
