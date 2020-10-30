test_gepub = 'test-gepub'

executable(
  test_gepub,
  test_gepub + '.c',
  include_directories: top_inc,
  dependencies: [
    libgepub_dep,
    dependency('gtk+-3.0')
  ]
)

test_gepub_toc = 'test-gepub-toc'

executable(
  test_gepub_toc,
  test_gepub_toc + '.c',
  include_directories: top_inc,
  dependencies: [
    libgepub_dep,
    dependency('gtk+-3.0')
  ]
)