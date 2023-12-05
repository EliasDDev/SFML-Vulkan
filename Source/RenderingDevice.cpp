#include <cassert>
#include <iostream>
#include <array>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "RenderingDevice.hpp"

#ifdef DEBUG
static constexpr bool USE_VALIDATION_LAYERS = true;
#else
static constexpr bool USE_VALIDATION_LAYERS = false;
#endif

static constexpr std::array<const char*, 1> VALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};

static constexpr std::array<const char*, 1> DEVICE_EXTENSIONS = {
	"VK_KHR_swapchain"
};

static sf::WindowBase* s_Window = {};

static vk::Instance					s_Instance = {};
static vk::SurfaceKHR				s_Surface = {};
static vk::PhysicalDevice			s_PhysicalDevice = {};
static sf::Uint32					s_QueueFamilyIndex = {};
static vk::Device					s_Device = {};
static vk::Queue					s_Queue = {};
static vk::SurfaceCapabilitiesKHR	s_SurfaceCapabilities = {};
static vk::SurfaceFormatKHR			s_SurfaceFormat = {};
static vk::PresentModeKHR			s_PresentMode = {};
static vk::SwapchainKHR				s_Swapchain = {};
static std::vector<vk::ImageView>	s_ImageViews = {};
static vk::RenderPass				s_RenderPass = {};
static std::vector<vk::Framebuffer> s_Framebuffers = {};
static vk::CommandPool				s_CommandPool = {};
static vk::CommandBuffer			s_CommandBuffer = {};
static vk::DescriptorPool			s_DescriptorPool = {};
static sf::Uint32					s_SwapchainImageIndex = {};
static vk::Semaphore				s_ImageReadySemaphore = {};
static vk::Semaphore				s_RenderReadySemaphore = {};
static vk::Fence					s_WaitFrameFence = {};

void RenderingDevice::Initialize(sf::WindowBase* window)
{
	s_Window = window;
	assert(s_Window);

	if (!sf::Vulkan::isAvailable())
	{
		std::cerr << "Vulkan is NOT supported (Failed to find extensions)\n";
		s_Window->close();
		return;
	}

	CreateInstance();
	CreateSurface();

	s_PhysicalDevice = FindPhysicalDevice();
	if (!s_PhysicalDevice)
	{
		std::cerr << "Vulkan is NOT supported (Failed to find GPU)\n";
		s_Window->close();
		return;
	}

	s_QueueFamilyIndex = FindQueueFamily(vk::QueueFlagBits::eGraphics);
	CreateDevice();

	CreateSwapchain();

	CreateCommandPool();
	s_CommandBuffer = AllocateCommandBuffer();

	CreateSynchronization();
}

void RenderingDevice::Terminate()
{
	DestroyAll();
	s_Window = {};
}

