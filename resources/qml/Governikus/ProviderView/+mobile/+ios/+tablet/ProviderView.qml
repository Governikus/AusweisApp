/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

BaseProviderView {
	id: baseItem

	additionalProviderListTopPadding: searchBarHeader.height

	Rectangle {
		id: searchBarHeader

		width: baseItem.width
		height: providerSearchBar.height

		color: menuBar && menuBar.color ? menuBar.color : titleBarColor

		Behavior on color { ColorAnimation { duration: Constants.animation_duration } }

		SearchBar {
			id: providerSearchBar

			width: parent.width
			onSearchTextChanged: ProviderCategoryFilterModel.searchString = searchText
		}
	}
}
