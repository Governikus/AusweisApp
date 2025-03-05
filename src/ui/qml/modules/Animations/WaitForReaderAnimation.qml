/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQml.Models

SymbolAnimation {
	source: "qrc:///animations/wait_for_reader.svg"

	model: ListModel {
		ListElement {
			sx: 0.85
			sy: 0.72
		}
	}
}