VulkanShader RenderingDevice::CreateShader(const sf::String& vsFilePath, const sf::String& fsFilePath)
{
	VulkanShader vulkanShader = {};

	// Read vertex shader

	sf::FileInputStream vsFile = {};
	assert(vsFile.open(vsFilePath));

	sf::Int64 vsSize = vsFile.getSize();
	assert(vsSize != -1);

	std::vector<sf::Uint32> vsBuffer(vsSize);
	assert(vsFile.read(vsBuffer.data(), vsSize) == vsSize);

	// Read fragment shader

	sf::FileInputStream fsFile = {};
	assert(fsFile.open(fsFilePath));

	sf::Int64 fsSize = fsFile.getSize();
	assert(fsSize != -1);

	std::vector<sf::Uint32> fsBuffer(fsSize);
	assert(fsFile.read(fsBuffer.data(), fsSize) == fsSize);

	// Create vertex shader
	vk::ShaderModuleCreateInfo vertexModuleCreateInfo(vk::ShaderModuleCreateFlags(), vsBuffer.size(), vsBuffer.data());
	vk::ShaderModule vertexModule = s_Device.createShaderModule(vertexModuleCreateInfo);

	// Create fragment shader
	vk::ShaderModuleCreateInfo fragmentModuleCreateInfo(vk::ShaderModuleCreateFlags(), fsBuffer.size(), fsBuffer.data());
	vk::ShaderModule fragmentModule = s_Device.createShaderModule(fragmentModuleCreateInfo);

	// Pipeline stages
	vk::PipelineShaderStageCreateInfo vertexStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertexModule, "main");
	vk::PipelineShaderStageCreateInfo fragmentStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragmentModule, "main");
	std::array<vk::PipelineShaderStageCreateInfo, 2> stages = { vertexStageCreateInfo, fragmentStageCreateInfo };

	// Vertex input
	vk::VertexInputBindingDescription bindingDescription(0, sizeof(sf::Vector3f), vk::VertexInputRate::eVertex);
	vk::VertexInputAttributeDescription attributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0);

	// Input assembly
	vk::PipelineVertexInputStateCreateInfo vertexInputState(vk::PipelineVertexInputStateCreateFlags(), bindingDescription, attributeDescription);
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList, false);

	// Dynamic states
	vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(), 1, nullptr, 1, nullptr);
	std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicState(vk::PipelineDynamicStateCreateFlags(), dynamicStates);

	// Rasterizer & Multisampling
	vk::PipelineRasterizationStateCreateInfo rasterizationState(vk::PipelineRasterizationStateCreateFlags(), false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, false, 0.0f, 0.0f, 0.0f, 1.0f);
	vk::PipelineMultisampleStateCreateInfo multisampleState(vk::PipelineMultisampleStateCreateFlags(), vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false);

	// Color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment(true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	vk::PipelineColorBlendStateCreateInfo colorBlendState(vk::PipelineColorBlendStateCreateFlags(), false, vk::LogicOp::eCopy, colorBlendAttachment);

	// Create pipeline layout
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), nullptr, nullptr);
	vulkanShader.PipelineLayout = s_Device.createPipelineLayout(pipelineLayoutCreateInfo);

	// Create graphics pipeline
	vk::GraphicsPipelineCreateInfo pipelineCreateInfo(vk::PipelineCreateFlags(),
		stages,
		&vertexInputState,
		&inputAssemblyState,
		nullptr,
		&viewportState,
		&rasterizationState,
		&multisampleState,
		nullptr,
		&colorBlendState,
		&dynamicState,
		vulkanShader.PipelineLayout,
		s_RenderPass);

	vk::ResultValue<vk::Pipeline> result = s_Device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
	assert(result.result == vk::Result::eSuccess);
	vulkanShader.Pipeline = result.value;

	// Cleanup
	s_Device.destroyShaderModule(vertexModule);
	s_Device.destroyShaderModule(fragmentModule);

	return vulkanShader;
}

void RenderingDevice::DestroyShader(VulkanShader vulkanShader)
{
	s_Device.waitIdle();
	s_Device.destroyPipelineLayout(vulkanShader.PipelineLayout);
	s_Device.destroyPipeline(vulkanShader.Pipeline);
	vulkanShader.PipelineLayout = nullptr;
	vulkanShader.Pipeline = nullptr;
}

VulkanBuffer RenderingDevice::CreateVertexBuffer(const std::vector<sf::Vector3f>& vertices)
{
	// Calculate buffer size in bytes
	vk::DeviceSize size = sizeof(sf::Vector3f) * vertices.size();

	// Create buffer
	VulkanBuffer vertexBuffer = CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	// Map data
	void* data = s_Device.mapMemory(vertexBuffer.Memory, 0, size);
	{
		std::memcpy(data, vertices.data(), size);
	}
	s_Device.unmapMemory(vertexBuffer.Memory);

	return vertexBuffer;
}

void RenderingDevice::DestroyVertexBuffer(VulkanBuffer vertexBuffer)
{
	s_Device.waitIdle();
	s_Device.destroyBuffer(vertexBuffer.Buffer);
	s_Device.freeMemory(vertexBuffer.Memory);
	vertexBuffer.Buffer = nullptr;
	vertexBuffer.Memory = nullptr;
}

void RenderingDevice::SetViewport(sf::Vector2f position, sf::Vector2f size)
{
	vk::Viewport viewport(position.x, position.y, size.x, size.y, 0.0f, 1.0f);
	s_CommandBuffer.setViewport(0, viewport);
}

void RenderingDevice::SetScissors(sf::Vector2i offset, sf::Vector2i extent)
{
	vk::Rect2D scissor(vk::Offset2D(offset.x, offset.y), vk::Extent2D(extent.x, extent.y));
	s_CommandBuffer.setScissor(0, scissor);
}

void RenderingDevice::BindShader(VulkanShader& vulkanShader)
{
	s_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vulkanShader.Pipeline);
}

void RenderingDevice::BindVertexBuffer(VulkanBuffer& vertexBuffer)
{
	s_CommandBuffer.bindVertexBuffers(0, vertexBuffer.Buffer, { 0 });
}

void RenderingDevice::Draw(sf::Uint32 count)
{
	s_CommandBuffer.draw(count, 1, 0, 0);
}

