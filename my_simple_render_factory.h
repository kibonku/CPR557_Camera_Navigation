#ifndef __MY_SIMPLE_RENDER_FACTORY_H__
#define __MY_SIMPLE_RENDER_FACTORY_H__

#include "my_device.h"
#include "my_game_object.h"
#include "my_pipeline.h"
#include "my_camera.h"

// std
#include <memory>
#include <vector>

class MySimpleRenderFactory
{
public:
	MySimpleRenderFactory(MyDevice& device, VkRenderPass renderPass);
	~MySimpleRenderFactory();

	MySimpleRenderFactory(const MySimpleRenderFactory&) = delete;
	MySimpleRenderFactory& operator=(const MySimpleRenderFactory&) = delete;
	MySimpleRenderFactory(MySimpleRenderFactory&&) = delete;
	MySimpleRenderFactory& operator=(const MySimpleRenderFactory&&) = delete;

	void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<MyGameObject>& gameObjects, const MyCamera &camera);

private:
	void _createPipelineLayout();
	void _createPipeline(VkRenderPass renderPass);

	MyDevice&                   m_myDevice;

	std::unique_ptr<MyPipeline> m_pMyPipeline;
	VkPipelineLayout            m_vkPipelineLayout;
};

#endif

