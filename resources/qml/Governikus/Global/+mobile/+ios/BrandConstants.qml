/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

DeviceConstants {
	readonly property bool is_layout_android: false
	readonly property bool is_layout_ios: true
	readonly property string layout: "ios"
	readonly property bool leftNavigation: false
	readonly property bool bottomNavigation: true
}
