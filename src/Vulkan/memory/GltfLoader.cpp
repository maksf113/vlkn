#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "vulkan/memory/GltfLoader.hpp"

#include <vulkan/vulkan.h>

namespace vk
{
	std::vector<std::unique_ptr<Mesh>> GltfLoader::loadFromFile(const std::string& filePath, const std::shared_ptr<Device>& device, VkCommandPool commandPool)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret;
		if (filePath.substr(filePath.size() - 4) == ".glb")
		{
			ret = loader.LoadBinaryFromFile(&model, &err, &warn, filePath);
		}
		else
		{
			ret = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
		}
		if (!warn.empty())
		{
			std::cout << "Warn: " << warn << std::endl;
		}
		if (!err.empty())
		{
			std::cerr << "Err: " << err << std::endl;
		}
		if (!ret)
		{
			throw std::runtime_error("Failed to load glTF model");
		}
		std::vector<std::unique_ptr<Mesh>> meshes;
		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				// vertices
				std::vector<Vertex> vertices;
				// accessors
				const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
				const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				// buffer views
				const tinygltf::BufferView& positionView = model.bufferViews[positionAccessor.bufferView];
				const tinygltf::BufferView& normalView = model.bufferViews[normalAccessor.bufferView];
				const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
				const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
				//buffers
				const tinygltf::Buffer& positionBuffer = model.buffers[positionView.buffer];
				const tinygltf::Buffer& normalBuffer = model.buffers[normalView.buffer];
				const tinygltf::Buffer& tangentBuffer = model.buffers[tangentView.buffer];
				const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];
				// data pointers
				const float* positionData = reinterpret_cast<const float*>(positionBuffer.data.data() + positionView.byteOffset + positionAccessor.byteOffset);
				const float* normalData = reinterpret_cast<const float*>(normalBuffer.data.data() + normalView.byteOffset + normalAccessor.byteOffset);
				const float* tangentData = reinterpret_cast<const float*>(tangentBuffer.data.data() + tangentView.byteOffset + tangentAccessor.byteOffset);
				const float* uvData = reinterpret_cast<const float*>(uvBuffer.data.data() + uvView.byteOffset + uvAccessor.byteOffset);

				for (size_t i = 0; i < positionAccessor.count; ++i)
				{
					Vertex vertex{};

					vertex.position[0] = positionData[i * 3 + 0];
					vertex.position[1] = positionData[i * 3 + 1];
					vertex.position[2] = positionData[i * 3 + 2];

					vertex.normal[0] = normalData[i * 3 + 0];
					vertex.normal[1] = normalData[i * 3 + 1];
					vertex.normal[2] = normalData[i * 3 + 2];

					vertex.tangent[0] = tangentData[i * 4 + 0];
					vertex.tangent[1] = tangentData[i * 4 + 1];
					vertex.tangent[2] = tangentData[i * 4 + 2];

					vertex.uv[0] = uvData[i * 2 + 0];
					vertex.uv[1] = uvData[i * 2 + 1];

					vertices.push_back(vertex);
				}

				// indices
				std::vector<uint32_t> indices;

				// accessor
				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				// buffer view
				const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
				//buffer
				const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];
				// data uint32_t or uint16_t
				if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					const uint32_t* uint32IndexData = reinterpret_cast<const uint32_t*>(indexBuffer.data.data() + indexView.byteOffset + indexAccessor.byteOffset);
					for (size_t i = 0; i < indexAccessor.count; ++i)
					{
						indices.push_back(uint32IndexData[i]);
					}
				}
				else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					const uint16_t* uint16IndexData = reinterpret_cast<const uint16_t*>(indexBuffer.data.data() + indexView.byteOffset + indexAccessor.byteOffset);
					for (size_t i = 0; i < indexAccessor.count; ++i)
					{
						indices.push_back(static_cast<uint32_t>(uint16IndexData[i]));
					}
				}
				else
				{
					throw std::runtime_error("Unsupported index component type");
				}

				meshes.push_back(std::make_unique<Mesh>(vertices, indices, device, commandPool));
			}
		}
		return meshes;
	}
}