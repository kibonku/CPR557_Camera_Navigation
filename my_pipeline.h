#ifndef __MY_PIPELINE_H__
#define __MY_PIPELINE_H__

#include <string>
#include <vector>
#include "my_device.h"

struct PipelineConfigInfo
{
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo      viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo; // how configure how different stages of the pipeline work
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo   multisampleInfo;
	VkPipelineColorBlendAttachmentState    colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;

	std::vector<VkDynamicState>            dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo       dynamicStateInfo;

	VkPipelineLayout                       pipelineLayout = nullptr;
	VkRenderPass                           renderPass = nullptr;
	uint32_t                               subpass = 0;
};

class MyPipeline
{
public:
	MyPipeline(MyDevice &device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo &configInfo);
	~MyPipeline();

	MyPipeline(const MyPipeline&) = delete;
	MyPipeline& operator=(const MyPipeline&) = delete;
	MyPipeline(MyPipeline &&) = delete;
	MyPipeline& operator=(const MyPipeline&&) = delete;

    void bind(VkCommandBuffer commandBuffer);
	static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

private:
	static std::vector<char> _readFile(const std::string& filename);

	void _createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
	void _createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	MyDevice&      m_myDevice;
	VkPipeline     m_vkGraphicsPipeline;
	VkShaderModule m_vkVertShaderModule;
	VkShaderModule m_vkFragShaderModule;
};

#endif

