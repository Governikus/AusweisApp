/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

BaseProviderView {
	subTitleBarAction: SearchBar {
		width: baseItem.width
		onSearchTextChanged: ProviderCategoryFilterModel.searchString = searchText
	}
}
