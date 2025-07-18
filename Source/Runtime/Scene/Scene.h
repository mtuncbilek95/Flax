/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2024 Metehan Tuncbilek
 */
#pragma once

#include <Runtime/Core/CoreMinimal.h>

#include <entt/entt.hpp>

namespace Flax
{
	class ComponentResolver;
	class Entity;
	class IComponentBase;

	class Scene
	{
		friend class ComponentResolver;

	public:
		Scene(const String& name = "DefaultScene");
		~Scene();

		Entity* AddEntity(Entity* parent = nullptr);
		void RemoveEntity(Entity* entity);

		const String& Name() const { return m_name; }
		Entity* Child(usize index) const;
		Entity* Root() const { return m_rootEntity.get(); }
		usize Count() const;

		entt::registry& Registry() { return m_registry; }

	private:
		entt::registry m_registry;
		Ref<Entity> m_rootEntity;

		String m_name;
	};
}