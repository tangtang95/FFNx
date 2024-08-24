/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "log.h"
#include "cfg.h"
#include "utils.h"

#include "external_mesh.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

bool ExternalMesh::importExternalMeshGltfFile(char* file_path, char* tex_path)
{
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, file_path, &data);
	if (result != cgltf_result_success)
	{
		return false;
	}

	result = cgltf_load_buffers(&options, data, file_path);
	if (result != cgltf_result_success)
	{
		return false;
	}

    std::string modelPath = file_path;
    std::string modelFolder = modelPath.substr(0, modelPath.find_last_of("/") + 1);
    std::string modelFilename =  modelPath.substr(modelPath.find_last_of("/") + 1);
    std::string modelFilenameWithoutExt =  modelFilename.substr(0, modelFilename.find_last_of("."));
    std::string configPath = modelFolder + modelFilenameWithoutExt + "_config.toml";
    loadConfig(configPath);

	for (size_t i = 0; i < data->textures_count; i++)
	{
		auto texture = data->textures[i];
		std::string relativePath = texture.image->uri;

		std::string filename = relativePath.substr(relativePath.find_last_of("/") + 1);
		std::string name = filename.substr(0, filename.find_last_of("."));

		std::string texFullPath = tex_path + name + ".dds";

        std::string modPath = !override_mod_path.empty() ? override_mod_path : mod_path;

        Material candidateMaterial;

        auto pair = materials.emplace(name, candidateMaterial);
        if(pair.second)
        {
            auto& material = (*pair.first).second;
            uint32_t width, height, mipCount = 0;

            char full_tex_path[512];

            int texCount = getTextureCount(name);
            material.frameInterval = getFrameInterval(name);
            for (int texIndex = 0; texIndex < texCount; ++texIndex)
            {
                if (texIndex != 0)
                {
                    auto nameWithoutNumber = name.substr(0, name.length() - 1);
                    _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s%s_00.dds", basedir, modPath.c_str(), nameWithoutNumber.data(), std::to_string(texIndex + 1).data());
                }
                else
                    _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s_00.dds", basedir, modPath.c_str(), name.data());

                auto textureHandle = newRenderer.createTextureHandle(full_tex_path, &width, &height, &mipCount);
                if (!textureHandle.idx)
                {
                    if (texIndex != 0)
                        _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s%s.dds", basedir, modPath.c_str(), name.data(), std::to_string(texIndex + 1).data());
                    else
                        _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s.dds", basedir, modPath.c_str(), name.data());

                    textureHandle = newRenderer.createTextureHandle(texFullPath.data(), &width, &height, &mipCount);
                    if (!textureHandle.idx) textureHandle = BGFX_INVALID_HANDLE;
                }

                if(bgfx::isValid(textureHandle))
                {
                    material.baseColorTexHandles.push_back(textureHandle);
                }
            }

            std::string nmlTexFullPath = tex_path + name + "_nml.dds";
            auto nmlTextureHandle = newRenderer.createTextureHandle(nmlTexFullPath.data(), &width, &height, &mipCount, false);
            if (!nmlTextureHandle.idx) nmlTextureHandle = BGFX_INVALID_HANDLE;
            if(bgfx::isValid(nmlTextureHandle))
            {
                material.normalTexHandles.push_back(nmlTextureHandle);
            }

            std::string pbrTexFullPath = tex_path + name + "_pbr.dds";
            auto pbrTextureHandle = newRenderer.createTextureHandle(pbrTexFullPath.data(), &width, &height, &mipCount, false);
            if (!pbrTextureHandle.idx) pbrTextureHandle = BGFX_INVALID_HANDLE;
            if(bgfx::isValid(pbrTextureHandle))
            {
                material.pbrTexHandles.push_back(pbrTextureHandle);
            }
        }
	}

	for (size_t i = 0; i < data->meshes_count; i++)
	{
		cgltf_mesh mesh = data->meshes[i];

		for (size_t j = 0; j < mesh.primitives_count; j++)
		{
			Shape outShape;

			cgltf_primitive primitive = mesh.primitives[j];
			auto indexCount = primitive.indices->count;
			auto vertexCount = 0;

			float* posBuffer = nullptr;
			float* normalBuffer = nullptr;
			float* uvBuffer = nullptr;
			float* colorBuffer = nullptr;
			for (size_t k = 0; k < primitive.attributes_count; k++)
			{
				cgltf_attribute attr = primitive.attributes[k];

				if(strcmp(attr.name, "POSITION") == 0)
				{
					vertexCount = attr.data->count;
					posBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
					outShape.min.x = attr.data->min[0];
					outShape.min.y = attr.data->min[1];
					outShape.min.z = attr.data->min[2];
					outShape.max.x = attr.data->max[0];
					outShape.max.y = attr.data->max[1];
					outShape.max.z = attr.data->max[2];
				}
				else if(strcmp(attr.name, "NORMAL") == 0)
				{
					normalBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
				else if(strcmp(attr.name, "TEXCOORD_0") == 0)
				{
					uvBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
				else if(strcmp(attr.name, "COLOR_0") == 0)
				{
					colorBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
			}

            outShape.isDoubleSided = primitive.material->double_sided;

			auto texture = primitive.material->pbr_metallic_roughness.base_color_texture.texture;
			if(texture != nullptr)
			{
				std::string texName = texture->image->name;
				if(materials.contains(texName)) outShape.pMaterial = &materials[texName];
			}
			auto baseColorFactor = primitive.material->pbr_metallic_roughness.base_color_factor;
			for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				struct nvertex vertex;
				vertex._.x = posBuffer[3 * vertexIndex];
				vertex._.y = posBuffer[3 * vertexIndex + 2];
				vertex._.z = posBuffer[3 * vertexIndex + 1];

				vertex.color.w = 1.0f;
				vertex.color.r = static_cast<char>(baseColorFactor[0] * 255);
				vertex.color.g = static_cast<char>(baseColorFactor[1] * 255);
				vertex.color.b = static_cast<char>(baseColorFactor[2] * 255);
				vertex.color.a = static_cast<char>(baseColorFactor[3] * 255);

                if (uvBuffer != nullptr)
                {
				    vertex.u = uvBuffer[2 * vertexIndex];
				    vertex.v = uvBuffer[2 * vertexIndex + 1];
                }
                else
                {
                    vertex.u = 0.0f;
				    vertex.v = 0.0f;
                }

				outShape.vertices.push_back(vertex);

				struct vector3<float> normal;

				normal.x = normalBuffer[3 * vertexIndex];
				normal.y = normalBuffer[3 * vertexIndex + 2];
				normal.z = normalBuffer[3 * vertexIndex + 1];

				outShape.normals.push_back(normal);
			}

			if(primitive.indices->component_type == cgltf_component_type_r_16u)
			{
				auto indexBuffer = (unsigned short*)((char*)primitive.indices->buffer_view->buffer->data + primitive.indices->buffer_view->offset);

				for (int id = 0; id < indexCount; ++id)
				{
					outShape.indices.push_back(indexBuffer[id]);
				}
			}else if(primitive.indices->component_type == cgltf_component_type_r_32u)
			{
				auto indexBuffer = (unsigned int*)((char*)primitive.indices->buffer_view->buffer->data + primitive.indices->buffer_view->offset);
				for (int id = 0; id < indexCount; ++id)
				{
					outShape.indices.push_back(indexBuffer[id]);
				}
			}

            fillExternalMeshVertexBuffer(outShape.vertices.data(), outShape.normals.data(), outShape.vertices.size());
            fillExternalMeshIndexBuffer(outShape.indices.data(), outShape.indices.size());

            shapes.push_back(outShape);
		}
	}

    updateExternalMeshBuffers();

    cgltf_free(data);

	return true;
}

uint32_t ExternalMesh::fillExternalMeshVertexBuffer(struct nvertex* inVertex, struct vector3<float>* normals, uint32_t inCount)
{
    if (!bgfx::isValid(vertexBufferHandle)) vertexBufferHandle = bgfx::createDynamicVertexBuffer(inCount, newRenderer.GetVertexLayout(), BGFX_BUFFER_ALLOW_RESIZE);

    uint32_t currentOffset = vertexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        vertexBufferData.push_back(Vertex());

        vertexBufferData[currentOffset + idx].x = inVertex[idx]._.x;
        vertexBufferData[currentOffset + idx].y = inVertex[idx]._.y;
        vertexBufferData[currentOffset + idx].z = inVertex[idx]._.z;
        vertexBufferData[currentOffset + idx].w = (::isinf(inVertex[idx].color.w) ? 1.0f : inVertex[idx].color.w);
        vertexBufferData[currentOffset + idx].bgra = inVertex[idx].color.color;
        vertexBufferData[currentOffset + idx].u = inVertex[idx].u;
        vertexBufferData[currentOffset + idx].v = inVertex[idx].v;

        if (normals)
        {
            vertexBufferData[currentOffset + idx].nx = normals[idx].x;
            vertexBufferData[currentOffset + idx].ny = normals[idx].y;
            vertexBufferData[currentOffset + idx].nz = normals[idx].z;
        }

        if (vertex_log && idx == 0) ffnx_trace("%s: %u [XYZW(%f, %f, %f, %f), BGRA(%08x), UV(%f, %f)]\n", __func__, idx, vertexBufferData[currentOffset + idx].x, vertexBufferData[currentOffset + idx].y, vertexBufferData[currentOffset + idx].z, vertexBufferData[currentOffset + idx].w, vertexBufferData[currentOffset + idx].bgra, vertexBufferData[currentOffset + idx].u, vertexBufferData[currentOffset + idx].v);
        if (vertex_log && idx == 1) ffnx_trace("%s: See the rest on RenderDoc.\n", __func__);
    }

    return currentOffset;
};

