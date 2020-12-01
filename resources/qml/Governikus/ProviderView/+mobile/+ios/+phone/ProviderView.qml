/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.View 1.0

BaseProviderView {
	id: baseItem

	subTitleBarAction: searchBar
	searchBar: SearchBar {
		width: baseItem.width
	}
}
