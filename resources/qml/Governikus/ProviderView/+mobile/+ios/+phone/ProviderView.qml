/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.View 1.0

BaseProviderView {
	id: baseItem
	additionalProviderListTopPadding: searchBarHeader.height
	searchBar: providerSearchBar

	Rectangle {
		id: searchBarHeader

		readonly property var menuBar: ApplicationWindow.menuBar

		color: menuBar && menuBar.color ? menuBar.color : titleBarColor
		height: providerSearchBar.height
		width: baseItem.width

		Behavior on color  {
			ColorAnimation {
				duration: Constants.animation_duration
			}
		}

		SearchBar {
			id: providerSearchBar
			width: parent.width
		}
	}
}