uint32_t ExternalMesh::fillExternalMeshIndexBuffer(uint32_t* inIndex, uint32_t inCount)
{
    if (!bgfx::isValid(indexBufferHandle)) indexBufferHandle = bgfx::createDynamicIndexBuffer(inCount, BGFX_BUFFER_ALLOW_RESIZE | BGFX_BUFFER_INDEX32);

    uint32_t currentOffset = indexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        indexBufferData.push_back(inIndex[idx]);
    }

    return currentOffset;
};

void ExternalMesh::updateExternalMeshBuffers()
{
    bgfx::update(
        vertexBufferHandle,
        0,
        bgfx::copy(
            vertexBufferData.data(),
            vectorSizeOf(vertexBufferData)
        )
    );

    bgfx::update(
        indexBufferHandle,
        0,
        bgfx::copy(
            indexBufferData.data(),
            vectorSizeOf(indexBufferData)
        )
    );
}

void ExternalMesh::bindField3dVertexBuffer(uint32_t offset, uint32_t inCount)
{
    bgfx::setVertexBuffer(0, vertexBufferHandle, offset, inCount);
}

void ExternalMesh::bindField3dIndexBuffer(uint32_t offset, uint32_t inCount)
{
    bgfx::setIndexBuffer(indexBufferHandle, offset, inCount);
}

