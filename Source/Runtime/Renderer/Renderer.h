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
	class VInstance;
	class VDevice;
	class VSwapchain;
	class VQueue;
	class VCmdPool;
	class VCmdBuffer;
	class VSemaphore;
	class VFence;
	class VImage;
	class VImageView;
	class VRenderPass;
	class VFramebuffer;

	struct RendererProps final
	{
		Math::Vec2u rendererSize;
		void* windowHandle;
	};

	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

		void Initialize(const RendererProps& desc);
		void Run();
		void Stop();

		VDevice* GetDevice() const { return m_vkDevice.get(); }
		VRenderPass* GetMainPass() const { return m_presentPass.get(); }
		VQueue* GetGraphicsQueue() const { return m_vkGraphicsQueue.get(); }
		VQueue* GetTransferQueue() const { return m_vkTransferQueue.get(); }

	private:
		Ref<VInstance> m_vkInstance;
		Ref<VDevice> m_vkDevice;
		Ref<VSwapchain> m_vkSwapchain;

		Ref<VQueue> m_vkGraphicsQueue;
		Ref<VQueue> m_vkTransferQueue;

		Ref<VCmdPool> m_executionCmdPool;
		Vector<Ref<VCmdBuffer>> m_executionCmdBuffers;

		Vector<Ref<VSemaphore>> m_imageSemaphores;
		Vector<Ref<VSemaphore>> m_presentSemaphores;
		Vector<Ref<VFence>> m_inFlightFences;

		Ref<VFence> m_transferFence;

		Ref<VImage> m_depthImage;
		Ref<VImageView> m_depthView;

		Ref<VRenderPass> m_presentPass;
		Ref<VFramebuffer> m_presentFBO;

		u32 m_currentIndex = 0;
	};
}
