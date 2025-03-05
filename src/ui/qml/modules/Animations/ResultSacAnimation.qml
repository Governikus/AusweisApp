/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

SymbolAnimation {
	animated: false
	source: "qrc:///animations/result_sac.svg"
	symbol.type: Symbol.Type.ERROR

	model: ListModel {
		ListElement {
			sx: 0.68
			sy: 0.53
		}
	}
}
