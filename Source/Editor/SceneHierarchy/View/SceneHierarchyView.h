/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2024 Metehan Tuncbilek
 */
#pragma once

#include <Runtime/Core/CoreMinimal.h>

namespace Flax
{
	class SceneHierarchyModel;
	class SceneHierarchyViewModel;

	class SceneHierarchyView : public QWidget
	{
		Q_OBJECT
	public:
		SceneHierarchyView(QWidget* pParent = nullptr);
		~SceneHierarchyView();



	private:
		QTreeView* m_view;
		SceneHierarchyViewModel* m_viewModel;
	};
}