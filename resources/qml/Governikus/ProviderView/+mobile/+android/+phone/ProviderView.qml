/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Provider 1.0

BaseProviderView {
	id: baseItem
	contentBehindTitlebar: true
	rightTitleBarAction: searchBar
	titleBarOpacity: headerItem.titleBarOpacity

	headerComponent: Component {
		ProviderHeader {
			selectedCategory: baseItem.category
			width: baseItem.width

			onCategorySelected: ProviderCategoryFilterModel.setCategorySelection(category)
		}
	}
	searchBar: SearchBar {
		availableWidth: baseItem.width - Style.dimens.menubar_width
	}
}
