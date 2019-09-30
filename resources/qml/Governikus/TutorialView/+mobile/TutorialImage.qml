/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0

Image {
	property real centerX: 0.5
	property real centerY: 0.5

	height: width * (sourceSize.height / sourceSize.width)
	fillMode: Image.PreserveAspectFit

	y: (parent.height * centerY) - (height / 2)
	x: (parent.width * centerX) - (width / 2)

	asynchronous: true
}
