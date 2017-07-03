import QtQuick 2.5

import "../global"

Rectangle {
	id: baseItem

	property alias searchText: searchBar.searchText

	// Properties that are set by ProviderSectionView.
	property string selectedCategory: ""

	property bool withButtons: false

	height: Constants.titlebar_height + Constants.searchbar_height

	SearchBar {
		id: searchBar

		anchors.right: parent.right
		anchors.bottom: parent.bottom

		width: parent.width

		searchBarColor: baseItem.selectedCategory === "" ? Constants.blue : Category.displayColor(baseItem.selectedCategory)
	}
}
