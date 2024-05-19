project('learn-opengl', 'c', 'cpp',
        default_options: ['default_library=static', 'cpp_std=c++20'])

glad_includes = include_directories('libraries/glad/include')
glad = library('glad', 'libraries/glad/src/glad.c',
               include_directories: glad_includes)

imgui = dependency('imgui')
sdl = dependency('sdl2')

c = run_command('scripts/find_sources.sh', check: true)
sources = c.stdout().strip().split('\n')
executable('learn-opengl', sources,
           include_directories: [glad_includes],
           link_with: [glad],
           dependencies: [imgui, sdl],
           cpp_args: ['-Wall', '-Wimplicit-fallthrough'])
