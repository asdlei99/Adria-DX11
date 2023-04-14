#pragma once
#include <memory>
#include <DirectXCollision.h>
#include "Enums.h"
#include "Terrain.h"
#include "../Core/Definitions.h"
#include "../Math/Constants.h"
#include "../Graphics/GfxVertexTypes.h"
#include "../Graphics/GfxBuffer.h"
#include "../Graphics/TextureManager.h"
#include "../tecs/entity.h"

#define COMPONENT 

namespace adria
{
	struct COMPONENT Transform
	{
		DirectX::XMMATRIX starting_transform = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX current_transform = DirectX::XMMatrixIdentity();
	};

	struct COMPONENT Relationship
	{
		static constexpr size_t MAX_CHILDREN = 2048;
		tecs::entity parent = tecs::null_entity;
		size_t children_count = 0;
		tecs::entity children[MAX_CHILDREN] = { tecs::null_entity };
	};

	struct COMPONENT Mesh
	{
		std::shared_ptr<GfxBuffer>	vertex_buffer = nullptr;
		std::shared_ptr<GfxBuffer>	index_buffer = nullptr;
		std::shared_ptr<GfxBuffer>   instance_buffer = nullptr;

		//only vb
		uint32 vertex_count = 0;
		uint32 start_vertex_location = 0; //Index of the first vertex

		//vb/ib
		uint32 indices_count = 0;
		uint32 start_index_location = 0; //The location of the first index read by the GPU from the index buffer
		int32 base_vertex_location = 0;  //A value added to each index before reading a vertex from the vertex buffer

		//instancing
		uint32 instance_count = 0;
		uint32 start_instance_location = 0; //A value added to each index before reading per-instance data from a vertex buffer

		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		void Draw(ID3D11DeviceContext* context) const
		{
			context->IASetPrimitiveTopology(topology);
			BindVertexBuffer(context, vertex_buffer.get());
			if (index_buffer)
			{
				BindIndexBuffer(context, index_buffer.get());
				if (instance_buffer)
				{
					BindVertexBuffer(context, instance_buffer.get(), 1);
					context->DrawIndexedInstanced(indices_count, instance_count, 
						start_index_location, base_vertex_location, start_instance_location);
				}
				else context->DrawIndexed(indices_count, start_index_location, base_vertex_location);
			}
			else
			{
				if (instance_buffer)
				{
					BindVertexBuffer(context, instance_buffer.get(), 1);
					context->DrawInstanced(vertex_count, instance_count,
						start_vertex_location, start_instance_location);
				}
				else context->Draw(vertex_count, start_vertex_location);
			}
		}
		void Draw(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY override_topology) const
		{
			context->IASetPrimitiveTopology(override_topology);
			BindVertexBuffer(context, vertex_buffer.get());
			if (index_buffer)
			{
				BindIndexBuffer(context, index_buffer.get());
				if (instance_buffer)
				{
					BindVertexBuffer(context, instance_buffer.get(), 1);
					context->DrawIndexedInstanced(indices_count, instance_count,
						start_index_location, base_vertex_location, start_instance_location);
				}
				else context->DrawIndexed(indices_count, start_index_location, base_vertex_location);
			}
			else
			{
				if (instance_buffer)
				{
					BindVertexBuffer(context, instance_buffer.get(), 1);
					context->DrawInstanced(vertex_count, instance_count,
						start_vertex_location, start_instance_location);
				}
				else context->Draw(vertex_count, start_vertex_location);
			}
		}
	};

	struct COMPONENT Material
	{
		TextureHandle albedo_texture			  = INVALID_TEXTURE_HANDLE;
		TextureHandle normal_texture			  = INVALID_TEXTURE_HANDLE;
		TextureHandle metallic_roughness_texture  = INVALID_TEXTURE_HANDLE;
		TextureHandle emissive_texture			  = INVALID_TEXTURE_HANDLE;

		float32 albedo_factor		= 1.0f;
		float32 metallic_factor		= 1.0f;
		float32 roughness_factor	= 1.0f;
		float32 emissive_factor		= 1.0f;

		EMaterialAlphaMode alpha_mode = EMaterialAlphaMode::Opaque;
		float32 alpha_cutoff		= 0.5f;
		bool    double_sided		= false;

		DirectX::XMFLOAT3 diffuse = DirectX::XMFLOAT3(1, 1, 1);
		EShaderProgram shader = EShaderProgram::Unknown;
	};

