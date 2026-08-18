#include "render_system.h"
#include "shader.h"
#include <vector>

BEGIN_GAIA_GRAPHICS_VK1

std::vector<std::string> EnumerateInstanceExtensions()
{
	std::vector<std::string> extensionNames;
	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
	if (extCount > 0)
	{
		std::vector<VkExtensionProperties> extensions(extCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
		{
			for (VkExtensionProperties& extension : extensions)
			{
				extensionNames.push_back(extension.extensionName);
			}
		}
	}
	return extensionNames;
}

std::vector<std::string> EnumerateDeviceExtensions(VkPhysicalDevice physicalDevice)
{
	std::vector<std::string> extensionNames;
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
	if (extCount > 0)
	{
		std::vector<VkExtensionProperties> extensions(extCount);
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
		{
			for (VkExtensionProperties& extension : extensions)
			{
				extensionNames.push_back(extension.extensionName);
			}
		}
	}
	return extensionNames;
}


const uint32_t invalid_queue_family_index = UINT32_MAX;

auto FindQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool preferPerformance)
{
	struct
	{
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	}
	queueFamilyIndices = 
	{
		invalid_queue_family_index,
		invalid_queue_family_index,
		invalid_queue_family_index
	};

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	if (preferPerformance)
	{
		for (uint32_t i = 0; i < queueFamilies.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
			if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
			{
				queueFamilyIndices.compute = i;
				break;
			}
		}
		for (uint32_t i = 0; i < queueFamilies.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
			if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
			{
				queueFamilyIndices.transfer = i;
				break;
			}
		}
		if (invalid_queue_family_index == queueFamilyIndices.compute)
		{
			for (uint32_t i = 0; i < queueFamilies.size(); i++)
			{
				const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
				if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				{
					queueFamilyIndices.graphics = i;
					queueFamilyIndices.compute = i;
					if (invalid_queue_family_index == queueFamilyIndices.transfer)
					{
						queueFamilyIndices.transfer = i;
					}
					break;
				}
			}
		}
		else
		{
			for (uint32_t i = 0; i < queueFamilies.size(); i++)
			{
				const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
				if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				{
					queueFamilyIndices.graphics = i;
					if (invalid_queue_family_index == queueFamilyIndices.transfer)
					{
						queueFamilyIndices.transfer = i;
					}
					break;
				}
			}
		}
	}
	else
	{
		bool found = false;
		for (uint32_t i = 0; i < queueFamilies.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
			{
				queueFamilyIndices.graphics = i;
				queueFamilyIndices.compute = i;
				queueFamilyIndices.transfer = i;
				found = true;
				break;
			}
		}
		if (!found)
		{
			for (uint32_t i = 0; i < queueFamilies.size(); i++)
			{
				const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
				if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				{
					queueFamilyIndices.graphics = i;
					queueFamilyIndices.transfer = i;
					break;
				}
			}
			for (uint32_t i = 0; i < queueFamilies.size(); i++)
			{
				const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
				if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
				{
					queueFamilyIndices.compute = i;
					break;
				}
			}
		}
	}
	return queueFamilyIndices;
}


RenderSystem* RenderSystem::New()
{
}

RenderSystem::~RenderSystem()
{
	if (m_device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_device, nullptr);
	}
	if (m_instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_instance, nullptr);
	}
}