void ExternalMesh::clearExternalMesh3dBuffers()
{
    vertexBufferData.clear();
    vertexBufferData.shrink_to_fit();

    indexBufferData.clear();
    indexBufferData.shrink_to_fit();
}

void ExternalMesh::unloadExternalMesh()
{
    for (const auto& mat : materials)
    {
        for (const auto& tex : mat.second.baseColorTexHandles)
        {
            if (bgfx::isValid(tex))
                bgfx::destroy(tex);
        }
        for (const auto& tex : mat.second.normalTexHandles)
        {
            if (bgfx::isValid(tex))
                bgfx::destroy(tex);
        }
        for (const auto& tex : mat.second.pbrTexHandles)
        {
            if (bgfx::isValid(tex))
                bgfx::destroy(tex);
        }
    }
    shapes.clear();
    materials.clear();
    clearExternalMesh3dBuffers();
}

void ExternalMesh::loadConfig(const std::string& path)
{
    try
    {
        config = toml::parse_file(path);
    }
    catch (const toml::parse_error &err)
    {
        config = toml::parse("");
    }
}

int ExternalMesh::getTextureCount(std::string tex_name)
{
    auto node = config[tex_name];
    if(node)
    {
        if (auto sub_node = node["num_textures"]) return sub_node.value_or(0);
    }

    return 1;
}

int ExternalMesh::getFrameInterval(std::string tex_name)
{
    auto node = config[tex_name];
    if(node)
    {
        if (auto sub_node = node["frame_interval"]) return sub_node.value_or(0);
    }

    return 0;
}
