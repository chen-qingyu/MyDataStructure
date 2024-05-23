set_project("pyincpp")
set_version("2.4.0")
set_license("GPL-3.0")
set_languages("cxx20")

add_rules("mode.debug", "mode.release")
add_requires("catch2")

target("test")
    set_kind("binary")
    add_packages("catch2")
    add_files("tests/*.cpp|compatibility.cpp")

target("bench")
    set_kind("binary")
    add_packages("catch2")
    add_files("benches/*.cpp")
