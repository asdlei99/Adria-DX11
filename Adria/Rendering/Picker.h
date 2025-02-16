#pragma once
#include <memory>
#include "ShaderManager.h"
#include "Graphics/GfxBuffer.h"
#include "Graphics/GfxDevice.h" 
#include "Graphics/GfxCommandContext.h" 
#include "Graphics/GfxShaderProgram.h" 

namespace adria
{

	struct PickingData
	{
		Vector4 position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		Vector4 normal = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	};

	class Picker
	{
		friend class Renderer;
		
	private:

		Picker(GfxDevice* gfx) : gfx(gfx), picking_buffer(nullptr)
		{
			GfxBufferDesc desc{};
			desc.size = sizeof(PickingData);
			desc.stride = sizeof(PickingData);
			desc.cpu_access = GfxCpuAccess::Read;
			desc.misc_flags = GfxBufferMiscFlag::BufferStructured;
			desc.resource_usage = GfxResourceUsage::Default;
			desc.bind_flags = GfxBindFlag::UnorderedAccess;
			picking_buffer = std::make_unique<GfxBuffer>(gfx, desc);
			picking_buffer->CreateUAV();
		}

		PickingData Pick(GfxShaderResourceRO depth_srv, GfxShaderResourceRO normal_srv)
		{
			GfxCommandContext* command_context = gfx->GetCommandContext();
			GfxShaderResourceRO srvs[2] = { depth_srv, normal_srv };
			command_context->SetShaderResourcesRO(GfxShaderStage::CS, 0, srvs);
			GfxShaderResourceRW lights_uav = picking_buffer->UAV();
			command_context->SetShaderResourceRW(0, lights_uav);
			
			ShaderManager::GetShaderProgram(ShaderProgram::Picker)->Bind(command_context);
			command_context->Dispatch(1, 1, 1);

			command_context->UnsetShaderResourcesRO(GfxShaderStage::CS, 0, ARRAYSIZE(srvs));
			command_context->SetShaderResourceRW(0, nullptr);

			PickingData const* data = (PickingData const*)picking_buffer->MapForRead();
			PickingData picking_data = *data;
			picking_buffer->Unmap();
			return picking_data;
		}

	private:

		GfxDevice* gfx;
		std::unique_ptr<GfxBuffer> picking_buffer;
	};
}