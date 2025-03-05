/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

SymbolAnimation {
	animated: false
	source: "qrc:///animations/result_nfc.svg"
	symbol.type: Symbol.Type.ERROR

	model: ListModel {
		ListElement {
			sx: 0.815
			sy: 0.645
		}
	}
}
