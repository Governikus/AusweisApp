/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Shapes 1.12

import Governikus.Global 1.0

Shape {
	property double pointerYRatio: 0.5

	ShapePath {
		startX: 0
		startY: 0
		fillColor: Constants.black

		PathLine { x: 0; y: height }
		PathLine { x: width; y: pointerYRatio * height }
		PathLine { x: 0; y: 0 }
	}
}
