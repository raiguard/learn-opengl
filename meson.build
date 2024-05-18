project('learn-opengl', 'c', 'cpp',
        default_options: ['default_library=static', 'cpp_std=c++20'])

glad_includes = include_directories('glad/include')
glad = library('glad', 'glad/src/glad.c',
               include_directories: glad_includes)

glfw = dependency('glfw3')

# c = run_command('scripts/find_sources.sh', check: true)
# sources = c.stdout().strip().split('\n')
executable('learn-opengl', 'src/main.cpp',
           include_directories: glad_includes,
           link_with: [glad],
           dependencies: [glfw],
           cpp_args: '-Werror')
