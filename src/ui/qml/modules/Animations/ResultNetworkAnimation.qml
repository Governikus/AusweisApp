/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

SymbolAnimation {
	animated: false
	source: "qrc:///animations/result_network.svg"
	symbol.type: Symbol.Type.BLOCK

	model: ListModel {
		ListElement {
			sx: 0.76
			sy: 0.72
		}
	}
}
