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
	struct IAssetMetaExtension;

	struct IAssetImporter
	{
		virtual ~IAssetImporter() = default;

		virtual String AssetType() const = 0;
		virtual void Import(const Path& virtualPath) = 0;
	};
}
