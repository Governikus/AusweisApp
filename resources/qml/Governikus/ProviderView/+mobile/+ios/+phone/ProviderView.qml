/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.View 1.0

BaseProviderView {
	id: baseItem

	searchBar: providerSearchBar
	additionalProviderListTopPadding: searchBarHeader.height

	Rectangle {
		id: searchBarHeader

		readonly property var menuBar: ApplicationWindow.menuBar

		width: baseItem.width
		height: providerSearchBar.height

		color: menuBar && menuBar.color ? menuBar.color : titleBarColor

		Behavior on color { ColorAnimation { duration: Constants.animation_duration } }

		SearchBar {
			id: providerSearchBar

			width: parent.width
		}
	}
}
