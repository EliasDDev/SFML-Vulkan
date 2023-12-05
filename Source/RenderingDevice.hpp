#pragma once

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <vulkan/vulkan.hpp>

struct VulkanShader
{
	vk::PipelineLayout PipelineLayout = {};
	vk::Pipeline Pipeline = {};
};

struct VulkanBuffer
{
	vk::Buffer Buffer = {};
	vk::DeviceMemory Memory = {};
};

struct VulkanImage
{
	vk::Image Image = {};
	vk::DeviceMemory Memory = {};
};

class RenderingDevice
{
public:
	static void Initialize(sf::WindowBase* window);
	static void Terminate();

	static VulkanShader CreateShader(const sf::String& vsFilePath, const sf::String& fsFilePath);
	static void DestroyShader(VulkanShader vulkanShader);

	static VulkanBuffer CreateVertexBuffer(const std::vector<sf::Vector3f>& vertices);
	static void DestroyVertexBuffer(VulkanBuffer vertexBuffer);

	static void SetViewport(sf::Vector2f position, sf::Vector2f size);
	static void SetScissors(sf::Vector2i offset, sf::Vector2i extent);

	static void BindShader(VulkanShader& vulkanShader);
	static void BindVertexBuffer(VulkanBuffer& vertexBuffer);

	static void Draw(sf::Uint32 count);

	static void RecreateSwapchain();
	static void BeginRenderPass();
	static void EndRenderPass();
	static void Present();
private:
	RenderingDevice();
	RenderingDevice(const RenderingDevice&);

	static void CreateInstance();
	static void CreateSurface();
	static vk::PhysicalDevice FindPhysicalDevice();
	static sf::Uint32 FindQueueFamily(vk::QueueFlags queueFlags);
	static void CreateDevice();
	static void CreateSwapchain();
	static void CreateRenderPass();
	static void CreateCommandPool();
	static void CreateSynchronization();

	static void DestroySwapchain();
	static void DestroyAll();

	static vk::CommandBuffer BeginSingleTimeCommands();
	static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);

	static VulkanBuffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	static void DestroyBuffer(VulkanBuffer buffer);

	static VulkanImage CreateImage(sf::Uint32 width, sf::Uint32 height, vk::Format format, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
	static void DestroyImage(VulkanImage image);

	static vk::ImageView CreateImageView(vk::Image image, vk::Format format);
	static vk::Framebuffer CreateFramebuffer(vk::ImageView imageView, sf::Uint32 width, sf::Uint32 height);
	static vk::CommandBuffer AllocateCommandBuffer();

	static sf::Uint32 FindMemoryType(sf::Uint32 suitableTypes, vk::MemoryPropertyFlags properties);
	static void ChangeImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
};
