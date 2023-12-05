#include <SFML/Window.hpp>

#include "RenderingDevice.hpp"

int main()
{
	sf::WindowBase window(sf::VideoMode(960, 540), "SFML-Vulkan");

	RenderingDevice::Initialize(&window);

	VulkanShader shader = RenderingDevice::CreateShader("Resources/vert.spv", "Resources/frag.spv");

	std::vector<sf::Vector3f> vertices = {
		sf::Vector3f(0.0f, -0.5f, 0.0f),
		sf::Vector3f(0.5f,  0.5f, 0.0f),
		sf::Vector3f(-0.5f, 0.5f, 0.0f)
	};

	VulkanBuffer vertexBuffer = RenderingDevice::CreateVertexBuffer(vertices);

	while (window.isOpen())
	{
		sf::Event event = {};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::Resized)
				RenderingDevice::RecreateSwapchain();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
		}

		if (!window.isOpen())
			break;

		RenderingDevice::BeginRenderPass();
		{
			RenderingDevice::SetViewport(sf::Vector2f(0.0f, 0.0f), (sf::Vector2f)window.getSize());
			RenderingDevice::SetScissors(sf::Vector2i(0, 0), (sf::Vector2i)window.getSize());

			RenderingDevice::BindShader(shader);
			RenderingDevice::BindVertexBuffer(vertexBuffer);
			RenderingDevice::Draw(vertices.size());
		}
		RenderingDevice::EndRenderPass();
		RenderingDevice::Present();
	}

	RenderingDevice::DestroyVertexBuffer(vertexBuffer);
	RenderingDevice::DestroyShader(shader);

	RenderingDevice::Terminate();

	return 0;
}