void RenderingDevice::RecreateSwapchain()
{
	vk::Extent2D extent = s_PhysicalDevice.getSurfaceCapabilitiesKHR(s_Surface).currentExtent;
	while (extent.width == 0 || extent.height == 0)
	{
		sf::Event event = {};
		s_Window->waitEvent(event);
		extent = s_PhysicalDevice.getSurfaceCapabilitiesKHR(s_Surface).currentExtent;
	}

	DestroySwapchain();
	CreateSwapchain();
}

void RenderingDevice::BeginRenderPass()
{
	// Wait for previous frame
	while (s_Device.waitForFences(s_WaitFrameFence, true, UINT64_MAX) == vk::Result::eTimeout);
	s_Device.resetFences(s_WaitFrameFence);

	try
	{
		// Get image from swapchain
		s_SwapchainImageIndex = s_Device.acquireNextImageKHR(s_Swapchain, UINT64_MAX, s_ImageReadySemaphore).value;
	}
	catch (const vk::OutOfDateKHRError&) // Swapchain outdated
	{
		// Recreate swapchain
		RecreateSwapchain();
	}
	catch (const vk::SystemError& error) // Unexpected error happened
	{
		// Print error
		std::cerr << error.what() << "\n";
	}

	// Begin command buffer
	s_CommandBuffer.reset();
	s_CommandBuffer.begin(vk::CommandBufferBeginInfo());

	// Render area & Clear color
	vk::Rect2D renderArea(vk::Offset2D(0, 0), s_SurfaceCapabilities.currentExtent);
	vk::ClearValue clearValue(vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f));

	// Begin render pass
	vk::RenderPassBeginInfo renderPassBeginInfo(s_RenderPass, s_Framebuffers[s_SwapchainImageIndex], renderArea, clearValue);
	s_CommandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void RenderingDevice::EndRenderPass()
{
	// End render pass
	s_CommandBuffer.endRenderPass();

	// End command buffer
	s_CommandBuffer.end();

	// Ensures that the color output is finished rendering before continuing
	vk::PipelineStageFlags pipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	// Submit render commands to GPU
	vk::SubmitInfo submitInfo(s_ImageReadySemaphore, pipelineStageFlags, s_CommandBuffer, s_RenderReadySemaphore);
	s_Queue.submit(submitInfo, s_WaitFrameFence);
}

void RenderingDevice::Present()
{
	try
	{
		// Present the rendered image
		vk::Result result = s_Queue.presentKHR(vk::PresentInfoKHR(s_RenderReadySemaphore, s_Swapchain, s_SwapchainImageIndex));
	}
	catch (const vk::OutOfDateKHRError&) // Swapchain outdated
	{
		// Recreate swapchain
		RecreateSwapchain();
	}
	catch (const vk::SystemError& error) // Unexpected error happened
	{
		// Print error
		std::cerr << error.what() << "\n";
	}
}

void RenderingDevice::CreateInstance()
{
	vk::ApplicationInfo applicationInfo = {};
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions = sf::Vulkan::getGraphicsRequiredInstanceExtensions();

	vk::InstanceCreateInfo instanceCreateInfo(vk::InstanceCreateFlags(), &applicationInfo, nullptr, nullptr);
	if (USE_VALIDATION_LAYERS)
	{
		instanceCreateInfo.enabledLayerCount = (sf::Uint32)VALIDATION_LAYERS.size();
		instanceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	}

	instanceCreateInfo.enabledExtensionCount = (sf::Uint32)instanceExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

	s_Instance = vk::createInstance(instanceCreateInfo);
}

void RenderingDevice::CreateSurface()
{
	VkSurfaceKHR surface = {};
	assert(s_Window->createVulkanSurface(s_Instance, surface));
	s_Surface = surface;
}

vk::PhysicalDevice RenderingDevice::FindPhysicalDevice()
{
	return s_Instance.enumeratePhysicalDevices().front();
}

sf::Uint32 RenderingDevice::FindQueueFamily(vk::QueueFlags queueFlags)
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = s_PhysicalDevice.getQueueFamilyProperties();
	for (sf::Uint32 i = 0; i < queueFamilyProperties.size(); i++)
	{
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			return i;
		}
	}

	std::cerr << "Failed to find queue family\n";
	return UINT32_MAX;
}

void RenderingDevice::CreateDevice()
{
	const std::array<float, 1> queuePriorities = { 1.0f };
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), s_QueueFamilyIndex, queuePriorities);

	vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo, nullptr, nullptr);
	if (USE_VALIDATION_LAYERS)
	{
		deviceCreateInfo.enabledLayerCount = (sf::Uint32)VALIDATION_LAYERS.size();
		deviceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	}

	deviceCreateInfo.enabledExtensionCount = (sf::Uint32)DEVICE_EXTENSIONS.size();
	deviceCreateInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

	s_Device = s_PhysicalDevice.createDevice(deviceCreateInfo);

	s_Queue = s_Device.getQueue(s_QueueFamilyIndex, 0);
}