	struct COMPONENT Light
	{
		DirectX::XMVECTOR position	= DirectX::XMVectorSet(0, 10, 0, 1);
		DirectX::XMVECTOR direction	= DirectX::XMVectorSet(0, -1, 0, 0);
		DirectX::XMVECTOR color		= DirectX::XMVectorSet(1, 1, 1, 1);
		float32 energy = 1.0f;
		float32 range = 100.0f;
		ELightType type = ELightType::Directional;
		float32 outer_cosine = cos(pi<float32> / 4);
		float32 inner_cosine = cos(pi<float32> / 8);
		bool casts_shadows = false;
		bool use_cascades = false;
		bool active = true;
		bool volumetric = false;
		float32 volumetric_strength = 1.0f;
		bool screen_space_contact_shadows = false;
		float32 sscs_thickness = 0.5f;
		float32 sscs_max_ray_distance = 0.05f;
		float32 sscs_max_depth_distance = 200.0f;
		bool lens_flare = false;
		bool god_rays = false;
		float32 godrays_decay = 0.825f;
		float32 godrays_weight = 0.25f;
		float32 godrays_density = 0.975f;
		float32 godrays_exposure = 2.0f;
	};

	struct COMPONENT AABB
	{
		DirectX::BoundingBox bounding_box;
		bool camera_visible = true;
		bool light_visible = true;
		bool skip_culling = false;
		bool draw_aabb = false;
		std::shared_ptr<GfxBuffer> aabb_vb = nullptr;

		void UpdateBuffer(GfxDevice* gfx)
		{
			DirectX::XMFLOAT3 corners[8];
			bounding_box.GetCorners(corners);
			SimpleVertex vertices[] =
			{
				SimpleVertex{corners[0]},
				SimpleVertex{corners[1]},
				SimpleVertex{corners[2]},
				SimpleVertex{corners[3]},
				SimpleVertex{corners[4]},
				SimpleVertex{corners[5]},
				SimpleVertex{corners[6]},
				SimpleVertex{corners[7]}
			}; 
			aabb_vb = std::make_unique<GfxBuffer>(gfx, VertexBufferDesc(ARRAYSIZE(vertices), sizeof(SimpleVertex)), vertices);
		}
	};

	struct COMPONENT RenderState
	{
		EBlendState blend_state = EBlendState::None;
		EDepthState depth_state = EDepthState::None;
		ERasterizerState raster_state = ERasterizerState::None;
	};

	struct COMPONENT Skybox 
	{
		TextureHandle cubemap_texture = INVALID_TEXTURE_HANDLE;
		bool active = false;
	};

	struct COMPONENT Ocean {};

	struct COMPONENT Foliage {};

	struct COMPONENT Deferred {};

	struct COMPONENT TerrainComponent
	{
		inline static std::unique_ptr<Terrain> terrain;
		inline static DirectX::XMFLOAT2 texture_scale;
		TextureHandle sand_texture = INVALID_TEXTURE_HANDLE;
		TextureHandle grass_texture = INVALID_TEXTURE_HANDLE;
		TextureHandle rock_texture = INVALID_TEXTURE_HANDLE;
		TextureHandle base_texture = INVALID_TEXTURE_HANDLE;
		TextureHandle layer_texture = INVALID_TEXTURE_HANDLE;
	};

	struct COMPONENT Emitter
	{
		TextureHandle		particle_texture = INVALID_TEXTURE_HANDLE;
		DirectX::XMFLOAT4	position = DirectX::XMFLOAT4(0, 0, 0, 0);
		DirectX::XMFLOAT4	velocity = DirectX::XMFLOAT4(0, 5, 0, 0);
		DirectX::XMFLOAT4	position_variance = DirectX::XMFLOAT4(0, 0, 0, 0);
		int32					number_to_emit = 0;
		float32					particle_lifespan = 5.0f;
		float32					start_size = 10.0f;
		float32					end_size = 1.0f;
		float32					mass = 1.0f;
		float32					velocity_variance = 1.0f;
		float32					particles_per_second = 100;
		float32					accumulation = 0.0f;
		float32					elapsed_time = 0.0f;
		bool				collisions_enabled = false;
		int32					collision_thickness = 40;
		bool				alpha_blended = true;
		bool				pause = false;
		bool				sort = false;
		mutable bool		reset_emitter = true;
	};

	struct COMPONENT Decal
	{
		TextureHandle albedo_decal_texture = INVALID_TEXTURE_HANDLE;
		TextureHandle normal_decal_texture = INVALID_TEXTURE_HANDLE;
		DirectX::XMMATRIX decal_model_matrix = DirectX::XMMatrixIdentity();
		EDecalType decal_type = EDecalType::Project_XY;
		bool modify_gbuffer_normals = false;
	};

	struct COMPONENT Forward 
	{
		bool transparent;
	};

	struct COMPONENT Tag
	{
		std::string name = "default";
	};
}