/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

SymbolAnimation {
	animated: false
	source: "qrc:///animations/result_card.svg"
	symbol.type: Symbol.Type.BLOCK

	model: ListModel {
		ListElement {
			sx: 0.81
			sy: 0.7
		}
	}
}
