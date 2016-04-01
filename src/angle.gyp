# Copyright (c) 2012 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables':
    {
        'angle_code': 1,
        'angle_post_build_script%': 0,
        'angle_gen_path': '<(SHARED_INTERMEDIATE_DIR)/angle',
        'angle_id_script_base': 'commit_id.py',
        'angle_id_script': '<(angle_gen_path)/<(angle_id_script_base)',
        'angle_id_header_base': 'commit.h',
        'angle_id_header': '<(angle_gen_path)/id/<(angle_id_header_base)',
        'angle_use_commit_id%': '<!(python <(angle_id_script_base) check ..)',
        'angle_enable_d3d9%': 0,
        'angle_enable_d3d11%': 0,
        'angle_enable_gl%': 0,
        'angle_enable_vulkan%': 0,
        'angle_enable_essl%': 1, # Enable this for all configs by default
        'angle_enable_glsl%': 1, # Enable this for all configs by default
        'angle_enable_hlsl%': 0,
        'angle_link_glx%': 0,
        'angle_gl_library_type%': 'shared_library',
        'conditions':
        [
            ['OS=="win"',
            {
                'angle_enable_gl%': 1,
                'angle_enable_d3d9%': 1,
                'angle_enable_d3d11%': 1,
                'angle_enable_hlsl%': 1,
                'angle_enable_vulkan%': 1,
            }],
            ['OS=="linux"',
            {
                'angle_enable_gl%': 1,
            }],
            ['OS=="mac"',
            {
                'angle_enable_gl%': 1,
            }],
        ],
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
    'includes':
    [
        'compiler.gypi',
        'libGLESv2.gypi',
        'libEGL.gypi'
    ],

    'targets':
    [
        {
            'target_name': 'angle_common',
            'type': 'static_library',
            'includes': [ '../build/common_defines.gypi', ],
            'sources':
            [
                '<@(libangle_common_sources)',
            ],
            'include_dirs':
            [
                '.',
                '../include',
            ],
            'direct_dependent_settings':
            {
                'include_dirs':
                [
                    '<(angle_path)/src',
                    '<(angle_path)/include',
                ],
                'conditions':
                [
                    ['OS=="win"',
                    {
                        'configurations':
                        {
                            'Debug_Base':
                            {
                                'defines':
                                [
                                    'ANGLE_ENABLE_DEBUG_ANNOTATIONS'
                                ],
                            },
                        },
                    }],
                ],
            },
            'conditions':
            [
                ['OS=="win"',
                {
                    'configurations':
                    {
                        'Debug_Base':
                        {
                            'defines':
                            [
                                'ANGLE_ENABLE_DEBUG_ANNOTATIONS'
                            ],
                        },
                    },
                }],
            ],
        },

        {
            'target_name': 'copy_scripts',
            'type': 'none',
            'includes': [ '../build/common_defines.gypi', ],
            'hard_dependency': 1,
            'copies':
            [
                {
                    'destination': '<(angle_gen_path)',
                    'files': [ 'copy_compiler_dll.bat', '<(angle_id_script_base)' ],
                },
            ],
            'conditions':
            [
                ['angle_build_winrt==1',
                {
                    'type' : 'shared_library',
                }],
            ],
        },
    ],
    'conditions':
    [
        ['angle_use_commit_id!=0',
        {
            'targets':
            [
                {
                    'target_name': 'commit_id',
                    'type': 'none',
                    'includes': [ '../build/common_defines.gypi', ],
                    'dependencies': [ 'copy_scripts', ],
                    'hard_dependency': 1,
                    'actions':
                    [
                        {
                            'action_name': 'Generate ANGLE Commit ID Header',
                            'message': 'Generating ANGLE Commit ID',
                            # reference the git index as an input, so we rebuild on changes to the index
                            'inputs': [ '<(angle_id_script)', '<(angle_path)/.git/index' ],
                            'outputs': [ '<(angle_id_header)' ],
                            'msvs_cygwin_shell': 0,
                            'action':
                            [
                                'python', '<(angle_id_script)', 'gen', '<(angle_path)', '<(angle_id_header)'
                            ],
                        },
                    ],
                    'all_dependent_settings':
                    {
                        'include_dirs':
                        [
                            '<(angle_gen_path)',
                        ],
                    },
                    'conditions':
                    [
                        ['angle_build_winrt==1',
                        {
                            'type' : 'shared_library',
                        }],
                    ],
                }
            ]
        },
        { # angle_use_commit_id==0
            'targets':
            [
                {
                    'target_name': 'commit_id',
                    'type': 'none',
                    'hard_dependency': 1,
                    'includes': [ '../build/common_defines.gypi', ],
                    'copies':
                    [
                        {
                            'destination': '<(angle_gen_path)/id',
                            'files': [ '<(angle_id_header_base)' ]
                        }
                    ],
                    'all_dependent_settings':
                    {
                        'include_dirs':
                        [
                            '<(angle_gen_path)',
                        ],
                    },
                    'conditions':
                    [
                        ['angle_build_winrt==1',
                        {
                            'type' : 'shared_library',
                        }],
                    ],
                }
            ]
        }],
        ['angle_enable_vulkan==1',
        {
            'targets':
            [
                {
                    'target_name': 'vulkan_loader',
                    'type': 'static_library',
                    'sources':
                    [
                        '<@(vulkan_loader_sources)',
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
                                '/wd4127', # Conditional expression is constant
                                '/wd4152', # Nonstandard extension used (pointer conversion)
                                '/wd4706', # Assignment within conditional expression
                                '/wd4996', # Unsafe stdlib function
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
                },
                {
                    'target_name': 'vulkan_layer_utils',
                    'type': '<(component)',
                    'sources':
                    [
                        '<(vulkan_layers_path)/layers/vk_layer_config.cpp',
                        '<(vulkan_layers_path)/layers/vk_layer_extension_utils.cpp',
                        '<(vulkan_layers_path)/layers/vk_layer_utils.cpp',
                    ],
                    'include_dirs':
                    [
                        '<(vulkan_layers_path)/include',
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
                                '/wd4309', # Truncation of constant value
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
                },
                {
                    'target_name': 'vulkan_generate_helpers',
                    'type': 'none',
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_generate_dispatch_table_helper',
                            'message': 'Generating Vulkan dispatch table helper...',
                            'msvs_cygwin_shell': 0,
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
                            'message': 'Generating Vulkan enum string helper...',
                            'msvs_cygwin_shell': 0,
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
                            'message': 'Generating Vulkan struct wrappers...',
                            'msvs_cygwin_shell': 0,
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
                    'target_name': 'vulkan_layer_draw_state',
                    'type': 'shared_library',
                    'dependencies':
                    [
                        'vulkan_generate_helpers',
                        'vulkan_layer_utils',
                    ],
                    'sources':
                    [
                        '<(vulkan_layers_path)/layers/draw_state.cpp',
                        '<(vulkan_layers_path)/layers/draw_state.h',
                        '<(vulkan_layers_path)/layers/vk_layer_table.cpp',
                        '<(vulkan_layers_path)/layers/vk_layer_table.h',
                    ],
                    'include_dirs':
                    [
                        '<(angle_gen_path)/vulkan',
                        '<(glslang_path)/spirv',
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
                                '/wd4018', # Signed/unsigned mismatch
                                '/wd4100', # Unreferenced formal parameter
                                '/wd4127', # Conditional expression is constant
                                '/wd4245', # Conversion on return, signed/unsigned mismatch
                                '/wd4389', # Signed/unsigned mismatch
                                '/wd4456', # Declaration hides previous local declaration
                                '/wd4505', # Unreferenced local function has been removed
                                '/wd4800', # Forcing bool to true or false
                                '/wd4805', # Unsafe mix of types with bool
                                '/wd4996', # Unsafe stdlib function
                                '/bigobj', # draw_state requires this flag
                            ],
                        },
                    },
                    'actions':
                    [
                        {
                            'action_name': 'vulkan_generate_draw_state_def',
                            'message': 'Generating draw state layer def file...',
                            'msvs_cygwin_shell': 0,
                            'inputs':
                            [
                                '<(vulkan_layers_path)/vk-generate.py',
                                '<(vulkan_layers_path)/vulkan.py',
                            ],
                            'outputs':
                            [
                                '<(angle_gen_path)/vulkan/VkLayer_draw_state.def',
                            ],
                            'action':
                            [
                                # Hack this in one line so we can redirect stdout
                                # TODO(jmadill): Select correct display server for Linux/Android/etc.
                                'python <(vulkan_layers_path)/vk-generate.py Win32 win-def-file VkLayer_draw_state layer > <(angle_gen_path)/vulkan/VkLayer_draw_state.def',
                            ],
                        },
                    ],
                },
            ],
        }],
        ['OS=="win"',
        {
            'targets':
            [
                {
                    'target_name': 'copy_compiler_dll',
                    'type': 'none',
                    'dependencies': [ 'copy_scripts', ],
                    'includes': [ '../build/common_defines.gypi', ],
                    'conditions':
                    [
                        ['angle_build_winrt==0',
                        {
                            'actions':
                            [
                                {
                                    'action_name': 'copy_dll',
                                    'message': 'Copying D3D Compiler DLL...',
                                    'msvs_cygwin_shell': 0,
                                    'inputs': [ 'copy_compiler_dll.bat' ],
                                    'outputs': [ '<(PRODUCT_DIR)/d3dcompiler_47.dll' ],
                                    'action':
                                    [
                                        "<(angle_gen_path)/copy_compiler_dll.bat",
                                        "$(PlatformName)",
                                        "<(windows_sdk_path)",
                                        "<(PRODUCT_DIR)"
                                    ],
                                },
                            ], #actions
                        }],
                        ['angle_build_winrt==1',
                        {
                            'type' : 'shared_library',
                        }],
                    ]
                },
            ], # targets
        }],
        ['angle_post_build_script!=0 and OS=="win"',
        {
            'targets':
            [
                {
                    'target_name': 'post_build',
                    'type': 'none',
                    'includes': [ '../build/common_defines.gypi', ],
                    'dependencies': [ 'libGLESv2', 'libEGL' ],
                    'actions':
                    [
                        {
                            'action_name': 'ANGLE Post-Build Script',
                            'message': 'Running <(angle_post_build_script)...',
                            'msvs_cygwin_shell': 0,
                            'inputs': [ '<(angle_post_build_script)', '<!@(["python", "<(angle_post_build_script)", "inputs", "<(angle_path)", "<(CONFIGURATION_NAME)", "$(PlatformName)", "<(PRODUCT_DIR)"])' ],
                            'outputs': [ '<!@(python <(angle_post_build_script) outputs "<(angle_path)" "<(CONFIGURATION_NAME)" "$(PlatformName)" "<(PRODUCT_DIR)")' ],
                            'action': ['python', '<(angle_post_build_script)', 'run', '<(angle_path)', '<(CONFIGURATION_NAME)', '$(PlatformName)', '<(PRODUCT_DIR)'],
                        },
                    ], #actions
                },
            ], # targets
        }],
    ] # conditions
}
