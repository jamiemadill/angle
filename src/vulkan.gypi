# Copyright 2016 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables':
    {
        'glslang_path': '../third_party/glslang',
        'vulkan_layers_path': '../third_party/vulkan-validation-layers',
        'vulkan_loader_sources':
        [
            '<(vulkan_layers_path)/loader/cJSON.c',
            '<(vulkan_layers_path)/loader/cJSON.h',
            '<(vulkan_layers_path)/loader/debug_report.c',
            '<(vulkan_layers_path)/loader/debug_report.h',
            '<(vulkan_layers_path)/loader/dev_ext_trampoline.c',
            '<(vulkan_layers_path)/loader/gpa_helper.h',
            '<(vulkan_layers_path)/loader/loader.c',
            '<(vulkan_layers_path)/loader/loader.h',
            '<(vulkan_layers_path)/loader/murmurhash.c',
            '<(vulkan_layers_path)/loader/murmurhash.h',
            '<(vulkan_layers_path)/loader/table_ops.h',
            '<(vulkan_layers_path)/loader/trampoline.c',
            '<(vulkan_layers_path)/loader/vk_loader_platform.h',
            '<(vulkan_layers_path)/loader/wsi.c',
            '<(vulkan_layers_path)/loader/wsi.h',
        ],
        'vulkan_loader_win_sources':
        [
            '<(vulkan_layers_path)/loader/dirent_on_windows.c',
            '<(vulkan_layers_path)/loader/dirent_on_windows.h',
        ],
    },
    'conditions':
    [
        ['angle_enable_vulkan==1',
        {
            'targets':
            [
                {
                    'target_name': 'vulkan_loader',
                    'type': 'static_library',
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/angle_loader.h',
                        '<@(vulkan_loader_sources)',
                    ],
                    'defines':
                    [
                        'DEFAULT_VK_LAYERS_PATH_OVERRIDE=\\\"<(PRODUCT_DIR)\\\"',
                    ],
                    'include_dirs':
                    [
                        '<(vulkan_layers_path)/include',
                        '<(vulkan_layers_path)/loader',
                    ],
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'AdditionalOptions':
                            [
                                '/wd4054', # Type cast from function pointer
                                '/wd4055', # Type cast from data pointer
                                '/wd4100', # Unreferenced formal parameter
                                '/wd4152', # Nonstandard extension used (pointer conversion)
                                '/wd4305', # Type cast truncation
                                '/wd4706', # Assignment within conditional expression
                                '/wd4996', # Unsafe stdlib function

                                # force include the ANGLE loader header
                                '/FI<(angle_gen_path)/vulkan/angle_loader.h',
                            ],
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalDependencies':
                            [
                                'shlwapi.lib',
                            ],
                        },
                    },
                    'direct_dependent_settings':
                    {
                        'include_dirs':
                        [
                            '<(vulkan_layers_path)/include',
                            '<(vulkan_layers_path)/loader',
                        ],
                        'msvs_settings':
                        {
                            'VCLinkerTool':
                            {
                                'AdditionalDependencies':
                                [
                                    'shlwapi.lib',
                                ],
                            },
                        },
                        'conditions':
                        [
                            ['OS=="win"',
                            {
                                'defines':
                                [
                                    'VK_USE_PLATFORM_WIN32_KHR',
                                ],
                            }],
                        ],
                    },
                    'conditions':
                    [
                        ['OS=="win"',
                        {
                            'sources':
                            [
                                '<@(vulkan_loader_win_sources)',
                            ],
                            'defines':
                            [
                                'VK_USE_PLATFORM_WIN32_KHR',
                            ],
                        }],
                    ],
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_loader_gen_angle_header',
                            'message': 'generating Vulkan loader ANGLE header',
                            'msvs_cygwin_shell': 0,
                            'inputs':
                            [
                                '<(angle_path)/scripts/generate_vulkan_header.py',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/angle_loader.h',
                            ],
                            'action':
                            [
                                # TODO(jmadill): Use correct platform path
                                'python <(angle_path)/scripts/generate_vulkan_header.py <(PRODUCT_DIR) <(vulkan_layers_path)/layers/windows;../<(vulkan_layers_path)/layers/windows <(angle_gen_path)/vulkan/angle_loader.h',
                            ],
                        },
                    ],
                },
                {
                    'target_name': 'vulkan_layer_utils_static',
                    'type': 'static_library',
                    'sources':
                    [
                        '<(vulkan_layers_path)/layers/vk_layer_config.cpp',
                        '<(vulkan_layers_path)/layers/vk_layer_config.h',
                        '<(vulkan_layers_path)/layers/vk_layer_extension_utils.cpp',
                        '<(vulkan_layers_path)/layers/vk_layer_extension_utils.h',
                        '<(vulkan_layers_path)/layers/vk_layer_utils.cpp',
                        '<(vulkan_layers_path)/layers/vk_layer_utils.h',
                    ],
                    'include_dirs':
                    [
                        '<(vulkan_layers_path)/include',
                        '<(vulkan_layers_path)/loader',
                    ],
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'PreprocessorDefinitions':
                            [
                                '_HAS_EXCEPTIONS=0',
                            ],
                            'AdditionalOptions':
                            [
                                '/wd4100', # Unreferenced formal parameter
                                '/wd4309', # Truncation of constant value
                                '/wd4505', # Unreferenced local function has been removed
                                '/wd4996', # Unsafe stdlib function
                            ],
                        },
                    },
                    'conditions':
                    [
                        ['OS=="win"',
                        {
                            'defines':
                            [
                                'WIN32',
                            ],
                        }],
                    ],
                    'direct_dependent_settings':
                    {
                        'msvs_cygwin_shell': 0,
                        'sources':
                        [
                            '<(vulkan_layers_path)/layers/vk_layer_table.cpp',
                            '<(vulkan_layers_path)/layers/vk_layer_table.h',
                        ],
                        'include_dirs':
                        [
                            '<(angle_gen_path)/vulkan',
                            '<(glslang_path)',
                            '<(vulkan_layers_path)/include',
                            '<(vulkan_layers_path)/loader',
                            '<(vulkan_layers_path)/layers',
                        ],
                        'msvs_settings':
                        {
                            'VCCLCompilerTool':
                            {
                                'PreprocessorDefinitions':
                                [
                                    '_HAS_EXCEPTIONS=0',
                                ],
                                'AdditionalOptions':
                                [
                                    '/wd4100', # Unreferenced local parameter
                                    '/wd4456', # declaration hides previous local declaration
                                    '/wd4505', # Unreferenced local function has been removed
                                    '/wd4996', # Unsafe stdlib function
                                ],
                            }
                        },
                        'conditions':
                        [
                            ['OS=="win"',
                            {
                                'defines':
                                [
                                    'VK_USE_PLATFORM_WIN32_KHR',
                                ],
                                'configurations':
                                {
                                    'Debug_Base':
                                    {
                                        'msvs_settings':
                                        {
                                            'VCCLCompilerTool':
                                            {
                                                'AdditionalOptions':
                                                [
                                                    '/bigobj',
                                                ],
                                            },
                                        },
                                    },
                                },
                            }],
                        ],
                    },
                },
                {
                    'target_name': 'vulkan_generate_layer_helpers',
                    'type': 'none',
                    'msvs_cygwin_shell': 0,
                    'dependencies':
                    [
                        'vulkan_generate_layer_def_files',
                    ],
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_generate_dispatch_table_helper',
                            'message': 'generating Vulkan dispatch table helper',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/vk-generate.py',
                                '<(vulkan_layers_path)/vulkan.py'
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/vk_dispatch_table_helper.h'
                            ],
                            'action':
                            [
                                # Hack this in one line so we can redirect stdout
                                # TODO(jmadill): Select correct display server for Linux/Android/etc.
                                'python <(vulkan_layers_path)/vk-generate.py Win32 dispatch-table-ops layer > <(angle_gen_path)/vulkan/vk_dispatch_table_helper.h',
                            ],
                        },
                        {
                            'action_name': 'vulkan_generate_enum_string_helper',
                            'message': 'generating Vulkan enum string helper',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/vk_helper.py',
                                '<(vulkan_layers_path)/include/vulkan/vulkan.h'
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/vk_enum_string_helper.h',
                            ],
                            'action':
                            [
                                'python',
                                '<(vulkan_layers_path)/vk_helper.py',
                                '--gen_enum_string_helper',
                                '<(vulkan_layers_path)/include/vulkan/vulkan.h',
                                '--abs_out_dir',
                                '<(angle_gen_path)/vulkan',
                            ],
                        },

                        {
                            'action_name': 'vulkan_generate_struct_wrappers',
                            'message': 'generating Vulkan struct wrappers',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/vk_helper.py',
                                '<(vulkan_layers_path)/include/vulkan/vulkan.h'
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/vk_safe_struct.cpp',
                                '<(angle_gen_path)/vulkan/vk_safe_struct.h',
                                '<(angle_gen_path)/vulkan/vk_struct_size_helper.c',
                                '<(angle_gen_path)/vulkan/vk_struct_size_helper.h',
                                '<(angle_gen_path)/vulkan/vk_struct_string_helper.h',
                                '<(angle_gen_path)/vulkan/vk_struct_string_helper_cpp.h',
                                '<(angle_gen_path)/vulkan/vk_struct_string_helper_no_addr.h',
                                '<(angle_gen_path)/vulkan/vk_struct_string_helper_no_addr_cpp.h',
                                '<(angle_gen_path)/vulkan/vk_struct_validate_helper.h',
                                '<(angle_gen_path)/vulkan/vk_struct_wrappers.cpp',
                                '<(angle_gen_path)/vulkan/vk_struct_wrappers.h',
                            ],
                            'action':
                            [
                                'python',
                                '<(vulkan_layers_path)/vk_helper.py',
                                '--gen_struct_wrappers',
                                '<(vulkan_layers_path)/include/vulkan/vulkan.h',
                                '--abs_out_dir',
                                '<(angle_gen_path)/vulkan',
                            ],
                        },
                    ],
                },

                {
                    'target_name': 'vulkan_generate_layer_def_files',
                    'type': 'none',
                    'msvs_cygwin_shell': 0,

                    # Kind of a hack, but necessary to get the correct arguments to the def generator rule.
                    'sources':
                    [
                        '<(vulkan_layers_path)/layers/core_validation.h',
                        '<(vulkan_layers_path)/layers/device_limits.h',
                        '<(vulkan_layers_path)/layers/image.h',
                        '<(vulkan_layers_path)/layers/object_tracker.h',
                        '<(vulkan_layers_path)/layers/swapchain.h',
                        '<(vulkan_layers_path)/layers/threading.h',
                        '<(vulkan_layers_path)/layers/unique_objects.h',
                    ],

                    'rules':
                    [
                        {
                            'rule_name': 'vulkan_generate_def_files',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/vk-generate.py',
                                '<(vulkan_layers_path)/vulkan.py',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/VkLayer_<(RULE_INPUT_ROOT).def',
                            ],
                            'extension': 'h',
                            'message': 'generating Vulkan <(RULE_INPUT_ROOT) layer def file',
                            'action':
                            [
                                # Hack this in one line so we can redirect stdout
                                # TODO(jmadill): Select correct display server for Linux/Android/etc.
                                'python <(vulkan_layers_path)/vk-generate.py Win32 win-def-file VkLayer_<(RULE_INPUT_ROOT) layer > <(angle_gen_path)/vulkan/VkLayer_<(RULE_INPUT_ROOT).def',
                            ],
                        },
                    ],

                    # parameter_validation doesn't have a correctly named header file. GYP can be a pain.
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_generate_parameter_validation_def_file',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/vk-generate.py',
                                '<(vulkan_layers_path)/vulkan.py',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/VkLayer_parameter_validation.def',
                            ],
                            'message': 'generating Vulkan parameter_validation layer def file',
                            'action':
                            [
                                # Hack this in one line so we can redirect stdout
                                # TODO(jmadill): Select correct display server for Linux/Android/etc.
                                'python <(vulkan_layers_path)/vk-generate.py Win32 win-def-file VkLayer_parameter_validation layer > <(angle_gen_path)/vulkan/VkLayer_parameter_validation.def',
                            ],
                        },
                    ],
                },

                {
                    'target_name': 'VkLayer_core_validation',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/VkLayer_core_validation.def',
                        '<(angle_gen_path)/vulkan/vk_safe_struct.cpp',
                        '<(angle_gen_path)/vulkan/vk_safe_struct.h',
                        '<(vulkan_layers_path)/layers/core_validation.cpp',
                        '<(vulkan_layers_path)/layers/core_validation.h',
                    ],
                },

                {
                    'target_name': 'VkLayer_device_limits',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/VkLayer_device_limits.def',
                        '<(vulkan_layers_path)/layers/device_limits.cpp',
                        '<(vulkan_layers_path)/layers/device_limits.h',
                    ],
                },

                {
                    'target_name': 'VkLayer_image',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/VkLayer_image.def',
                        '<(vulkan_layers_path)/layers/image.cpp',
                        '<(vulkan_layers_path)/layers/image.h',
                    ],
                },

                {
                    'target_name': 'VkLayer_swapchain',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/VkLayer_swapchain.def',
                        '<(vulkan_layers_path)/layers/swapchain.cpp',
                        '<(vulkan_layers_path)/layers/swapchain.h',
                    ],
                },

                {
                    'target_name': 'VkLayer_object_tracker',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/object_tracker.cpp',
                        '<(angle_gen_path)/vulkan/VkLayer_object_tracker.def',
                        '<(vulkan_layers_path)/layers/object_tracker.h',
                    ],
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_layer_object_tracker_generate',
                            'message': 'generating vulkan object tracker layer',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/include/vulkan/vulkan.h',
                                '<(vulkan_layers_path)/vk-layer-generate.py',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/object_tracker.cpp',
                            ],
                            'action':
                            [
                                # Hack this in one line so we can redirect stdout
                                # TODO(jmadill): Select correct display server for Linux/Android/etc.
                                'python <(vulkan_layers_path)/vk-layer-generate.py Win32 object_tracker <(vulkan_layers_path)/include/vulkan/vulkan.h > <(angle_gen_path)/vulkan/object_tracker.cpp',
                            ],
                        },
                    ],
                },

                {
                    'target_name': 'VkLayer_unique_objects',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/unique_objects.cpp',
                        '<(angle_gen_path)/vulkan/VkLayer_unique_objects.def',
                        '<(angle_gen_path)/vulkan/vk_safe_struct.cpp',
                        '<(angle_gen_path)/vulkan/vk_safe_struct.h',
                        '<(vulkan_layers_path)/layers/unique_objects.h',
                    ],
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_layer_unique_objects_generate',
                            'message': 'generating Vulkan unique_objects layer',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/include/vulkan/vulkan.h',
                                '<(vulkan_layers_path)/vk-layer-generate.py',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/unique_objects.cpp',
                            ],
                            'action':
                            [
                                # Hack this in one line so we can redirect stdout
                                # TODO(jmadill): Select correct display server for Linux/Android/etc.
                                'python <(vulkan_layers_path)/vk-layer-generate.py Win32 unique_objects <(vulkan_layers_path)/include/vulkan/vulkan.h > <(angle_gen_path)/vulkan/unique_objects.cpp',
                            ],
                        },
                    ],
                },

                {
                    'target_name': 'VkLayer_threading',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
						'<(angle_gen_path)/vulkan/thread_check.h',
                        '<(angle_gen_path)/vulkan/VkLayer_threading.def',
                        '<(vulkan_layers_path)/layers/threading.cpp',
                        '<(vulkan_layers_path)/layers/threading.h',
                    ],
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_layer_threading_generate',
                            'message': 'generating Vulkan threading header',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/generator.py',
                                '<(vulkan_layers_path)/genvk.py',
                                '<(vulkan_layers_path)/reg.py',
                                '<(vulkan_layers_path)/vk.xml',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/thread_check.h',
                            ],
                            'action':
                            [
                                'python', '<(vulkan_layers_path)/genvk.py', '-outdir', '<(angle_gen_path)/vulkan', '-registry', '<(vulkan_layers_path)/vk.xml', 'thread_check.h',
                            ],
                        },
                    ],
                },

                {
                    'target_name': 'VkLayer_parameter_validation',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_layer_helpers',
                        'vulkan_layer_utils_static',
                    ],
                    'sources':
                    [
                        '<(angle_gen_path)/vulkan/parameter_validation.h',
                        '<(angle_gen_path)/vulkan/VkLayer_parameter_validation.def',
                        '<(vulkan_layers_path)/layers/parameter_validation.cpp',
                    ],
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_layer_parameter_validation_generate',
                            'message': 'generating Vulkan parameter_validation header',
                            'inputs':
                            [
                                '<(vulkan_layers_path)/generator.py',
                                '<(vulkan_layers_path)/genvk.py',
                                '<(vulkan_layers_path)/reg.py',
                                '<(vulkan_layers_path)/vk.xml',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/parameter_validation.h',
                            ],
                            'action':
                            [
                                'python', '<(vulkan_layers_path)/genvk.py', '-outdir', '<(angle_gen_path)/vulkan', '-registry', '<(vulkan_layers_path)/vk.xml', 'parameter_validation.h',
                            ],
                        },
                    ],
                },
            ],
        }],
    ],
}