void RenderingDevice::CreateSwapchain()
{
	s_SurfaceCapabilities = s_PhysicalDevice.getSurfaceCapabilitiesKHR(s_Surface);

	s_SurfaceFormat.format = vk::Format::eB8G8R8A8Srgb;
	s_SurfaceFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

	for (const auto& presentMode : s_PhysicalDevice.getSurfacePresentModesKHR(s_Surface))
	{
		if (presentMode == vk::PresentModeKHR::eMailbox)
		{
			s_PresentMode = vk::PresentModeKHR::eMailbox;
			break;
		}

		s_PresentMode = vk::PresentModeKHR::eFifo;
	}

	vk::SwapchainCreateInfoKHR swapchainCreateInfo(vk::SwapchainCreateFlagsKHR(),
		s_Surface,
		s_SurfaceCapabilities.minImageCount,
		s_SurfaceFormat.format,
		s_SurfaceFormat.colorSpace,
		s_SurfaceCapabilities.currentExtent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		nullptr,
		s_SurfaceCapabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		s_PresentMode,
		true,
		nullptr);

	s_Swapchain = s_Device.createSwapchainKHR(swapchainCreateInfo);

	std::vector<vk::Image> swapchainImages = s_Device.getSwapchainImagesKHR(s_Swapchain);

	s_ImageViews.resize(swapchainImages.size());
	for (sf::Uint32 i = 0; i < s_ImageViews.size(); i++)
		s_ImageViews[i] = CreateImageView(swapchainImages[i], s_SurfaceFormat.format);

	CreateRenderPass();

	vk::Extent2D currentExtent = s_SurfaceCapabilities.currentExtent;

	s_Framebuffers.resize(s_ImageViews.size());
	for (sf::Uint32 i = 0; i < s_Framebuffers.size(); i++)
		s_Framebuffers[i] = CreateFramebuffer(s_ImageViews[i], currentExtent.width, currentExtent.height);
}

void RenderingDevice::CreateRenderPass()
{
	vk::AttachmentDescription colorAttachment(vk::AttachmentDescriptionFlags(),
		s_SurfaceFormat.format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::SubpassDescription subpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, nullptr, colorReference);

	vk::SubpassDependency subpassDependency(
		vk::SubpassExternal,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::AccessFlagBits::eNone,
		vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo renderPassCreateInfo(vk::RenderPassCreateFlags(), colorAttachment, subpassDescription, subpassDependency);
	s_RenderPass = s_Device.createRenderPass(renderPassCreateInfo);
}

void RenderingDevice::CreateCommandPool()
{
	vk::CommandPoolCreateInfo commandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, s_QueueFamilyIndex);
	s_CommandPool = s_Device.createCommandPool(commandPoolCreateInfo);
}

void RenderingDevice::CreateSynchronization()
{
	s_ImageReadySemaphore = s_Device.createSemaphore(vk::SemaphoreCreateInfo());
	s_RenderReadySemaphore = s_Device.createSemaphore(vk::SemaphoreCreateInfo());
	s_WaitFrameFence = s_Device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
}

void RenderingDevice::DestroySwapchain()
{
	s_Device.waitIdle();

	for (auto const& framebuffer : s_Framebuffers)
		s_Device.destroyFramebuffer(framebuffer);

	s_Device.destroyRenderPass(s_RenderPass);

	for (auto const& imageView : s_ImageViews)
		s_Device.destroyImageView(imageView);

	s_Device.destroySwapchainKHR(s_Swapchain);
}

void RenderingDevice::DestroyAll()
{
	s_Device.waitIdle();

	s_Device.destroyFence(s_WaitFrameFence);
	s_Device.destroySemaphore(s_RenderReadySemaphore);
	s_Device.destroySemaphore(s_ImageReadySemaphore);
	s_Device.freeCommandBuffers(s_CommandPool, s_CommandBuffer);
	s_Device.destroyCommandPool(s_CommandPool);

	DestroySwapchain();

	s_Device.destroy();
	s_Instance.destroySurfaceKHR(s_Surface);
	s_Instance.destroy();
}

vk::CommandBuffer RenderingDevice::BeginSingleTimeCommands()
{
	vk::CommandBuffer commandBuffer = AllocateCommandBuffer();

	vk::CommandBufferBeginInfo commandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	commandBuffer.begin(commandBufferBeginInfo);

	return commandBuffer;
}

