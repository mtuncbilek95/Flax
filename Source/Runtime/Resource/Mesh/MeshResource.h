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
	class VBuffer;

	class MeshResource
	{
	public:
		MeshResource() = default;
		~MeshResource() = default;

		void Load(const String& path);
		void Unload();

		operator Ref<VBuffer>() { return m_GpuBuffer; }

	private:
		Ref<VBuffer> m_GpuBuffer;
	};
}