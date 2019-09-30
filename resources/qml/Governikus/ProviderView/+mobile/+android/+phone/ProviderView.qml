/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Provider 1.0

BaseProviderView {
	rightTitleBarAction: SearchBar {
		availableWidth: baseItem.width - Style.dimens.menubar_width
		onSearchTextChanged: ProviderCategoryFilterModel.searchString = searchText
	}

	header: ProviderHeader {
		width: baseItem.width
		selectedCategory: category
	}
}
