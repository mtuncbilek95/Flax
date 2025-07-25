/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2024 Metehan Tuncbilek
 */
#pragma once

#include <Editor/Core/CoreMinimal.h>

namespace Flax
{
	class SceneGraphController final : public ControllerBase
	{
		Q_OBJECT
	public:
		void InitializeController() override final;
		void ShutdownController() override final;

	public slots:
		void onSceneLoadRequested(const QString& filePath);
	};
}
