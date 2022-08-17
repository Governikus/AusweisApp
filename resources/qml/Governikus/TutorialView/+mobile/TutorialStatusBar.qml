/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0

Rectangle {
	anchors.top: parent.top
	height: plugin.safeAreaMargins.top
	width: parent.width

	color: footer.color

	Behavior on color { ColorAnimation { duration: Constants.animation_duration } }
}
