/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style

SymbolAnimation {
	animated: false
	source: symbol.type === Symbol.Type.WARNING ? "qrc:///animations/status_warning.svg" : "qrc:///animations/status_normal.svg"
	tintColor: {
		switch (symbol.type) {
		case Symbol.Type.ERROR:
			return Style.color.error;
		case Symbol.Type.INFO:
			return Style.color.image;
		case Symbol.Type.WARNING:
			return Style.color.warning;
		default:
			return Style.color.success;
		}
	}

	model: ListModel {
		ListElement {
			sx: 0.5
			sy: 0.5
		}
	}

	symbol {
		maxSize: 0.65
		tintEnabled: true
		type: Symbol.Type.CHECK
	}
}
