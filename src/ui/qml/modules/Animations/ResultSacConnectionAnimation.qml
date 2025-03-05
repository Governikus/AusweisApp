/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

SymbolAnimation {
	animated: false
	source: "qrc:///animations/result_sac_connection.svg"
	symbol.type: Symbol.Type.ERROR

	model: ListModel {
		ListElement {
			sx: 0.075
			sy: 0.665
		}
		ListElement {
			sx: 0.445
			sy: 0.665
		}
		ListElement {
			sx: 0.765
			sy: 0.38
		}
	}
}
