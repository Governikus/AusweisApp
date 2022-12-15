/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

BaseProviderView {
	id: baseItem
	rightTitleBarAction: SearchBar {
		availableWidth: baseItem.width - Style.dimens.menubar_width

		onSearchTextChanged: ProviderCategoryFilterModel.searchString = searchText
	}
}
