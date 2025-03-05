/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQml.Models

SymbolAnimation {
	animated: false
	source: "qrc:///animations/wait_for_sac.svg"
	symbol.type: Symbol.Type.ERROR

	model: ListModel {
		ListElement {
			sx: 0.815
			sy: 0.645
		}
	}
}