void RenderingDevice::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo(nullptr, nullptr, commandBuffer, nullptr);
	s_Queue.submit(submitInfo);

	s_Device.waitIdle();
	s_Device.freeCommandBuffers(s_CommandPool, commandBuffer);
}

VulkanBuffer RenderingDevice::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	VulkanBuffer vulkanBuffer = {};

	// Create buffer
	vk::BufferCreateInfo bufferCreateInfo(vk::BufferCreateFlags(), size, usage, vk::SharingMode::eExclusive);
	vulkanBuffer.Buffer = s_Device.createBuffer(bufferCreateInfo);

	// Allocate memory
	vk::MemoryRequirements requirments = s_Device.getBufferMemoryRequirements(vulkanBuffer.Buffer);
	vk::MemoryAllocateInfo allocateInfo(requirments.size, FindMemoryType(requirments.memoryTypeBits, properties));
	vulkanBuffer.Memory = s_Device.allocateMemory(allocateInfo);

	// Bind memory
	s_Device.bindBufferMemory(vulkanBuffer.Buffer, vulkanBuffer.Memory, 0);

	return vulkanBuffer;
}

void RenderingDevice::DestroyBuffer(VulkanBuffer buffer)
{
	s_Device.waitIdle();
	s_Device.destroyBuffer(buffer.Buffer);
	s_Device.freeMemory(buffer.Memory);
	buffer.Buffer = nullptr;
	buffer.Memory = nullptr;
}

VulkanImage RenderingDevice::CreateImage(sf::Uint32 width, sf::Uint32 height, vk::Format format, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	VulkanImage vulkanImage = {};

	// Create image
	vk::ImageCreateInfo imageCreateInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, format, vk::Extent3D(width, height, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive);
	vulkanImage.Image = s_Device.createImage(imageCreateInfo);

	// Allocate memory
	vk::MemoryRequirements requirements = s_Device.getImageMemoryRequirements(vulkanImage.Image);
	vk::MemoryAllocateInfo allocateInfo(requirements.size, FindMemoryType(requirements.memoryTypeBits, properties));
	vulkanImage.Memory = s_Device.allocateMemory(allocateInfo);

	// Bind memory
	s_Device.bindImageMemory(vulkanImage.Image, vulkanImage.Memory, 0);

	return vulkanImage;
}

void RenderingDevice::DestroyImage(VulkanImage image)
{
	s_Device.waitIdle();
	s_Device.destroyImage(image.Image);
	s_Device.freeMemory(image.Memory);
	image.Image = nullptr;
	image.Memory = nullptr;
}

vk::ImageView RenderingDevice::CreateImageView(vk::Image image, vk::Format format)
{
	vk::ImageViewCreateInfo imageViewCreateInfo(vk::ImageViewCreateFlags(),
		image,
		vk::ImageViewType::e2D,
		format,
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

	return s_Device.createImageView(imageViewCreateInfo);
}

vk::Framebuffer RenderingDevice::CreateFramebuffer(vk::ImageView imageView, sf::Uint32 width, sf::Uint32 height)
{
	vk::FramebufferCreateInfo framebufferCreateInfo(vk::FramebufferCreateFlags(),
		s_RenderPass,
		imageView,
		width,
		height,
		1);

	return s_Device.createFramebuffer(framebufferCreateInfo);
}

vk::CommandBuffer RenderingDevice::AllocateCommandBuffer()
{
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo(s_CommandPool, vk::CommandBufferLevel::ePrimary, 1);
	return s_Device.allocateCommandBuffers(commandBufferAllocateInfo).front();
}

sf::Uint32 RenderingDevice::FindMemoryType(sf::Uint32 suitableTypes, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memoryProperties = s_PhysicalDevice.getMemoryProperties();
	for (sf::Uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (suitableTypes & (1 << i) && memoryProperties.memoryTypes[i].propertyFlags & properties)
		{
			return i;
		}
	}

	std::cerr << "Failed to find memory type\n";
	return UINT32_MAX;
}

void RenderingDevice::ChangeImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();
	{
		vk::ImageMemoryBarrier memoryBarrier = {};

		vk::PipelineStageFlags sourceStage = {};
		vk::PipelineStageFlags destinationStage = {};

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			memoryBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
			memoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			memoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			memoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			std::cerr << "Image layout NOT supported (Image layout transition)\n";
			s_Window->close();
			return;
		}

		memoryBarrier.oldLayout = oldLayout;
		memoryBarrier.newLayout = newLayout;
		memoryBarrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		memoryBarrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		memoryBarrier.image = image;
		memoryBarrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), nullptr, nullptr, memoryBarrier);
	}
	EndSingleTimeCommands(commandBuffer);
}