bool RenderSystem::initialize(const RenderSystemConfig& config)
{
	std::vector<const char*> enabledInstanceExtensions;
	std::vector<const char*> enabledDeviceExtensions;


	std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
#if GAIA_PLATFORM == GAIA_PLATFORM_WIN32
	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif GAIA_PLATFORM == GAIA_PLATFORM_ANDROID
	instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif GAIA_PLATFORM == GAIA_PLATFORM_WAYLAND
	instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

	std::vector<std::string> supportedInstanceExtensions = EnumerateInstanceExtensions();

	for (const char* enabledExtension : enabledInstanceExtensions)
	{
		if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), enabledExtension) != supportedInstanceExtensions.end())
		{
			instanceExtensions.push_back(enabledExtension);
		}
		else
		{
			std::cerr << "Instance extension " << enabledExtension << " is not supported and will be ignored";
		}
	}

	if (config.debug)
	{
		if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != supportedInstanceExtensions.end())
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
	}

	VkApplicationInfo applicationInfo
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = config.appName.c_str(),
		.pEngineName = "Gaia",
		.apiVersion = VK_API_VERSION_1_3
	};

	VkInstanceCreateInfo instanceCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &applicationInfo
	};

	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.pfnUserCallback = debugUtilsMessageCallback
	};

	if (config.debug)
	{
		debugUtilsMessengerCreateInfo.pNext = instanceCreateInfo.pNext;
		instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfo;
	}

	if (!instanceExtensions.empty())
	{
		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	}

	const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
	if (config.debug)
	{
		uint32_t instanceLayerCount;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
		std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
		bool validationLayerPresent = false;
		for (VkLayerProperties& layer : instanceLayerProperties)
		{
			if (strcmp(layer.layerName, validationLayerName) == 0)
			{
				validationLayerPresent = true;
				break;
			}
		}
		if (validationLayerPresent)
		{
			instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
			instanceCreateInfo.enabledLayerCount = 1;
		}
		else
		{
			std::cerr << "Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled";
		}
	}

	VkInstance instance = VK_NULL_HANDLE;
	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		std::cerr << "Failed to create Vulkan instance: " << result;
		return false;
	}
	m_instance = instance;

	uint32_t gpuCount = 0;
	result = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
	if (result != VK_SUCCESS) 
	{
		std::cerr << "Could not enumerate physical devices : " << result;
		return false;
	}

	if (gpuCount == 0) 
	{
		std::cerr << "No device with Vulkan support found";
		return false;
	}
	std::vector<VkPhysicalDevice> allPhysicalDevices(gpuCount);
	result = vkEnumeratePhysicalDevices(instance, &gpuCount, allPhysicalDevices.data());
	if (result != VK_SUCCESS) 
	{
		std::cerr << "Could not enumerate physical devices : " << result;
		return false;
	}
	std::vector<std::string> physicalDeviceNames;
	std::vector<VkPhysicalDevice> physicalDevices;
	for (VkPhysicalDevice& physicalDevice : allPhysicalDevices)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		if(properties.apiVersion >= VK_API_VERSION_1_3)
		{
			physicalDevices.push_back(physicalDevice);
			physicalDeviceNames.push_back(properties.deviceName);
		}
	}

	if (physicalDevices.empty())
	{
		std::cerr << "No physical device supporting Vulkan 1.3 found";
		return false;
	}

	VkPhysicalDevice physicalDevice = physicalDevices[0];
	if (physicalDevices.size() > 1 && config.selectPhysicalDeviceFunc)
	{
		uint32_t selectedDevice = config.selectPhysicalDeviceFunc(physicalDeviceNames);
		if (selectedDevice >= physicalDevices.size())
		{
			std::cerr << "Invalid physical device index selected: " << selectedDevice;
			return false;
		}
		else
		{
			physicalDevice = physicalDevices[selectedDevice];
		}
	}
	m_physicalDevice = physicalDevice;

	VkPhysicalDeviceFeatures2 features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	VkPhysicalDeviceVulkan13Features vulkan13Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features2.pNext = &vulkan13Features;
	vkGetPhysicalDeviceFeatures2(context.gpu, &features2);

	if (!vulkan13Features.dynamicRendering)
	{
		std::cerr << "Dynamic Rendering feature is missing";
		return false;
	}

	if (!vulkan13Features.synchronization2)
	{
		std::cerr << "Synchronization2 feature is missing";
		return false;
	}

	VkPhysicalDeviceVulkan13Features enableVulkan13Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE,
	};

	VkPhysicalDeviceVulkan12Features enableVulkan12Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.pNext = &enableVulkan13Features,
	};

	VkPhysicalDeviceVulkan11Features enableVulkan11Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = &enableVulkan12Features,
	};

	VkPhysicalDeviceFeatures2 enableFeatures2{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &enableVulkan11Features };

	auto queueFamilyIndices = FindQueueFamilyIndices(physicalDevice, false);

	if (invalid_queue_family_index == queueFamilyIndices.graphics ||
		invalid_queue_family_index == queueFamilyIndices.compute ||
		invalid_queue_family_index == queueFamilyIndices.transfer)
	{
		std::cerr << "Failed to find valid queue family indices";
		return false;
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	const float graphicsQueuePriority = 1.0f;
	const float computeQueuePriority = 0.75f;
	const float transferQueuePriority = 0.5f;

	VkDeviceQueueCreateInfo graphicsQueueInfo
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queueFamilyIndices.graphics,
		.queueCount = 1,
		.pQueuePriorities = &graphicsQueuePriority
	};
	queueCreateInfos.push_back(graphicsQueueInfo);

	if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
	{
		VkDeviceQueueCreateInfo computeQueueInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamilyIndices.compute,
			.queueCount = 1,
			.pQueuePriorities = &computeQueuePriority
		};
		queueCreateInfos.push_back(computeQueueInfo);
	}

	if(queueFamilyIndices.transfer != queueFamilyIndices.graphics && queueFamilyIndices.transfer != queueFamilyIndices.compute)
	{
		VkDeviceQueueCreateInfo transferQueueInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamilyIndices.transfer,
			.queueCount = 1,
			.pQueuePriorities = &transferQueuePriority
		};
		queueCreateInfos.push_back(transferQueueInfo);
	}

	std::vector<const char*> deviceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	std::vector<std::string> supportedDeviceExtensions = EnumerateDeviceExtensions();

	for (const char* enabledDeviceExtension : enabledDeviceExtensions)
	{
		if (std::find(supportedDeviceExtensions.begin(), supportedDeviceExtensions.end(), enabledDeviceExtension) != supportedDeviceExtensions.end())
		{
			deviceExtensions.push_back(enabledDeviceExtension);
		}
		else
		{
			std::cerr << "Device extension " << enabledDeviceExtension << " is not supported and will be ignored";
		}
	}


	VkDeviceCreateInfo deviceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &enableFeatures2,
		.queueCreateInfoCount = uint32_t(queueCreateInfos.size()),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.enabledExtensionCount = uint32_t(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data() };

	VkDevice device = VK_NULL_HANDLE;
	result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		std::cerr << "Failed to create logical device: " << result;
		return false;
	}
	m_device = device;

	VmaAllocatorCreateInfo allocatorCreateInfo{};
	allocatorCreateInfo.physicalDevice = physicalDevice;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.instance = instance;

	VmaAllocator allocator = VK_NULL_HANDLE;
	result = vmaCreateAllocator(&allocatorCreateInfo, &allocator);
	if (result != VK_SUCCESS)
	{
		return false;
	}
	m_allocator = allocator;


	uint32_t recordThreadCount = config.recordThreadCount;
	if (recordThreadCount > 0)
	{
		uint32_t hardwareConcurrency = std::thread::hardware_concurrency();
		if (recordThreadCount > hardwareConcurrency)
		{
			recordThreadCount = hardwareConcurrency;
		}
		if (hardwareConcurrency == recordThreadCount && recordThreadCount > 0)
		{
			recordThreadCount -= 1;
		}
	}

	if(recordThreadCount > 0)
	{
		m_recordThreads.resize(recordThreadCount);
		for (RecordThread& recordThread : m_recordThreads)
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.queueFamilyIndex = queueFamilyIndices.graphics,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
			};
			result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &recordThread.commandPool);
			if (result != VK_SUCCESS)
			{
				std::cerr << "Failed to create command pool for record thread: " << result;
				return false;
			}
			VkCommandBufferAllocateInfo commandBufferAllocateInfo
			{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = recordThread.commandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1
			};
			result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &recordThread.commandBuffer);
			if (result != VK_SUCCESS)
			{
				std::cerr << "Failed to allocate command buffer for record thread: " << result;
				return false;
			}
		}
	}
}
	
VertexShader* RenderSystem::createVertexShader(const char* code, size_t codeSize, const char* entryPoint)
{
	return new VertexShader(code, codeSize, entryPoint);
}

RenderWindow* RenderSystem::createRenderWindow(void* platformHandle)
{
	return new RenderWindow(platformHandle);
}

void RenderSystem::render(VertexShaderElement* graphicsElements)
{

}

END_GAIA_GRAPHICS_VK1
