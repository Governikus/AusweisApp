/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

EnterPasswordAnimation {
	id: root

	animated: false
	source: "qrc:///animations/result_changepin.svg"
	symbol: Symbol.Type.STAR
	symbolCount: 6
	symbolDistance: 0.128
	symbolSize: 0.11
	symbolX: 0.10
	symbolY: 0.545

	Symbol {
		id: check

		sourceSize.height: parent.height * 0.24
		type: Symbol.Type.CHECK

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: root.width * 0.42
			verticalCenter: parent.top
			verticalCenterOffset: root.height * 0.26
		}
	}
}
