/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2024 Metehan Tuncbilek
 */
#pragma once

#include <Runtime/Data/Definitions/Definitions.h>
#include <Runtime/Data/Definitions/StdNames.h>
#include <Runtime/Data/Logger/Logger.h>
#include <Runtime/Data/Service/RuntimeServiceBase.h>

namespace Flax
{
	class RuntimeService
	{
	public:
		template<typename T, typename = std::enable_if_t<std::is_base_of_v<RuntimeServiceBase, T>>>
		static void Register(Ref<T> service)
		{
			auto typeId = GetUniqueTypeId<T>();
			Services()[typeId] = service;

			Log::Debug(LogType::IO, "Service '{}' registered successfully.", typeid(T).name());
		}

		template<typename T>
		static Ref<T> Get()
		{
			auto typeId = GetUniqueTypeId<T>();
			auto it = Services().find(typeId);

			if (it == Services().end())
			{
				//Log::Critical(LogType::IO, "Service '{}' could not be found!", typeid(T).name());
				return nullptr;
			}

			return std::static_pointer_cast<T>(it->second);
		}

		static void InitializeServices()
		{
			auto& sVices = Services();

			for (auto& el : sVices)
				el.second->InitializeServiceField();
		}

		static void ClearServices()
		{
			auto& sVices = Services();

			for (auto& el : sVices)
				el.second->ResetServiceField();

			sVices.clear();
		}

	private:
		static HashMap<i32, Ref<RuntimeServiceBase>>& Services()
		{
			static HashMap<i32, Ref<RuntimeServiceBase>> instanceMap;
			return instanceMap;
		}
	};
}
