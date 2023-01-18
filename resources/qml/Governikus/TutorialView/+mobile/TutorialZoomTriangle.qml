/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Shapes 1.15
import Governikus.Global 1.0

Shape {
	property double pointerXRatio: 1.0
	property double pointerYRatio: 0.5

	ShapePath {
		fillColor: Constants.black
		startX: 0
		startY: 0

		PathLine {
			x: 0
			y: height
		}
		PathLine {
			x: pointerXRatio * width
			y: pointerYRatio * height
		}
		PathLine {
			x: 0
			y: 0
		}
	}
}
