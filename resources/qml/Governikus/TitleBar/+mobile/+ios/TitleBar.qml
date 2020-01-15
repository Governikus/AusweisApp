/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property alias titleBarOpacity: titleBar.titleBarOpacity
	property NavigationAction navigationAction
	property alias title: titleBar.title
	property alias rightAction: titleBar.rightAction
	property alias subTitleBarAction: titleBar.subTitleBarAction
	property alias color: titleBar.color

	height: titleBar.height

	BaseTitleBar {
		id: titleBar

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}

		leftAction: TitleBarNavigation {
			state: baseItem.navigationAction ? baseItem.navigationAction.state : ""
			enabled: baseItem.navigationAction ? baseItem.navigationAction.enabled : true
			onClicked: baseItem.navigationAction.clicked()
			onTextUpdated: titleBar.updateLeftAction()
		}

	}
}
