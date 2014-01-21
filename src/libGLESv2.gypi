# Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables':
    {
        'angle_enable_d3d9%': 1,
        'angle_enable_d3d11%': 1,
    },
    'target_defaults':
    {
        'defines':
        [
            'ANGLE_COMPILE_OPTIMIZATION_LEVEL=D3DCOMPILE_OPTIMIZATION_LEVEL1',
            'ANGLE_PRELOADED_D3DCOMPILER_MODULE_NAMES={ TEXT("d3dcompiler_46.dll"), TEXT("d3dcompiler_43.dll") }',
        ],
    },

    'conditions':
    [
        ['OS=="win"',
        {
            'targets':
            [
                {
                    'target_name': 'libGLESv2',
                    'type': 'shared_library',
                    'dependencies': [ 'translator', 'commit_id', 'copy_compiler_dll' ],
                    'include_dirs':
                    [
                        '.',
                        '../include',
                        'libGLESv2',
                        '<(SHARED_INTERMEDIATE_DIR)',
                    ],
                    'sources':
                    [
                        '<!@(python <(angle_build_scripts_path)/enumerate_files.py \
                             -dirs common libGLESv2 third_party/murmurhash ../include \
                             -types *.cpp *.h *.hlsl *.vs *.ps *.bat *.def *.rc \
                             -excludes */d3d/* */d3d9/* */d3d11/*)',
                    ],
                    # TODO(jschuh): http://crbug.com/167187 size_t -> int
                    'msvs_disabled_warnings': [ 4267 ],

                    'conditions':
                    [
                        ['angle_enable_d3d9==1',
                        {
                            'sources':
                            [
                                '<!@(python <(angle_build_scripts_path)/enumerate_files.py \
                                     -dirs libGLESv2/renderer/d3d libGLESv2/renderer/d3d9 \
                                     -types *.cpp *.h *.vs *.ps *.bat)',
                            ],
                            'defines':
                            [
                                'ANGLE_ENABLE_D3D9',
                            ],
                            'msvs_settings':
                            {
                                'VCLinkerTool':
                                {
                                    'AdditionalDependencies':
                                    [
                                        'd3d9.lib',
                                    ]
                                }
                            },
                        }],
                        ['angle_enable_d3d11==1',
                        {
                            'sources':
                            [
                                '<!@(python <(angle_build_scripts_path)/enumerate_files.py \
                                     -dirs libGLESv2/renderer/d3d libGLESv2/renderer/d3d11 \
                                     -types *.cpp *.h *.hlsl *.bat)',
                            ],
                            'defines':
                            [
                                'ANGLE_ENABLE_D3D11',
                            ],
                            'configurations':
                            {
                                'Debug':
                                {
                                    'msvs_settings':
                                    {
                                        'VCLinkerTool':
                                        {
                                            'AdditionalDependencies':
                                            [
                                                'dxguid.lib',
                                            ]
                                        }
                                    },
                                }
                            },
                        }],
                    ],

                    'configurations':
                    {
                        'Debug':
                        {
                            'defines':
                            [
                                'ANGLE_ENABLE_PERF',
                            ],
                            'msvs_settings':
                            {
                                'VCLinkerTool':
                                {
                                    'AdditionalDependencies':
                                    [
                                        'd3d9.lib',
                                    ]
                                }
                            },
                        },
                    },
                },
                {
                    'target_name': 'mapstest',
                    'type': 'executable',
                    'dependencies': [ 'libGLESv2', 'libEGL' ],
                    'include_dirs':
                    [
                        '../include',
                        'third_party/SDL2-2.0.1/include',
                        'third_party/glew-1.10.0/include',
                    ],
                    'sources': [ '<!@(python <(angle_build_scripts_path)/enumerate_files.py third_party/benvanik -types *.cc *.h)' ],
                    'libraries': [ 'third_party/sdl-build/Release/SDL2.lib' ],
                    'copies': [
                        { 'destination': '<(PRODUCT_DIR)', 'files': [ 'third_party/sdl-build/Release/SDL2.dll'] },
                        { 'destination': '<(PRODUCT_DIR)', 'files': [ 'third_party/benvanik/google-maps-20131028T165101.bin'] },
                    ],
                },
            ],
        },
        ],
    ],
}
