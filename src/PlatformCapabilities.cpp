#include "PlatformCapabilities.h"


PlatformInfo PlatformCapabilities::getPlatformInfo() {
    const auto availableLayers = PlatformCapabilities::getAvailableVulkanInstanceLayers();
    const auto availableExtensions = PlatformCapabilities::getAvailableVulkanInstanceExtensions();
        
    PlatformInfo platformInfo { availableLayers, availableExtensions };

    return platformInfo;
}

PhysicalDeviceProperties PlatformCapabilities::getAvailableVulkanDeviceExtensions(VkPhysicalDevice physicalDevice) {
    auto deviceExtensionProperties =  std::vector<VkExtensionProperties> {};
    uint32_t numInstanceExtensions = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numInstanceExtensions, nullptr);
    if (numInstanceExtensions > 0) {
        deviceExtensionProperties.resize(numInstanceExtensions);
        vkEnumerateDeviceExtensionProperties(
            physicalDevice,
            nullptr, 
            &numInstanceExtensions, 
            deviceExtensionProperties.data()
        );
    }

    return PhysicalDeviceProperties(deviceExtensionProperties);
}

std::vector<VkLayerProperties> PlatformCapabilities::getAvailableVulkanInstanceLayers() {
    auto instanceLayerProperties = std::vector<VkLayerProperties> {};
    uint32_t numInstanceExtensions = 0;
    vkEnumerateInstanceLayerProperties(&numInstanceExtensions, nullptr);
    if (numInstanceExtensions > 0) {
        instanceLayerProperties.resize(numInstanceExtensions);
        vkEnumerateInstanceLayerProperties(
            &numInstanceExtensions, 
            instanceLayerProperties.data()
        );
    }

    return instanceLayerProperties;
}

std::vector<VkExtensionProperties> PlatformCapabilities::getAvailableVulkanInstanceExtensions() {
    auto instanceExtensionProperties = std::vector<VkExtensionProperties> {};
    uint32_t numInstanceExtensions = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, nullptr);
    if (numInstanceExtensions > 0) {
        instanceExtensionProperties.resize(numInstanceExtensions);
        vkEnumerateInstanceExtensionProperties(
            nullptr, 
            &numInstanceExtensions, 
            instanceExtensionProperties.data()
        );
    }

    return instanceExtensionProperties;
}



MissingPlatformRequirements PlatformCapabilities::detectMissingInstanceRequirements(
    const PlatformInfo& platformInfo,
    const PlatformRequirements& platformRequirements
) {
    auto missingExtensionNames = std::vector<std::string> {};
    auto availableExtensions = platformInfo.getAvailableExtensions();
    for (const auto& extensionName : platformRequirements.getExtensions()) {
            auto found = std::find_if(
                std::begin(availableExtensions),
                std::end(availableExtensions),
                [extensionName](const auto& extension) {
                    return strcmp(extensionName.data(), extension.extensionName) == 0;
                }
            );
            auto extensionNotFound = (found == std::end(availableExtensions));
            if (extensionNotFound) {
                missingExtensionNames.emplace_back(extensionName);
            }
    }

    auto missingLayerNames = std::vector<std::string> {};
    auto availableLayers = platformInfo.getAvailableLayers();
    for (const auto& layerName : platformRequirements.getLayers()) {
            auto found = std::find_if(
                std::begin(availableLayers),
                std::end(availableLayers),
                [layerName](const auto& layer) {
                    return strcmp(layerName.data(), layer.layerName) == 0;
                }
            );
            auto layerNotFound = (found == std::end(availableLayers));
            if (layerNotFound) {
                missingLayerNames.emplace_back(layerName);
            }
    }

    return MissingPlatformRequirements(missingExtensionNames, missingLayerNames);
}

MissingPhysicalDeviceRequirements PlatformCapabilities::detectMissingRequiredDeviceExtensions(
    const PhysicalDeviceProperties& physicalDeviceProperties,
    const PhysicalDeviceRequirements& physicalDeviceRequirements
) {
    auto missingExtensionNames = std::vector<std::string> {};
    auto requiredExtensions = physicalDeviceRequirements.getExtensions();
    for (const auto& requiredExtension : requiredExtensions) {
        auto found = std::find_if(
            std::begin(requiredExtensions),
            std::end(requiredExtensions),
            [requiredExtension](const auto& extension) {
            return strcmp(requiredExtension.data(), extension.data()) == 0;
            }
        );
        auto extensionNotFound = (found == std::end(requiredExtensions));
        if (extensionNotFound) {
            missingExtensionNames.emplace_back(requiredExtension);
        }
    }

    return PhysicalDeviceRequirements(missingExtensionNames);
}
