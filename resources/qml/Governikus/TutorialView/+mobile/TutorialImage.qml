/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0

Image {
	property real centerX: 0.5
	property real centerY: 0.5

	asynchronous: true
	fillMode: Image.PreserveAspectFit
	height: width * (sourceSize.height / sourceSize.width)
	x: (parent.width * centerX) - (width / 2)
	y: (parent.height * centerY) - (height / 2)
}
