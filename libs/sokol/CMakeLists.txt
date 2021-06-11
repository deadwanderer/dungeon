add_definitions(-D${sokol_backend})

fips_begin_lib(sokol)
  fips_vs_warning_level(4)
  fips_files(sokol.cpp)
fips_end_lib()

fips_begin_lib(sokol-mem)
  fips_vs_warning_level(4)
  fips_files(sokol-mem.cpp)
fips_end_lib()

fips_begin_lib(sokol-app)
  fips_vs_warning_level(4)
  fips_files(sokol-app.cpp)
fips_end_lib()

fips_begin_lib(sokol-app-mem)
  fips_vs_warning_level(4)
  fips_files(sokol-app-mem.cpp)
fips_end_lib()