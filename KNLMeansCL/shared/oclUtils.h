/*
*	This file is part of KNLMeansCL,
*	Copyright(C) 2015-2016  Edoardo Brunetti.
*
*	KNLMeansCL is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*	KNLMeansCL is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*   along with KNLMeansCL. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __OCL_UTILS_H__
#define __OCL_UTILS_H__

//////////////////////////////////////////
// Type Definition
typedef struct _ocl_device_packed {
    cl_platform_id platform;
    cl_device_id device;
} ocl_device_packed;

//////////////////////////////////////////
// Common
inline size_t mrounds(const size_t num, const size_t mul) {
    return (size_t) ceil((double) num / (double) mul) * mul;
}

template <typename T>T inline fastmax(const T& left, const T& right) {
    return left > right ? left : right;
}

template <typename T>T inline clamp(const T& value, const T& low, const T& high) {
    return value < low ? low : (value > high ? high : value);
}

//////////////////////////////////////////
// oclErrorToString
#define STR(code) case code: return #code
const char* oclErrorToString(cl_int err) {
    switch (err) {
        STR(CL_DEVICE_NOT_FOUND);
        STR(CL_DEVICE_NOT_AVAILABLE);
        STR(CL_COMPILER_NOT_AVAILABLE);
        STR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        STR(CL_OUT_OF_RESOURCES);
        STR(CL_OUT_OF_HOST_MEMORY);
        STR(CL_PROFILING_INFO_NOT_AVAILABLE);
        STR(CL_MEM_COPY_OVERLAP);
        STR(CL_IMAGE_FORMAT_MISMATCH);
        STR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        STR(CL_BUILD_PROGRAM_FAILURE);
        STR(CL_MAP_FAILURE);
        STR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
        STR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
        STR(CL_COMPILE_PROGRAM_FAILURE);
        STR(CL_LINKER_NOT_AVAILABLE);
        STR(CL_LINK_PROGRAM_FAILURE);
        STR(CL_DEVICE_PARTITION_FAILED);
        STR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
        STR(CL_INVALID_VALUE);
        STR(CL_INVALID_DEVICE_TYPE);
        STR(CL_INVALID_PLATFORM);
        STR(CL_INVALID_DEVICE);
        STR(CL_INVALID_CONTEXT);
        STR(CL_INVALID_QUEUE_PROPERTIES);
        STR(CL_INVALID_COMMAND_QUEUE);
        STR(CL_INVALID_HOST_PTR);
        STR(CL_INVALID_MEM_OBJECT);
        STR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        STR(CL_INVALID_IMAGE_SIZE);
        STR(CL_INVALID_SAMPLER);
        STR(CL_INVALID_BINARY);
        STR(CL_INVALID_BUILD_OPTIONS);
        STR(CL_INVALID_PROGRAM);
        STR(CL_INVALID_PROGRAM_EXECUTABLE);
        STR(CL_INVALID_KERNEL_NAME);
        STR(CL_INVALID_KERNEL_DEFINITION);
        STR(CL_INVALID_KERNEL);
        STR(CL_INVALID_ARG_INDEX);
        STR(CL_INVALID_ARG_VALUE);
        STR(CL_INVALID_ARG_SIZE);
        STR(CL_INVALID_KERNEL_ARGS);
        STR(CL_INVALID_WORK_DIMENSION);
        STR(CL_INVALID_WORK_GROUP_SIZE);
        STR(CL_INVALID_WORK_ITEM_SIZE);
        STR(CL_INVALID_GLOBAL_OFFSET);
        STR(CL_INVALID_EVENT_WAIT_LIST);
        STR(CL_INVALID_EVENT);
        STR(CL_INVALID_OPERATION);
        STR(CL_INVALID_GL_OBJECT);
        STR(CL_INVALID_BUFFER_SIZE);
        STR(CL_INVALID_MIP_LEVEL);
        STR(CL_INVALID_GLOBAL_WORK_SIZE);
        STR(CL_INVALID_PROPERTY);
        STR(CL_INVALID_IMAGE_DESCRIPTOR);
        STR(CL_INVALID_COMPILER_OPTIONS);
        STR(CL_INVALID_LINKER_OPTIONS);
        STR(CL_INVALID_DEVICE_PARTITION_COUNT);
        default: return "CL_ERROR";
    }
}

//////////////////////////////////////////
// oclPlatformToInt
inline cl_int oclPlatformToInt(const char* platform_version) {
    cl_int major = platform_version[7] - '0';
    cl_int minor = platform_version[9] - '0';
    return major * 10 + minor;
}

//////////////////////////////////////////
// oclVersionToInt
inline cl_int oclVersionToInt(const char* platform_version) {
    cl_int major = platform_version[0] - '0';
    cl_int minor = platform_version[2] - '0';
    return major * 10 + minor;
}

//////////////////////////////////////////
// oclGetDevicesList
cl_int oclGetDevicesList(cl_device_type device_type, ocl_device_packed* devices, cl_uint* num_devices, const char* version) {
    if (devices == NULL && num_devices != NULL) {
        char str[2048];
        cl_uint avl_platforms = 0, rtn_devices = 0;
        cl_int ret = clGetPlatformIDs(0, NULL, &avl_platforms);
        if (ret != CL_SUCCESS) return ret;
        if (avl_platforms == 0) {
            *num_devices = 0;
            return CL_SUCCESS;
        }
        cl_platform_id *lst_platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * avl_platforms);
        if (lst_platforms == NULL) return CL_INVALID_VALUE;
        ret = clGetPlatformIDs(avl_platforms, lst_platforms, NULL);
        if (ret != CL_SUCCESS) return ret;
        for (cl_uint p = 0; p < avl_platforms; p++) {
            ret = clGetPlatformInfo(lst_platforms[p], CL_PLATFORM_VERSION, sizeof(char) * 2048, str, NULL);
            if (ret != CL_SUCCESS) return ret;
            if (oclPlatformToInt(str) >= oclVersionToInt(version)) {
                cl_uint avl_devices = 0;
                ret = clGetDeviceIDs(lst_platforms[p], device_type, 0, 0, &avl_devices);
                if (ret != CL_SUCCESS && ret != CL_DEVICE_NOT_FOUND) return ret;
                if (avl_devices > 0) {
                    cl_device_id *lst_devices = (cl_device_id*) malloc(sizeof(cl_device_id) * avl_devices);
                    if (lst_devices == NULL) return CL_INVALID_VALUE;
                    ret = clGetDeviceIDs(lst_platforms[p], device_type, avl_devices, lst_devices, NULL);
                    for (cl_uint d = 0; d < avl_devices; d++) {
                        cl_bool image_support;
                        ret = clGetDeviceInfo(lst_devices[d], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &image_support, NULL);
                        if (ret != CL_SUCCESS) return ret;
                        if (image_support) rtn_devices++;
                    }
                    free(lst_devices);
                }
            }
        }
        free(lst_platforms);
        *num_devices = rtn_devices;
        return CL_SUCCESS;
    } else if (devices != NULL && num_devices == NULL) {
        char str[2048];
        cl_uint avl_platforms = 0, index = 0;
        cl_int ret = clGetPlatformIDs(0, NULL, &avl_platforms);
        if (ret != CL_SUCCESS) return ret;
        if (avl_platforms == 0) return CL_SUCCESS;
        cl_platform_id *lst_platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * avl_platforms);
        if (lst_platforms == NULL) return CL_INVALID_VALUE;
        ret = clGetPlatformIDs(avl_platforms, lst_platforms, NULL);
        if (ret != CL_SUCCESS) return ret;
        for (cl_uint p = 0; p < avl_platforms; p++) {
            ret = clGetPlatformInfo(lst_platforms[p], CL_PLATFORM_VERSION, sizeof(char) * 2048, str, NULL);
            if (ret != CL_SUCCESS) return ret;
            if (oclPlatformToInt(str) >= oclVersionToInt(version)) {
                cl_uint avl_devices = 0;
                ret = clGetDeviceIDs(lst_platforms[p], device_type, 0, 0, &avl_devices);
                if (ret != CL_SUCCESS && ret != CL_DEVICE_NOT_FOUND) return ret;
                if (avl_devices > 0) {
                    cl_device_id *lst_devices = (cl_device_id*) malloc(sizeof(cl_device_id) * avl_devices);
                    if (lst_devices == NULL) return CL_INVALID_VALUE;
                    ret = clGetDeviceIDs(lst_platforms[p], device_type, avl_devices, lst_devices, NULL);
                    for (cl_uint d = 0; d < avl_devices; d++) {
                        cl_bool image_support;
                        ret = clGetDeviceInfo(lst_devices[d], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &image_support, NULL);
                        if (ret != CL_SUCCESS) return ret;
                        if (image_support) devices[index++] = { lst_platforms[p], lst_devices[d] };
                    }
                    free(lst_devices);
                }
            }
        }
        free(lst_platforms);
        return CL_SUCCESS;
    } else {
        return CL_INVALID_VALUE;
    }
}

#endif //__OCL_UTILS_H__
